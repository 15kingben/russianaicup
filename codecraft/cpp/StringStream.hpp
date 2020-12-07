#ifndef _STRING_STREAM_HPP_
#define _STRING_STREAM_HPP_

#include "Stream.hpp"
#include <memory>
#include <string>

class StringStream : public OutputStream {
public:
    StringStream();
    void flush();
    void writeBytes(const char *buffer, size_t byteCount);
    void write(bool value);
    void write(int value);
    void write(long long value);
    void write(float value);
    void write(double value);
    void write(const std::string &value);
    std::string get();
    void writeString(const std::string &value);
private:
    std::string str;
};

#endif