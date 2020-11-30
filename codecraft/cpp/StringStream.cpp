#include "StringStream.hpp"
#include <memory>
#include <string>



StringStream::StringStream() {
    str = "";
}

void StringStream::flush() {
    str = "";
}

std::string StringStream::get() {
    return str;
}

void StringStream::writeBytes(const char *buffer, size_t byteCount) {
    for (int i = 0; i < byteCount; i++) {
        str.push_back(buffer[i]);
    }
}
