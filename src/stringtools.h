#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

#include <string>
#include <vector>

#include "Strings.h"
//typedef std::vector<std::string> Strings;

// simple string manipulations
std::string delSpaces(const std::string& s);
std::string trim(const std::string& s);

// reduce each occurence of char to one char
//   if a char is used as delimiter and may be doubled
//   this function
void reduceToOne(std::string& s, char c);

// replace each occurence of c1 with character c2
void substitute(std::string& s, char c1, char c2);

// split in parts
// uses del as delimiter to split a string in parts
// if expectedParts is given and not zero a wrong
// number of parts causes an exception
int split(const std::string& s, Strings& parts,
          char del, int expectedParts = 0);

// functions for parsing of string
// parsing starts at position i and sets i to the
// position after end of read part
// white space after read part is skipped

// skip white spaces
void skipWS(const std::string& s, unsigned  int& i);

// read int value
int getInt(const std::string& s, unsigned int& i);

// read long int value
long int getLongInt(const std::string& s, unsigned int& i);

// read word (series of letters)
std::string getWord(const std::string& s, unsigned int& i);

// pick all digits from a string and convert to long int
// quick and dirty solution to read formatted ints like "1,000,000"
long int getNumber(const std::string& l);

// compares first characters of s with the string start
bool startsWith(const std::string& s, const std::string& start);

// string to time
// converts a given string to a time period (in seconds)
time_t stot(const std::string& s);

// replaces in string template the placeholder string with the
// string given as content
// "my name is %name","%name","Wolfgang" -> "my name is Wolfgang"
void replacePlaceHolder(std::string& templ,
                        const std::string& placeholder,
                        const std::string& content);

#endif
