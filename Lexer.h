#ifndef PARSER_H
#define PARSER_H

#include <stdexcept>

#include "Exception.h"

#include <ctype.h>
#include <string>

class Lexer
{
public:
  static const int nothing = 0;
  static const int integer = 1;
  static const int floatingpoint = 2;
  static const int identifier = 4;
  static const int stringliteral = 8;
  static const int singlecharacter = 16;

  Lexer(const std::string& s, int start = 0):
    str(s), pos(start)
  {
    nextToken();
  }

  // all handled
  virtual bool empty() const
  {
    return token.empty() && pos >= str.size();
  }

  // get next token
  virtual void nextToken();

  virtual std::string getAll()
  {
    std::string res = str.substr(pos, str.size() - pos);
    pos = str.size();
    return res;
  }

  virtual long int getInt();
  virtual double getDouble();
  virtual std::string getString();
  virtual std::string getWord();

  // expect and remove specific token (string)
  virtual void expect(const std::string& tok)
  {
    if (token != tok)
      throw Exception("Parsing", "Expected " + tok);
    nextToken();
  }

  // expect and remove specific token (singlecharacter)
  virtual void expect(char tok)
  {
    if (type != singlecharacter || token[0] != tok)
      throw Exception("Parsing", std::string("Expected ") + tok);
    nextToken();
  }

  virtual std::string getString() const
  {
    return str;
  }

  std::string token;
  int type;

private:
  virtual char nextChar() const;
  virtual char getChar();
  virtual void skipChar();
  virtual void skipWhiteSpace();

  //  virtual void get_token();

  std::string str;

  unsigned int pos;
};

#endif
