#include "Exception.h"
#include "Strings.h"
#include "FileName.h"
#include "expiretools.h"
#include "filetools.h"

#include "Image.h"

using namespace std;

Image::Image(const string& dir): name(dir)
{
  FileName fn(name);
  string dummy;// image "name"
  stringToDate(fn.getName(), time, dummy);
  string expireFileName = name + "/expires";

  valid = !fileExists(name + "/error") &&
          fileExists(expireFileName) &&
          dirExists(name + "/tree");

  //  cout << boolalpha << dir << " " << image.valid << endl;

  if (valid)
    {
      Strings expireText;
      file2Strings(expireFileName, expireText);
      if (expireText.empty())
        throw Exception("expireDate", "expire empty");
      stringToDate(expireText[0], expire, series);
      expireRule = expireText[1];
    }
}

void Image::printInfo() const
{
  cout << name << endl;
  if (valid)
    {
      if (series != "expire")
        cout << "backup set: " << series << endl;
      cout << "created: " << time.getString('h') << endl;
      cout << "expires: " << expire.getString('h') << " -  " << timeString(expire - DateTime::now()) << endl;
    }

  else
    cout << "invalid" << endl;
}
