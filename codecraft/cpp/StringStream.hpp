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
    std::string get();
private:
    std::string str;
};

#endif