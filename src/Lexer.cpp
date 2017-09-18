#include <ctype.h>

#include <iostream>

#include "Lexer.h"

using namespace std;

char Lexer::nextChar() const
{
  if (pos < str.size())
    return str[pos];
  return 0;
}

char Lexer::getChar()
{
  if (pos < str.size())
    return str[pos++];
  return 0;
}

void Lexer::skipChar()
{
  if (pos < str.size())
    pos++;
}

void Lexer::skipWhiteSpace()
{
  if (pos < str.size() && isblank(str[pos]))
    pos++;
}

void Lexer::nextToken()
{
  token.clear();
  type = nothing;
  skipWhiteSpace();
  char f = getChar();
  if (f != 0)
    {
      token += f;
      if (isdigit(f))
        {
          // token is number
          type = integer; // assume "integer", changed later if necessarry
          // number
          f = nextChar();
          while (isdigit(f) || f == '.')
            {
              if (f == '.')
                {
                  type = floatingpoint;  // -> floating point
                }
              token += f;
              skipChar();
              f = nextChar();
            }
        }
      else if (f == '"')
        {
          // string literal starting with '"'
          char delimiter = f;
          token = "\"";
          type = stringliteral;

          f = nextChar();
          while (f != delimiter)
            {
              if (f == 0)
                throw Exception("Parsing", "string literal not complete");
              token += f;
              skipChar();
              f = nextChar();
            }
          token += '"';
          skipChar();
        }
      else if (isalpha(f))
        {
          // identifier
          type = identifier;
          f = nextChar();
          while (isalnum(f))
            {
              token += f;
              skipChar();
              f = nextChar();
            }
        }
      else
        {
          type = singlecharacter;
          // single character
        }
    }
  skipWhiteSpace();
}

long int Lexer::getInt()
{
  if (type != integer)
    throw Exception("getInt", "integer value expected");
  int res = stol(token);
  nextToken();
  return res;
}

double Lexer::getDouble()
{
  if (type != floatingpoint)
    throw Exception("getDouble", "floating point value expected");
  double res = stod(token);
  nextToken();
  return res;
}

std::string Lexer::getString()
{
  if (type != stringliteral)
    throw Exception("getString", "string expected");
  string res = token.substr(1, token.length() - 2);
  nextToken();
  return res;
}

std::string Lexer::getWord()
{
  if (type != identifier)
    throw Exception("getString", "identifier or keyword expected");
  string res = token;
  nextToken();
  return res;
}
