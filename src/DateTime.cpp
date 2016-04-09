#include <vector>

#include "stringtools.h"
#include "Exception.h"

#include "DateTime.h"

using namespace std;

void DateTime::get(int& year, int& month, int& mday,
                   int& hour, int& min, int& sec) const
{
  tm* ltm = localtime(&theTime);
  year = 1900 + ltm->tm_year;
  month = 1 + ltm->tm_mon;
  mday = ltm->tm_mday;
  hour =  ltm->tm_hour;
  min = ltm->tm_min;
  sec = ltm->tm_sec;
}

DateTime::DateTime(int Y, int M, int D, int h, int m, int s)
{
  struct tm ltm;
  ltm.tm_year = Y - 1900;
  ltm.tm_mon = M - 1;
  ltm.tm_mday = D;
  ltm.tm_min = m;
  ltm.tm_hour = h;
  ltm.tm_sec = s;
  ltm.tm_isdst = -1;
  theTime = mktime(&ltm);
}

#if 0
struct tm
{
  int tm_sec;    /* Seconds (0-60) */
  int tm_min;    /* Minutes (0-59) */
  int tm_hour;   /* Hours (0-23) */
  int tm_mday;   /* Day of the month (1-31) */
  int tm_mon;    /* Month (0-11) */
  int tm_year;   /* Year - 1900 */
  int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
  int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
  int tm_isdst;  /* Daylight saving time */
};
#endif

bool DateTime::match(const set<int>& Y, const set<int>& M, const set<int>& D,
                     const set<int>& W,
                     const set<int>& h, const set<int>& m, const set<int>& s) const
{
  tm* tm = localtime(&theTime);
  return match(Y, tm->tm_year) &&
         match(M, tm->tm_mon + 1) &&
         match(D, tm->tm_mday) &&
         match(W, tm->tm_wday) &&
         match(h, tm->tm_hour) &&
         match(m, tm->tm_min) &&
         match(s, tm->tm_sec);
}

using std::to_string;

std::string DateTime::toString2(int v)
{
  std::string res = to_string(v);
  if (res.size() < 2)
    res = "0" + res;
  return res;
}

std::string DateTime::getString(char typ) const
{
  int y, mo, d, h, mi, s;
  get(y, mo, d, h, mi, s);
  switch (typ)
    {
    case 'h': // human (german)
      return to_string(d) + "." + to_string(mo) + "." + to_string(y) + "  " +
             toString2(h) + ':' + toString2(mi) + ':' + toString2(s);
    case 'm': // machine
      return toString2(y) + "-" + toString2(mo) + "-" + toString2(d) + "-" +
             toString2(h) + '-' + toString2(mi) + '-' + toString2(s);
    case 's': // short machine
      return toString2(y) + "-" + toString2(mo) + "-" + toString2(d) + "-" +
             toString2(h);
      std::to_string(h);
    }
  throw Exception("DateTime", "wrong string format type");
}
