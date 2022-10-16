#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    if (seg.header().syn) {
        headno = seg.header().seqno + 1;
        _ackno = headno;
    }
    if (ackno().has_value()) {
        if (seg.header().seqno.raw_value() >= headno.raw_value() || seg.header().syn) {
            const auto& segment = seg;
            _reassembler.push_substring(segment.payload().copy(),
                                        seg.header().syn ? 0 : unwrap(segment.header().seqno,headno, 0),
                                        segment.header().fin);
            _ackno = WrappingInt32(headno + _reassembler.stream_out().bytes_written());
        }
        if (_reassembler.stream_out().input_ended()) {
            _ackno.value() = _ackno.value() + 1;
        }
    }

}

optional<WrappingInt32> TCPReceiver::ackno() const { return _ackno; }

size_t TCPReceiver::window_size() const {
    return _reassembler.stream_out().remaining_capacity();
}
