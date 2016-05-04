#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <set>
#include <string>
#include <iostream>
#include <time.h>

class DateTime
{
public:
  static DateTime now()
  {
    return DateTime(time(NULL));
  }

  DateTime(): theTime(0) {}

  explicit DateTime(time_t t): theTime(t) {}

  DateTime(int Y, int M, int D, int h, int m, int s);

  friend DateTime operator+(const DateTime& t1, time_t t2)
  {
    return DateTime(t1.theTime + t2);
  }

  const DateTime& operator+=(time_t t2)
  {
    theTime += t2;
    return *this;
  }

  void get(int& year, int& month, int& mday,
           int& hour, int& min, int& sec) const;

  std::string getString(char typ = 'h') const;

  const DateTime& operator-=(time_t t2)
  {
    theTime -= t2;
    return *this;
  }

  friend DateTime operator-(const DateTime& t1, time_t t2)
  {
    return DateTime(t1.theTime - t2);
  }

  friend time_t operator-(const DateTime& t1, const DateTime& t2)
  {
    return t1.theTime - t2.theTime;
  }

  friend bool operator<(const DateTime& t1, const DateTime& t2)
  {
    return t1.theTime < t2.theTime;
  }

  friend bool operator<=(const DateTime& t1, const DateTime& t2)
  {
    return t1.theTime <= t2.theTime;
  }
  friend bool operator==(const DateTime& t1, const DateTime& t2)
  {
    return t1.theTime == t2.theTime;
  }
  friend bool operator!=(const DateTime& t1, const DateTime& t2)
  {
    return t1.theTime != t2.theTime;
  }

  friend bool operator>(const DateTime& t1, const DateTime& t2)
  {
    return t1.theTime > t2.theTime;
  }

  friend bool operator>=(const DateTime& t1, const DateTime& t2)
  {
    return t1.theTime >= t2.theTime;
  }

  bool match(const std::set<int>& Y, const std::set<int>& M, const std::set<int>& D,
             const std::set<int>& W,
             const std::set<int>& h, const std::set<int>& m, const std::set<int>& s) const;

private:
  static bool match(const std::set<int>& v, int v2)
  {
    return (v.empty()) || (v.count(v2) > 0);
  }

  static std::string toString2(int v);
  time_t theTime;
};
#endif
