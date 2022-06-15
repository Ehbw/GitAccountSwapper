#pragma once
#include <iostream> 
#include <cstdlib>
#include <fstream>
#include <string>
#include <regex>

void LOG(const char* message);
std::string Combine(const std::string& a, const std::string& b);
bool IsStringPath(const std::string& str);
LPWSTR ConvertString(const std::string& instr);