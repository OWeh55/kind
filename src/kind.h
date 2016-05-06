#ifndef _KIND_H_
#define _KIND_H_

#include <string>

extern bool verbose;
extern bool debug;
extern bool quiet;

void verbosePrint(const std::string& text);

void debugPrint(const std::string& text);

#endif
