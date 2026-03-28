#pragma once
#include <iostream>
#include <fstream>
#include <string>

extern std::ofstream logFile;

inline void LOG(const std::string& msg) {
    std::cerr << msg << std::endl;
    if (logFile.is_open()) logFile << msg << std::endl;
}