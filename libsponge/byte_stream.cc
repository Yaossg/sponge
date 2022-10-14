#include "byte_stream.hh"

#include <cstring>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity)
    : buffer(capacity, '\0'), w_cursor(0), read_count(0), write_count(0), input_end(false) {}

size_t ByteStream::write(const string &data) {
    size_t written = min(remaining_capacity(), data.size());
    memcpy(buffer.data() + w_cursor, data.data(), written);
    w_cursor += written;
    write_count += written;
    return written;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t read = min(len, buffer_size());
    string result(read, '\0');
    memcpy(result.data(), buffer.data(), read);
    return result;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    w_cursor -= len;
    read_count += len;
    memmove(buffer.data(), buffer.data() + len, buffer_size());
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string result = peek_output(len);
    pop_output(len);
    return result;
}

void ByteStream::end_input() {
    input_end = true;
}

bool ByteStream::input_ended() const {
    return input_end;
}

size_t ByteStream::buffer_size() const {
    return w_cursor;
}

bool ByteStream::buffer_empty() const {
    return buffer_size() == 0;
}

bool ByteStream::eof() const {
    return input_ended() && buffer_empty();
}

size_t ByteStream::bytes_written() const {
    return write_count;
}

size_t ByteStream::bytes_read() const {
    return read_count;
}

size_t ByteStream::remaining_capacity() const {
    return buffer.size() - w_cursor;
}
