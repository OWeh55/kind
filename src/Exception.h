#ifndef IM_EXCEPTIONS_H
#define IM_EXCEPTIONS_H
#include <string>
#include <exception>

class Exception: public std::exception
{
protected:
  std::string where;
  std::string msg;
  mutable std::string fullMessage;
public:
  Exception(): where("unknown"), msg("unknown") {}
  Exception(const std::string& where, const std::string& msg):
    where(where), msg(msg) {}

  virtual void setWhere(const std::string& w)
  {
    where = w;
  }

  virtual const char* what() const noexcept
  {
    fullMessage = where + " - " + msg;
    return fullMessage.c_str();
  }

  virtual ~Exception() throw() {}
};
#endif
