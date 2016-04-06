#ifndef RULECOMP_H
#define RULECOMP_H

#include <set>
#include <string>
#include "Lexer.h"

std::set<int> getValues(const std::string& range, int firstvalue, int lastvalue, int altstringmode = 0);

std::set<int> getValues(Lexer& p, int firstvalue, int lastvalue, int altstringmode = 0);

#endif
