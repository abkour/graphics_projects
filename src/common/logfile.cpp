#include "logfile.hpp"

LogFile::LogFile(const char* filename) {
    ofs.open(filename, std::ios::binary);
}

LogFile::~LogFile() {
    ofs.close();
}

void LogFile::write_line(const char* str) {
    ofs << str << '\n';
}

void LogFile::write_raw(const char* data, std::size_t size) {
    ofs.write(data, size);
}