#include "Exception.h"
#include "Strings.h"
#include "FileName.h"
#include "expiretools.h"
#include "filetools.h"

#include "Image.h"

using namespace std;

Image::Image(const string& dir): name(dir)
{
  FileName fn(dir);
  string dummy;
  stringToDate(fn.getName(), time, dummy);
  string expireFileName = name + "/expires";
  valid = !fileExists(name + "/error") &&
          fileExists(expireFileName) &&
          dirExists(name + "/tree");

  //  cout << boolalpha << dir << " " << image.valid << endl;

  if (valid)
    {
      Strings ex;
      file2Strings(expireFileName, ex);
      if (ex.empty())
        throw Exception("expireDate", "expire empty");
      stringToDate(ex[0], expire, series);
      expireRule = ex[1];
    }
}
