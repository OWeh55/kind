#ifndef _FILENAME_H
#define _FILENAME_H

#include <vector>
#include <string>

class FileName
{
public:
  static const char pathdel = '/';
  static const char extdel = '.';

  FileName(): absolute(false) {};

  FileName(const std::string& filename);

  FileName(const std::string& path,
           const std::string& name,
           const std::string& ext);

  std::string getFileName() const;

  std::string getName() const
  {
    return name;
  }
  std::string getPath() const;
  std::string getExtension() const
  {
    return extension;
  }

  void setName(const std::string& n);
  void setPath(const std::string& p);
  void setExtension(const std::string& x);

private:
  bool absolute;
  std::vector<std::string> path;
  std::string name;
  std::string extension;
};

#endif
