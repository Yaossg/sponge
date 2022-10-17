#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>

#include <cassert>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , timeout_threshold(_initial_retransmission_timeout) {}

uint64_t TCPSender::bytes_in_flight() const {
    size_t result = 0;
    for (const auto& [_, segment] : history) {
        result += segment.length_in_sequence_space();
    }
    return result;
}

void TCPSender::fill_window() {
    while (!closed) {
        bool syn = _next_seqno == 0, fin = _stream.input_ended();
        size_t bound = _window_size;
        size_t length = min(min(_stream.buffer_size(), TCPConfig::MAX_PAYLOAD_SIZE) + syn, bound);
        if (fin && (fin = length < bound)) ++length;
        if (length == 0) break;
        auto segment = makeSegment(syn, fin, _stream.read(length - syn - fin));
        _window_size -= length;
        _segments_out.push(segment);
        _next_seqno += length;
        history[_next_seqno] = segment;
        if (fin) {
            closed = true;
        }
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t seqno = unwrap(ackno, _isn, 0);
    if (seqno > _next_seqno) return;
    if (window_size) {
        _window_size = window_size;
        if (seqno < _next_seqno) {
            _window_size -= _next_seqno - seqno;
        }
        echo = false;
    } else {
        _window_size = 1;
        echo = true;
    }
    auto first = history.begin(), last = history.upper_bound(seqno);
    if (first != last) {
        if (window_size) {
            timeout_threshold = _initial_retransmission_timeout;
            retrans = 0;
            timeout_timer = 0;
        }
        history.erase(first, last);
    }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    timeout_timer += ms_since_last_tick;
    if (timeout_timer >= timeout_threshold) {
        ++retrans;
        if (!echo)
            timeout_threshold *= 2;
        timeout_timer = 0;
        if (!history.empty())
            _segments_out.push(history.begin()->second);
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return retrans; }

void TCPSender::send_empty_segment() {
    _segments_out.push(makeSegment(false, false, ""));
}

TCPSegment TCPSender::makeSegment(bool syn, bool fin, string payload) {
    TCPSegment segment;
    segment.header().syn = syn;
    segment.header().seqno = wrap(_next_seqno, _isn);
    segment.header().fin = fin;
    segment.payload() = Buffer(std::move(payload));
    return segment;
}