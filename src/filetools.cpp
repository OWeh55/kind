#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fstream>

#include "Exception.h"

#include "filetools.h"

std::string concatPathAndFilename(const std::string& path, const std::string& filename)
{
  std::string result = path;
  if (result[result.length() - 1] != '/')
    result += "/";
  result += filename;
  return result;
}

void fileList(const std::string& fn,
              std::vector<std::string>& file, bool recursive)
{
  struct stat fstat;
  if (stat(fn.c_str(), &fstat) != 0)
    return;
  if (S_ISREG(fstat.st_mode))
    file.push_back(fn);
  else if (S_ISDIR(fstat.st_mode))
    {
      struct dirent*  dp;
      DIR* dir;
      if ((dir = opendir(fn.c_str())) != NULL)
        {
          /* Loop through directory entries. */
          while ((dp = readdir(dir)) != NULL)
            {
              std::string fname = dp->d_name;
              std::string pname = concatPathAndFilename(fn, fname);
              if (stat(pname.c_str(), &fstat) == 0)
                {
                  if (S_ISDIR(fstat.st_mode))
                    {
                      if (recursive)
                        if (fname != "." && fname != "..")
                          fileList(pname, file, true);
                    }
                  else if (S_ISREG(fstat.st_mode))
                    file.push_back(pname);
                }
            }
          closedir(dir);
        }
    }
}

void dirList(const std::string& fn,
             std::vector<std::string>& file, bool recursive)
{
  struct stat fstat;
  if (stat(fn.c_str(), &fstat) != 0)
    return;
  if (S_ISREG(fstat.st_mode))
    file.push_back(fn);
  else if (S_ISDIR(fstat.st_mode))
    {
      struct dirent*  dp;
      DIR* dir;
      if ((dir = opendir(fn.c_str())) != NULL)
        {
          /* Loop through directory entries. */
          while ((dp = readdir(dir)) != NULL)
            {
              std::string fname = dp->d_name;
              std::string pname = concatPathAndFilename(fn, fname);
              if (stat(pname.c_str(), &fstat) == 0)
                {
                  if (S_ISDIR(fstat.st_mode))
                    {
                      if (fname != "." && fname != "..")
                        {
                          file.push_back(pname);
                          if (recursive)
                            dirList(pname, file, true);
                        }
                    }
                }
            }
          closedir(dir);
        }
    }
}

void fileList(int argc, char** argv,
              int optind, std::vector<std::string>& file, bool recursive)
{
  for (int i = optind; i < argc; i++)
    fileList(argv[i], file, recursive);
}

void strings2File(const std::vector<std::string>& s, const std::string& fn)
{
  std::ofstream os(fn);
  if (!os.good())
    throw Exception("strings to file", "Cannot open file " + fn);
  for (std::string t : s)
    os << t << std::endl;
  os.close();
}

void file2Strings(const std::string& fn, std::vector<std::string>& s)
{
  std::ifstream is(fn);
  if (!is.good())
    throw Exception("file to strings", "Cannot open file " + fn);
  std::string input;
  while (getline(is, input))
    s.push_back(input);
}

bool fileExists(const std::string& name)
{
  struct stat fstat;
  if (stat(name.c_str(), &fstat) != 0)
    return false;
  if (!S_ISREG(fstat.st_mode))
    return false;
  return true;
}

bool dirExists(const std::string& name)
{
  struct stat fstat;
  if (stat(name.c_str(), &fstat) != 0)
    return false;
  if (!S_ISDIR(fstat.st_mode))
    return false;
  return true;
}

int lock_fd;

void createLock(const std::string& lockfilename)
{
  lock_fd = open(lockfilename.c_str(), O_RDWR | O_CREAT, 0666); // open or create lockfile
  //check open success...
  int rc = flock(lock_fd, LOCK_EX | LOCK_NB); // grab exclusive lock, fail if can't obtain.
  if (rc)
    {
      std::cerr << "ERROR: cannot get lock at " + lockfilename << std::endl;
      exit(1);
      // no exception, because at normal end of program lock is
      // unlocked and removed
    }
}

void removeLock(const std::string& lockfilename)
{
  flock(lock_fd, LOCK_UN);
  unlink(lockfilename.c_str());
}

int removeDir(const std::string& path)
{
  DIR* d = opendir(path.c_str());

  int r = -1;
  if (d)
    {
      struct dirent* p;

      r = 0;
      while (!r && (p = readdir(d)))
        {
          int r2 = 0;
          std::string fn = p->d_name;
          if (fn != "." && fn != "..")
            {
              fn = path + "/" + fn;
              struct stat statbuf;
              if (lstat(fn.c_str(), &statbuf) == 0)
                {
                  if (S_ISLNK(statbuf.st_mode))
                    r2 = unlink(fn.c_str());
                  else if (S_ISDIR(statbuf.st_mode))
                    r2 = removeDir(fn);
                  else
                    r2 = unlink(fn.c_str());
                }
              else
                {
                  std::cout << "stat(" << fn << ") failed" << std::endl;
                  // we assume "file" here
                  r2 = unlink(fn.c_str());
                }
            }
          r = r2;
        }
      closedir(d);
    }

  if (r == 0)
    r = rmdir(path.c_str());

  return r;
}

#define POPEN_BUFFER_SIZE 2000
Strings localExec(const std::string& cmd,
                  int& rc, bool debug,
                  const std::string& logfn)
{
  if (debug)
    std::cout << "Executing " << cmd << std::endl;
  std::ofstream log;
  if (!logfn.empty())
    log.open(logfn);

  if (log.is_open())
    {
      log << "Executing " << cmd << std::endl;
      log << "--------------------------------------------------------" << std::endl;
    }
  Strings res;
  FILE* fd = popen(cmd.c_str(), "r");
  if (fd != nullptr)
    {
      char buffer[POPEN_BUFFER_SIZE];
      while (fgets(buffer, POPEN_BUFFER_SIZE - 1, fd))
        {
          buffer[POPEN_BUFFER_SIZE - 1] = 0; // force string end here
          int size = strlen(buffer);
          if (buffer[size - 1] == '\n') // substitute linefeed with string end
            buffer[size - 1] = 0;
          std::string input = buffer;
          if (log.is_open())
            log << input << std::endl;
          res.push_back(input);
          if (debug)
            std::cout << ">>" << input << "<<" << std::endl;
        }
      rc = pclose(fd);
      if (debug)
        std::cout << "    result code: " << rc << std::endl;
    }
  else
    {
      if (debug)
        std::cout << "    popen " << cmd  << " failed " << std::endl;
      rc = 1;
      return res;
    }

  if (log.is_open())
    {
      log << "--------------------------------------------------------" << std::endl;
      log << "result code: " << rc << std::endl;
    }
  return res;
}

Strings remoteExec(const std::string& rshCommand,
                   const std::string& cmd,
                   int& rc, bool debug,
                   const std::string& logfn)
{

  return localExec(rshCommand + " " + cmd, rc, debug, logfn);
}
