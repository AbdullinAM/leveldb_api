//
// Created by abdullin on 2/19/16.
//

#ifndef LEVELDB_API_LOGGER_H
#define LEVELDB_API_LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

namespace logging {

class Logger {

public:

    Logger() {
        logstream.open("leveldb-wrapper.log", std::ios::app);
    }

    Logger(const std::string& logfile) {
        logstream.open(logfile);
    }

    void print(const std::string& messg) {
        time_t timev;
        time(&timev);
        printTime(timev);
        logstream << messg << std::endl;
    }

    void print(const char* messg) {
        time_t timev;
        time(&timev);
        printTime(timev);
        logstream << messg << std::endl;
    }

    void print(const int messg) {
        time_t timev;
        time(&timev);
        printTime(timev);
        logstream << messg << std::endl;
    }

    ~Logger() {
        logstream.close();
    }

private:

    void printTime(time_t time) {
        struct tm* local_time;
        local_time = localtime(&time);
        int Day    = local_time->tm_mday;
        int Month  = local_time->tm_mon + 1;
        int Year   = local_time->tm_year + 1900;
        int Hour   = local_time->tm_hour;
        int Min    = local_time->tm_min;
        int Sec    = local_time->tm_sec;
        logstream << "[" << Year << "-" << Month << "-" << Day << " " << Hour << ":" << Min << ":" << Sec << "] ";
    }

    std::ofstream logstream;

};

}   /* namespace logging */

#endif //LEVELDB_API_LOGGER_H
