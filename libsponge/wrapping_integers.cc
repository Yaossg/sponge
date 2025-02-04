#include "wrapping_integers.hh"

#include <algorithm>

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    return WrappingInt32{static_cast<uint32_t>(n + isn.raw_value())};
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.

uint64_t diff(uint64_t a, uint64_t b) {
    return a > b ? a - b : b - a;
}

uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint32_t lo = n.raw_value() - isn.raw_value();
    uint64_t hi = checkpoint >> 32;
    uint64_t m1 = ((hi - 1) << 32) | lo;
    uint64_t m2 = (hi << 32) | lo;
    uint64_t m3 = ((hi + 1) << 32) | lo;
    uint64_t d1 = diff(m1, checkpoint);
    uint64_t d2 = diff(m2, checkpoint);
    uint64_t d3 = diff(m3, checkpoint);
    uint64_t m = min({d1, d2, d3});
    if (m == d1) return m1;
    if (m == d2) return m2;
    /*if (m == d3)*/ return m3;
}
