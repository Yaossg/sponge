#include "stream_reassembler.hh"

#include <cstring>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), buffer(capacity, '\0'), flags(capacity, '\0') {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (eof) {
        eof_index = index + data.length();
    }
    if (!data.empty()) {
        if (index + data.length() <= cursor) {
            // do nothing
        } else if (index < cursor) {
            push_substring(data.substr(cursor - index), cursor, eof);
        } else {
            size_t local = index - cursor;
            size_t len = min(data.length(), _capacity - local);
            memcpy(buffer.data() + local, data.data(), len);
            memset(flags.data() + local, 1, len);

            size_t ones = 0;
            while (ones < flags.size() && flags.at(ones)) ++ones;
            size_t written = _output.write(buffer.substr(0, ones));
            cursor += written;

            memmove(buffer.data(), buffer.data() + written, buffer.size() - written);
            memmove(flags.data(), flags.data() + written, flags.size() - written);
            memset(flags.data() + flags.size() - written, 0, written);
        }

    }
    if (_output.bytes_written() == eof_index) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t result = 0;
    for (char flag : flags) if (flag) ++result;
    return result;
}

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }
