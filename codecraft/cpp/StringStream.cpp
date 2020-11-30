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

void StringStream::write(int value) {
    str += std::to_string(value) + " ";
}

void StringStream::write(long long value) {
    str += std::to_string(value) + " ";
}
void StringStream::write(bool value) {
    str += std::to_string(value) + " ";
}
void StringStream::write(float value) {
    str += std::to_string(value) + " ";
}
void StringStream::write(double value) {
    str += std::to_string(value) + " ";
}
void StringStream::write(const std::string &value) {
    str += value + " ";
}