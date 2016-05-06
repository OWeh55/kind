#ifndef KIND_IMAGE_H
#define KIND_IMAGE_H

#include <string>
#include <vector>

#include "DateTime.h"
#include "expiretools.h"

class Image
{
public:
  Image(const std::string& dir);

  std::string name;
  DateTime time;
  std::string series;
  bool valid;
  DateTime expire;
  std::string expireRule;

  void printInfo() const;

  friend bool operator<(const Image& i1, const Image& i2)
  {
    return i1.time < i2.time;
  }
};

typedef std::vector<Image> Images;

#endif
