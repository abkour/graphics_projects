#ifndef _LOGFILE_HPP_
#define _LOGFILE_HPP_
#include <fstream>

class LogFile {

public:

    LogFile(const char* filename);
    ~LogFile();

    void write_line(const char* str);
    void write_raw(const char* data, const std::size_t size);

private:

    std::ofstream ofs;
};

#endif