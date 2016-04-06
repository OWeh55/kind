#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

#include <string>
#include <vector>

#include "Strings.h"
//typedef std::vector<std::string> Strings;

// simple string manipulations
std::string delSpaces(const std::string& s);
std::string trim(const std::string& s);

void reduceToOne(std::string& s, char c);
void substitute(std::string& s, char c1, char c2);

// split in parts
int split(const std::string& s, Strings& parts,
          char del, int expectedParts = 0);

// string parsing
void skipWS(const std::string& s, unsigned  int& i);
int getInt(const std::string& s, unsigned int& i);
long int getLongInt(const std::string& s, unsigned int& i);
std::string getWord(const std::string& s, unsigned int& i);

long int getNumber(const std::string& l);

bool startsWith(const std::string& s, const std::string& start);

// string to time
time_t stot(const std::string& s);

void replacePlaceHolder(std::string& format,
                        const std::string& placeholder,
                        const std::string& content);

#endif
