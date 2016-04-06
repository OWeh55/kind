# 1 "kind.ag"
#include <dirent.h>
# 2 "kind.ag"
#include <sys/stat.h>
# 3 "kind.ag"
#include <cstring>
# 4 "kind.ag"
#include <unistd.h>
# 5 "kind.ag"

# 6 "kind.ag"
#include <iostream>
# 7 "kind.ag"
#include <fstream>
# 8 "kind.ag"

# 9 "kind.ag"
#include <string>
# 10 "kind.ag"
#include <vector>
# 11 "kind.ag"
#include <set>
# 12 "kind.ag"

# 13 "kind.ag"
#include <algorithm>
# 14 "kind.ag"

# 15 "kind.ag"
#include "stringtools.h"
# 16 "kind.ag"
#include "Exception.h"
# 17 "kind.ag"

# 18 "kind.ag"
#include "DateTime.h"
# 19 "kind.ag"
#include "FileName.h"
# 20 "kind.ag"
#include "KindConfig.h"
# 21 "kind.ag"
#include "filetools.h"
# 22 "kind.ag"
#include "Lexer.h"
# 23 "kind.ag"
#include "rulecomp.h"
# 24 "kind.ag"
#include "Strings.h"
# 25 "kind.ag"

# 26 "kind.ag"
/*AppGen
# 27 "kind.ag"
  %%  Beschreibung des Programmes:
# 28 "kind.ag"
  prog: archiving backup
# 29 "kind.ag"
  %% Beschreibung Parameter
# 30 "kind.ag"
  % symbolischerName, Art, Typ,   Variablenname, Erklärung, Default-Wert
# 31 "kind.ag"
  para: vault_or_group, required, string, vault, Vault to backup
# 32 "kind.ag"
  %% Beschreibung der Optionen
# 33 "kind.ag"
  % kurz-Option, lang-Option, Typ, Variablenname, Erklärung, Default-Wert
# 34 "kind.ag"

# 35 "kind.ag"
  opt: f, full, void, fullImage, Force full image == initial backup, false
# 36 "kind.ag"
  opt: c, masterconfig, string, masterConfig, Master config file, ""
# 37 "kind.ag"
  opt2: if not given or empty kind looks for
# 38 "kind.ag"
  opt2:   /etc/kind/master.conf
# 39 "kind.ag"
  opt2:   /ffp/etc/kind/master.conf
# 40 "kind.ag"
  opt: B, backuponly, void, backupOnly, Only backup/no expire, false
# 41 "kind.ag"
  opt: E, expireonly, void, expireOnly, Only expire/no backup, false
# 42 "kind.ag"
  opt: D, dryrun, Void, dryRun, Dry run (no real backup), false
# 43 "kind.ag"
  opt: v, verbose, Void, verbose,  Verbose,  false
# 44 "kind.ag"
  opt: d, debug, Void, debug, Debug output of many data, false
# 45 "kind.ag"
  opt: q, quiet, Void, quiet, Be quiet - no messages, false
# 46 "kind.ag"
  opt: h, help, usage, ignored , This help
# 47 "kind.ag"
AppGen*/
# 48 "kind.ag"

# 49 "kind.ag"
using namespace std;
# 50 "kind.ag"

# 51 "kind.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <string>
bool dryRun = false;
bool verbose = false;
bool debug = false;
bool quiet = false;
/*AppGen:GlobalEnd*/
# 52 "kind.ag"

# 53 "kind.ag"
vector<string> banks;
# 54 "kind.ag"

# 55 "kind.ag"
typedef pair<long int, long int> Sizes;
# 56 "kind.ag"
map<string, Sizes> sizes;
# 57 "kind.ag"

# 58 "kind.ag"
void verbosePrint(const string& text)
# 59 "kind.ag"
{
# 60 "kind.ag"
  if (verbose)
# 61 "kind.ag"
    cout << "  " << text << endl;
# 62 "kind.ag"
}
# 63 "kind.ag"

# 64 "kind.ag"
void debugPrint(const string& text)
# 65 "kind.ag"
{
# 66 "kind.ag"
  if (verbose)
# 67 "kind.ag"
    cout << "    " << text << endl;
# 68 "kind.ag"
}
# 69 "kind.ag"

# 70 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 71 "kind.ag"
{
# 72 "kind.ag"
  verbosePrint("reading global config " + fn);
# 73 "kind.ag"
  conf.addFile(fn);
# 74 "kind.ag"
  banks = conf.getStrings("bank");
# 75 "kind.ag"
  if (banks.empty())
# 76 "kind.ag"
    throw Exception("read main config", "no banks defined");
# 77 "kind.ag"
}
# 78 "kind.ag"

# 79 "kind.ag"
string findVault(const string& v)
# 80 "kind.ag"
{
# 81 "kind.ag"
  bool found = false;
# 82 "kind.ag"
  FileName fn;
# 83 "kind.ag"
  fn.setName(v);
# 84 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 85 "kind.ag"
    {
# 86 "kind.ag"
      fn.setPath(banks[i]);
# 87 "kind.ag"
      if (dirExists(fn.getFileName()))
# 88 "kind.ag"
        found = true;
# 89 "kind.ag"
    }
# 90 "kind.ag"
  if (!found)
# 91 "kind.ag"
    throw Exception("find vault", v + " not found");
# 92 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 93 "kind.ag"
  return fn.getFileName();
# 94 "kind.ag"
}
# 95 "kind.ag"

# 96 "kind.ag"
void readVaultConfig(const string& vaultConfigName, KindConfig& conf)
# 97 "kind.ag"
{
# 98 "kind.ag"
  FileName fn(vaultConfigName);
# 99 "kind.ag"

# 100 "kind.ag"
  verbosePrint("reading vault config " + fn.getFileName());
# 101 "kind.ag"

# 102 "kind.ag"
  conf.addFile(fn.getFileName());
# 103 "kind.ag"
}
# 104 "kind.ag"

# 105 "kind.ag"
string getImageName(const KindConfig& conf)
# 106 "kind.ag"
{
# 107 "kind.ag"
  bool nonPortable = false;
# 108 "kind.ag"
  string res = conf.getString("imageName");
# 109 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < res.size(); ++i)
# 110 "kind.ag"
    {
# 111 "kind.ag"
      char c = res[i];
# 112 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 113 "kind.ag"
        nonPortable = true;
# 114 "kind.ag"
    }
# 115 "kind.ag"
  if (nonPortable)
# 116 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + res);
# 117 "kind.ag"
  return res;
# 118 "kind.ag"
}
# 119 "kind.ag"

# 120 "kind.ag"
bool isValidImage(const string& imageName)
# 121 "kind.ag"
{
# 122 "kind.ag"
  return dirExists(imageName) &&
# 123 "kind.ag"
         !fileExists(imageName + "/error") &&
# 124 "kind.ag"
         dirExists(imageName + "/tree");
# 125 "kind.ag"
}
# 126 "kind.ag"

# 127 "kind.ag"
Strings findValidImages(const string& vaultpath, const KindConfig& conf)
# 128 "kind.ag"
{
# 129 "kind.ag"
  Strings imageList;
# 130 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 131 "kind.ag"
  dirList(vaultpath, imageList);
# 132 "kind.ag"

# 133 "kind.ag"
  Strings validImageList;
# 134 "kind.ag"
  for (unsigned int i = 0; i < imageList.size(); ++i)
# 135 "kind.ag"
    {
# 136 "kind.ag"
      FileName fn(imageList[i]);
# 137 "kind.ag"
      string imgname = getImageName(conf);
# 138 "kind.ag"
      int len = imgname.length();
# 139 "kind.ag"
      if (fn.getName().substr(0, len) == imgname)
# 140 "kind.ag"
        {
# 141 "kind.ag"
          debugPrint("Checking " + imageList[i]);
# 142 "kind.ag"
          if (isValidImage(imageList[i]))
# 143 "kind.ag"
            validImageList.push_back(imageList[i]);
# 144 "kind.ag"
        }
# 145 "kind.ag"
    }
# 146 "kind.ag"
  if (validImageList.empty())
# 147 "kind.ag"
    throw Exception("Find reference", "No reference found");
# 148 "kind.ag"
  sort(validImageList.begin(), validImageList.end());
# 149 "kind.ag"
  return validImageList;
# 150 "kind.ag"
}
# 151 "kind.ag"

# 152 "kind.ag"
void backupVault(const string& vault,
# 153 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 154 "kind.ag"
                 const DateTime& imageTime,
# 155 "kind.ag"
                 bool fullImage)
# 156 "kind.ag"
{
# 157 "kind.ag"
  if (!quiet)
# 158 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 159 "kind.ag"
  try
# 160 "kind.ag"
    {
# 161 "kind.ag"
      sizes[vault].second = 0; // nothing backed up yet
# 162 "kind.ag"
      string vaultpath = findVault(vault);
# 163 "kind.ag"
      const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 164 "kind.ag"
      readVaultConfig(vaultConfigName, conf);
# 165 "kind.ag"
      if (debug)
# 166 "kind.ag"
        {
# 167 "kind.ag"
          cout << "vault config:" << endl;
# 168 "kind.ag"
          conf.print();
# 169 "kind.ag"
        }
# 170 "kind.ag"

# 171 "kind.ag"
      string imageName = getImageName(conf);
# 172 "kind.ag"
      if (!imageName.empty())
# 173 "kind.ag"
        imageName += '-';
# 174 "kind.ag"
      string imageFullName =  vaultpath + "/" + imageName ;
# 175 "kind.ag"

# 176 "kind.ag"
      if (conf.getBool("longImageName"))
# 177 "kind.ag"
        imageFullName += imageTime.getString('m');
# 178 "kind.ag"
      else
# 179 "kind.ag"
        imageFullName += imageTime.getString('s');
# 180 "kind.ag"

# 181 "kind.ag"
      verbosePrint("backup to \"" + imageFullName + "\"");
# 182 "kind.ag"

# 183 "kind.ag"
      // find reference image
# 184 "kind.ag"
      string referenceImage;
# 185 "kind.ag"
      if (!fullImage)
# 186 "kind.ag"
        {
# 187 "kind.ag"
          Strings validImageList = findValidImages(vaultpath, conf);
# 188 "kind.ag"
          // last image is newest image
# 189 "kind.ag"
          referenceImage = validImageList.back();
# 190 "kind.ag"
        }
# 191 "kind.ag"

# 192 "kind.ag"
      // create image path
# 193 "kind.ag"
      if (!dryRun)
# 194 "kind.ag"
        if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 195 "kind.ag"
          throw Exception("Create image", "failed to create " + imageFullName);
# 196 "kind.ag"

# 197 "kind.ag"
      // error message
# 198 "kind.ag"
      // we write an generic error message to mark backup as unsuccessful
# 199 "kind.ag"
      // will be deleted at successful end of rsync
# 200 "kind.ag"
      string errorfile = imageFullName + "/error";
# 201 "kind.ag"
      if (!dryRun)
# 202 "kind.ag"
        {
# 203 "kind.ag"
          ofstream error(errorfile);
# 204 "kind.ag"
          error << "failed" << endl;
# 205 "kind.ag"
          error.close();
# 206 "kind.ag"
        }
# 207 "kind.ag"

# 208 "kind.ag"
      string host;
# 209 "kind.ag"
      if (conf.hasKey("host"))
# 210 "kind.ag"
        host = conf.getString("host");
# 211 "kind.ag"

# 212 "kind.ag"
      string server;
# 213 "kind.ag"
      if (conf.hasKey("server"))
# 214 "kind.ag"
        server = conf.getString("server");
# 215 "kind.ag"

# 216 "kind.ag"
      if (!host.empty() && !server.empty())
# 217 "kind.ag"
        throw Exception("backupVault", "Cannot have host and server");
# 218 "kind.ag"

# 219 "kind.ag"
      string path = conf.getString("path");
# 220 "kind.ag"
      if (path.empty())
# 221 "kind.ag"
        throw Exception("rsync", "empty source path");
# 222 "kind.ag"
      if (path.back() != '/')
# 223 "kind.ag"
        path += '/';
# 224 "kind.ag"

# 225 "kind.ag"
      string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 226 "kind.ag"
      if (!conf.getBool("ignorePermission"))
# 227 "kind.ag"
        rsyncCmd += "-pgo";
# 228 "kind.ag"
      vector<string> rso = conf.getStrings("rsyncOption");
# 229 "kind.ag"
      for (string opt : rso)
# 230 "kind.ag"
        rsyncCmd += opt + " ";
# 231 "kind.ag"

# 232 "kind.ag"
      if (!host.empty())  // shell mode
# 233 "kind.ag"
        {
# 234 "kind.ag"
          // cout << "USING SHELLMODE '" << host << "'" << endl;
# 235 "kind.ag"
          string remoteShell = conf.getString("remoteShell");
# 236 "kind.ag"
          string userAtHost = conf.getString("user") + "@" +
# 237 "kind.ag"
                              conf.getString("host");
# 238 "kind.ag"
          string rshCommand = remoteShell;
# 239 "kind.ag"
          if (remoteShell.empty())
# 240 "kind.ag"
            rshCommand = "ssh";
# 241 "kind.ag"

# 242 "kind.ag"
          rshCommand += " " + userAtHost;
# 243 "kind.ag"

# 244 "kind.ag"
          // excludes
# 245 "kind.ag"
          Strings excluded;
# 246 "kind.ag"

# 247 "kind.ag"
          string userExcludeCommand = conf.getString("userExcludeCommand");
# 248 "kind.ag"

# 249 "kind.ag"
          if (!userExcludeCommand.empty())
# 250 "kind.ag"
            {
# 251 "kind.ag"
              replacePlaceHolder(userExcludeCommand, "%path", path);
# 252 "kind.ag"
              string excludeCommand = rshCommand + " " + userExcludeCommand;
# 253 "kind.ag"

# 254 "kind.ag"
              verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 255 "kind.ag"

# 256 "kind.ag"
              int rc;
# 257 "kind.ag"
              excluded = myPopen(excludeCommand, rc, debug);
# 258 "kind.ag"
              if (rc > 0)
# 259 "kind.ag"
                throw Exception("Find exludes", "Search for excludes failed");
# 260 "kind.ag"

# 261 "kind.ag"
              for (unsigned int i = 0; i < excluded.size(); ++i)
# 262 "kind.ag"
                {
# 263 "kind.ag"
                  FileName fn(excluded[i]);
# 264 "kind.ag"
                  excluded[i] = '/' + fn.getPath();
# 265 "kind.ag"
                  debugPrint("Excluding: " + excluded[i]);
# 266 "kind.ag"
                }
# 267 "kind.ag"
            }
# 268 "kind.ag"

# 269 "kind.ag"
          string userExcludeFile = conf.getString("userExcludeFile");
# 270 "kind.ag"
          if (!userExcludeFile.empty())
# 271 "kind.ag"
            {
# 272 "kind.ag"
              userExcludeFile = path + userExcludeFile;
# 273 "kind.ag"
              string getExcludeFileCommand = rshCommand;
# 274 "kind.ag"
              getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 275 "kind.ag"
              getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 276 "kind.ag"
              // cout << getExcludeFileCommand << endl;
# 277 "kind.ag"
              int rc;
# 278 "kind.ag"
              Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 279 "kind.ag"
              if (rc == 0)
# 280 "kind.ag"
                excluded += excludes2;
# 281 "kind.ag"
            }
# 282 "kind.ag"

# 283 "kind.ag"
          if (conf.hasKey("exclude"))
# 284 "kind.ag"
            excluded += conf.getStrings("exclude");
# 285 "kind.ag"

# 286 "kind.ag"
          if (!dryRun)
# 287 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 288 "kind.ag"

# 289 "kind.ag"
          // rsync image
# 290 "kind.ag"

# 291 "kind.ag"
          if (!remoteShell.empty())
# 292 "kind.ag"
            rsyncCmd += " -e \'" + remoteShell + "\' ";
# 293 "kind.ag"

# 294 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 295 "kind.ag"
          if (!referenceImage.empty())
# 296 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 297 "kind.ag"
          rsyncCmd += userAtHost + ":" + path + " ";
# 298 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 299 "kind.ag"
        } // shell mode
# 300 "kind.ag"
      else
# 301 "kind.ag"
        {
# 302 "kind.ag"
          // cout << "USING SERVERMODE" << endl;
# 303 "kind.ag"
          vector<string> excluded;
# 304 "kind.ag"
          if (conf.hasKey("exclude"))
# 305 "kind.ag"
            {
# 306 "kind.ag"
              Strings excludes = conf.getStrings("exclude");
# 307 "kind.ag"
              for (string s : excludes)
# 308 "kind.ag"
                excluded.push_back(s);
# 309 "kind.ag"
            }
# 310 "kind.ag"

# 311 "kind.ag"
          if (!dryRun)
# 312 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 313 "kind.ag"

# 314 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 315 "kind.ag"
          if (!referenceImage.empty())
# 316 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 317 "kind.ag"
          rsyncCmd += conf.getString("server") + "::" + path + " ";
# 318 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 319 "kind.ag"
        }
# 320 "kind.ag"

# 321 "kind.ag"
      debugPrint("Action: " + rsyncCmd);
# 322 "kind.ag"

# 323 "kind.ag"
      vector<string> backupResult;
# 324 "kind.ag"
      if (!dryRun)
# 325 "kind.ag"
        {
# 326 "kind.ag"
          verbosePrint("syncing (" + rsyncCmd + ")");
# 327 "kind.ag"
          int rc;
# 328 "kind.ag"
          backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 329 "kind.ag"
          // strings2File(backupResult, imageFullName + "/rsync-log");
# 330 "kind.ag"
          if (rc == 0 || rc == 24) // "no error" or "vanished source files" (ignored)
# 331 "kind.ag"
            {
# 332 "kind.ag"
              unlink(errorfile.c_str());
# 333 "kind.ag"
              string lastLink = vaultpath + "/last";
# 334 "kind.ag"
              unlink(lastLink.c_str());
# 335 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 336 "kind.ag"
              long int st = 0;
# 337 "kind.ag"
              long int sc = 0;
# 338 "kind.ag"
              for (auto bl : backupResult)
# 339 "kind.ag"
                {
# 340 "kind.ag"
                  if (bl.substr(0, 15) == "Total file size")
# 341 "kind.ag"
                    st = getNumber(bl);
# 342 "kind.ag"
                  else if (bl.substr(0, 27) == "Total transferred file size")
# 343 "kind.ag"
                    sc = getNumber(bl);
# 344 "kind.ag"
                }
# 345 "kind.ag"
              // sizes[vault] = pair<long int, long int>(st, sc);
# 346 "kind.ag"
              sizes[vault] = Sizes(st, sc);
# 347 "kind.ag"
              //  cout << vault << " " << st << " || " << sc << endl;
# 348 "kind.ag"
            }
# 349 "kind.ag"
          else
# 350 "kind.ag"
            throw Exception("Backup", "Failed to execute rsync");
# 351 "kind.ag"
        }
# 352 "kind.ag"
      else
# 353 "kind.ag"
        cout << "Not executing " << rsyncCmd << endl;
# 354 "kind.ag"
    }
# 355 "kind.ag"
  catch (Exception ex)
# 356 "kind.ag"
    {
# 357 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 358 "kind.ag"
    }
# 359 "kind.ag"
}
# 360 "kind.ag"

# 361 "kind.ag"
DateTime imageDate(const string& image)
# 362 "kind.ag"
{
# 363 "kind.ag"
  FileName fn(image);
# 364 "kind.ag"
  Strings ss;
# 365 "kind.ag"
  split(fn.getName(), ss, '-');
# 366 "kind.ag"
  if (ss.size() < 5)
# 367 "kind.ag"
    throw Exception("imageDate", "image date not available");
# 368 "kind.ag"
  int Y = stoi(ss[1]);
# 369 "kind.ag"
  int M = stoi(ss[2]);
# 370 "kind.ag"
  int D = stoi(ss[3]);
# 371 "kind.ag"
  int h = stoi(ss[4]);
# 372 "kind.ag"
  int m = 0, s = 0;
# 373 "kind.ag"
  if (ss.size() > 5) // longImageName
# 374 "kind.ag"
    m = stoi(ss[5]);
# 375 "kind.ag"
  if (ss.size() > 6)
# 376 "kind.ag"
    s = stoi(ss[6]);
# 377 "kind.ag"
  return DateTime(Y, M, D, h, m, s);
# 378 "kind.ag"
}
# 379 "kind.ag"

# 380 "kind.ag"
void parseRule(string rule,
# 381 "kind.ag"
               set<int>& M, set<int>& D, set<int>& W, set<int>& h,
# 382 "kind.ag"
               time_t& exptime)
# 383 "kind.ag"
{
# 384 "kind.ag"
  for (unsigned int i = 0; i < rule.size(); ++i)
# 385 "kind.ag"
    rule[i] = tolower(rule[i]);
# 386 "kind.ag"

# 387 "kind.ag"
  substitute(rule, ' ', ',');
# 388 "kind.ag"
  reduceToOne(rule, ',');
# 389 "kind.ag"

# 390 "kind.ag"
  // rule = hour wday mday month <exptime>
# 391 "kind.ag"
  Lexer p(rule);
# 392 "kind.ag"

# 393 "kind.ag"
  h = getValues(p, 0, 23); // hour
# 394 "kind.ag"

# 395 "kind.ag"
  p.expect(',');
# 396 "kind.ag"

# 397 "kind.ag"
  W = getValues(p, 0, 7, 1); // wday
# 398 "kind.ag"

# 399 "kind.ag"
  p.expect(',');
# 400 "kind.ag"

# 401 "kind.ag"
  D = getValues(p, 1, 31); // day of month
# 402 "kind.ag"

# 403 "kind.ag"
  p.expect(',');
# 404 "kind.ag"

# 405 "kind.ag"
  M = getValues(p, 1, 12, 2); // month
# 406 "kind.ag"

# 407 "kind.ag"
#if 0
# 408 "kind.ag"
  // debug-output
# 409 "kind.ag"
  cout << "hour: ";
# 410 "kind.ag"
  for (int i : h)
# 411 "kind.ag"
    cout << i << " ";
# 412 "kind.ag"
  cout << endl;
# 413 "kind.ag"
  cout << "wday: ";
# 414 "kind.ag"
  for (int i : W)
# 415 "kind.ag"
    cout << i << " ";
# 416 "kind.ag"
  cout << endl;
# 417 "kind.ag"
  cout << "mday: ";
# 418 "kind.ag"
  for (int i : D)
# 419 "kind.ag"
    cout << i << " ";
# 420 "kind.ag"
  cout << endl;
# 421 "kind.ag"
  cout << "month: ";
# 422 "kind.ag"
  for (int i : M)
# 423 "kind.ag"
    cout << i << " ";
# 424 "kind.ag"
  cout << endl;
# 425 "kind.ag"
#endif
# 426 "kind.ag"

# 427 "kind.ag"
  string ts = p.getAll();
# 428 "kind.ag"
  substitute(ts, ',', ' ');
# 429 "kind.ag"
  exptime = stot(ts);
# 430 "kind.ag"
}
# 431 "kind.ag"

# 432 "kind.ag"
int removeDir(const string& path)
# 433 "kind.ag"
{
# 434 "kind.ag"
  debugPrint("removeDir " + path);
# 435 "kind.ag"

# 436 "kind.ag"
  DIR* d = opendir(path.c_str());
# 437 "kind.ag"

# 438 "kind.ag"
  int r = -1;
# 439 "kind.ag"
  if (d)
# 440 "kind.ag"
    {
# 441 "kind.ag"
      struct dirent* p;
# 442 "kind.ag"

# 443 "kind.ag"
      r = 0;
# 444 "kind.ag"

# 445 "kind.ag"
      while (!r && (p = readdir(d)))
# 446 "kind.ag"
        {
# 447 "kind.ag"
          int r2 = 0;
# 448 "kind.ag"

# 449 "kind.ag"
          string fn = p->d_name;
# 450 "kind.ag"

# 451 "kind.ag"
          if (fn != "." && fn != "..")
# 452 "kind.ag"
            {
# 453 "kind.ag"
              fn = path + "/" + fn;
# 454 "kind.ag"

# 455 "kind.ag"
              debugPrint("-- " + fn);
# 456 "kind.ag"

# 457 "kind.ag"
              struct stat statbuf;
# 458 "kind.ag"
              if (lstat(fn.c_str(), &statbuf) == 0)
# 459 "kind.ag"
                {
# 460 "kind.ag"
                  if (S_ISLNK(statbuf.st_mode))
# 461 "kind.ag"
                    {
# 462 "kind.ag"
                      debugPrint("Remove link " + fn);
# 463 "kind.ag"
                      r2 = unlink(fn.c_str());
# 464 "kind.ag"
                    }
# 465 "kind.ag"
                  else if (S_ISDIR(statbuf.st_mode))
# 466 "kind.ag"
                    {
# 467 "kind.ag"
                      debugPrint("Remove dir " + fn);
# 468 "kind.ag"
                      r2 = removeDir(fn);
# 469 "kind.ag"
                    }
# 470 "kind.ag"
                  else
# 471 "kind.ag"
                    {
# 472 "kind.ag"
                      debugPrint("Remove file " + fn);
# 473 "kind.ag"
                      r2 = unlink(fn.c_str());
# 474 "kind.ag"
                    }
# 475 "kind.ag"
                }
# 476 "kind.ag"
              else
# 477 "kind.ag"
                {
# 478 "kind.ag"
                  cout << "stat(" << fn << ") failed" << endl;
# 479 "kind.ag"
                  // we assume "file" here
# 480 "kind.ag"
                  r2 = unlink(fn.c_str());
# 481 "kind.ag"
                }
# 482 "kind.ag"
            }
# 483 "kind.ag"
          r = r2;
# 484 "kind.ag"
        }
# 485 "kind.ag"

# 486 "kind.ag"
      closedir(d);
# 487 "kind.ag"
    }
# 488 "kind.ag"

# 489 "kind.ag"
  if (r == 0)
# 490 "kind.ag"
    {
# 491 "kind.ag"
      debugPrint("Remove Dir itself " + path);
# 492 "kind.ag"

# 493 "kind.ag"
      r = rmdir(path.c_str());
# 494 "kind.ag"
    }
# 495 "kind.ag"

# 496 "kind.ag"
  return r;
# 497 "kind.ag"
}
# 498 "kind.ag"

# 499 "kind.ag"
#if 0
# 500 "kind.ag"
int removeDir(const string& dname)
# 501 "kind.ag"
{
# 502 "kind.ag"
  int rc = 0;
# 503 "kind.ag"
  if (!dryRun)
# 504 "kind.ag"
    {
# 505 "kind.ag"
      Strings files;
# 506 "kind.ag"
      // subdirectories
# 507 "kind.ag"
      dirList(dname, files);
# 508 "kind.ag"
      for (unsigned int i = 0; i < files.size(); ++i)
# 509 "kind.ag"
        {
# 510 "kind.ag"
          debugPrint("Remove dir " + files[i]);
# 511 "kind.ag"
          for (unsigned int i = 0; i < files.size(); ++i)
# 512 "kind.ag"
            rc += removeDir(files[i]);
# 513 "kind.ag"
        }
# 514 "kind.ag"
      files.clear();
# 515 "kind.ag"

# 516 "kind.ag"
      // files in directory
# 517 "kind.ag"
      fileList(dname, files);
# 518 "kind.ag"
      for (unsigned int i = 0; i < files.size(); ++i)
# 519 "kind.ag"
        {
# 520 "kind.ag"
          debugPrint("unlink " + files[i]);
# 521 "kind.ag"
          if (!dryRun)
# 522 "kind.ag"
            {
# 523 "kind.ag"
              if (unlink(files[i].c_str()) != 0)
# 524 "kind.ag"
                rc++;
# 525 "kind.ag"
            }
# 526 "kind.ag"
        }
# 527 "kind.ag"
      debugPrint("rmdir " + dname);
# 528 "kind.ag"

# 529 "kind.ag"
      // directory
# 530 "kind.ag"
      if (rmdir(dname.c_str()) != 0)
# 531 "kind.ag"
        rc++;
# 532 "kind.ag"
    }
# 533 "kind.ag"

# 534 "kind.ag"
  return rc;
# 535 "kind.ag"
}
# 536 "kind.ag"
#endif
# 537 "kind.ag"

# 538 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 539 "kind.ag"
{
# 540 "kind.ag"
  if (!quiet)
# 541 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 542 "kind.ag"

# 543 "kind.ag"
  string vaultpath = findVault(vault);
# 544 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 545 "kind.ag"

# 546 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 547 "kind.ag"
  readVaultConfig(vaultConfigName, conf);
# 548 "kind.ag"
  if (debug)
# 549 "kind.ag"
    {
# 550 "kind.ag"
      cout << "vault config:" << endl;
# 551 "kind.ag"
      conf.print();
# 552 "kind.ag"
    }
# 553 "kind.ag"

# 554 "kind.ag"
  Strings dirlist; // list of subdirectories
# 555 "kind.ag"
  dirList(vaultpath, dirlist);
# 556 "kind.ag"

# 557 "kind.ag"
  Strings validImages;
# 558 "kind.ag"
  Strings invalidImages;
# 559 "kind.ag"
  string imgname = getImageName(conf);
# 560 "kind.ag"

# 561 "kind.ag"
  for (unsigned int i = 0; i < dirlist.size(); ++i)
# 562 "kind.ag"
    {
# 563 "kind.ag"
      FileName fn(dirlist[i]);
# 564 "kind.ag"
      if (startsWith(fn.getName(), imgname)) // dir is image ?
# 565 "kind.ag"
        {
# 566 "kind.ag"
          debugPrint(dirlist[i]);
# 567 "kind.ag"

# 568 "kind.ag"
          DateTime t = imageDate(dirlist[i]);
# 569 "kind.ag"

# 570 "kind.ag"
          if (t != now) // ignore just created image
# 571 "kind.ag"
            {
# 572 "kind.ag"
              if (!isValidImage(dirlist[i])) // invalid image?
# 573 "kind.ag"
                {
# 574 "kind.ag"
                  invalidImages.push_back(dirlist[i]);
# 575 "kind.ag"
                  debugPrint("- invalid image");
# 576 "kind.ag"
                }
# 577 "kind.ag"
              else
# 578 "kind.ag"
                {
# 579 "kind.ag"
                  validImages.push_back(dirlist[i]);
# 580 "kind.ag"
                  debugPrint("- valid image");
# 581 "kind.ag"
                }
# 582 "kind.ag"
            }
# 583 "kind.ag"
          else
# 584 "kind.ag"
            debugPrint("- current image - ignored");
# 585 "kind.ag"
        }
# 586 "kind.ag"
    }
# 587 "kind.ag"

# 588 "kind.ag"
  for (unsigned int i = 0; i < invalidImages.size(); ++i)
# 589 "kind.ag"
    {
# 590 "kind.ag"
      try
# 591 "kind.ag"
        {
# 592 "kind.ag"
          DateTime t = imageDate(invalidImages[i]);
# 593 "kind.ag"
          DateTime expireTime = t + stot(conf.getString("expireFailedImage"));
# 594 "kind.ag"
          if (debug)
# 595 "kind.ag"
            {
# 596 "kind.ag"
              cout << "image: " << t.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 597 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 598 "kind.ag"
            }
# 599 "kind.ag"
          if (expireTime < now)
# 600 "kind.ag"
            {
# 601 "kind.ag"
              if (!quiet)
# 602 "kind.ag"
                cout << "  removing invalid image " << invalidImages[i] << endl;
# 603 "kind.ag"
              if (removeDir(invalidImages[i]) != 0)
# 604 "kind.ag"
                cout << "Error removing " <<  invalidImages[i] << endl;
# 605 "kind.ag"
            }
# 606 "kind.ag"
        }
# 607 "kind.ag"
      catch (Exception ex)
# 608 "kind.ag"
        {
# 609 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 610 "kind.ag"
        }
# 611 "kind.ag"
    }
# 612 "kind.ag"

# 613 "kind.ag"
  sort(validImages.begin(), validImages.end()); // lexicographical order == temporal order
# 614 "kind.ag"
  for (unsigned int i = 0;
# 615 "kind.ag"
       i < validImages.size() - 1; // never expire latest image
# 616 "kind.ag"
       ++i)
# 617 "kind.ag"
    {
# 618 "kind.ag"
      try
# 619 "kind.ag"
        {
# 620 "kind.ag"
          DateTime imageTime = imageDate(validImages[i]);
# 621 "kind.ag"
          DateTime expireTime = DateTime::now() + 100; // don't expire if  no rule found
# 622 "kind.ag"
          Strings expireRules = conf.getStrings("expireRule");
# 623 "kind.ag"
          int ruleNr = 0;
# 624 "kind.ag"
          for (unsigned int k = 0; k < expireRules.size(); ++k)
# 625 "kind.ag"
            {
# 626 "kind.ag"
              debugPrint("Checking rule " + expireRules[k]);
# 627 "kind.ag"

# 628 "kind.ag"
              set<int> M, D, W, h;
# 629 "kind.ag"
              set<int> Y, m, s;
# 630 "kind.ag"
              time_t expirePeriod;
# 631 "kind.ag"
              parseRule(expireRules[k], M, D, W, h, expirePeriod);
# 632 "kind.ag"
              //    cout << M << " " << D << " " << W << " " << h << " " << expirePeriod << endl;
# 633 "kind.ag"

# 634 "kind.ag"
              if (imageTime.match(Y, M, D, W, h, m, s))
# 635 "kind.ag"
                {
# 636 "kind.ag"
                  debugPrint("match");
# 637 "kind.ag"
                  expireTime = imageTime + expirePeriod;
# 638 "kind.ag"
                  ruleNr = k;
# 639 "kind.ag"
                }
# 640 "kind.ag"
            }
# 641 "kind.ag"
          if (debug)
# 642 "kind.ag"
            {
# 643 "kind.ag"
              cout << "image: " << imageTime.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 644 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 645 "kind.ag"
            }
# 646 "kind.ag"
          if (now > expireTime)
# 647 "kind.ag"
            {
# 648 "kind.ag"
              if (!quiet)
# 649 "kind.ag"
                cout << "removing " << validImages[i] << " rule=" << expireRules[ruleNr] << endl;
# 650 "kind.ag"
              removeDir(validImages[i]);
# 651 "kind.ag"
            }
# 652 "kind.ag"
        }
# 653 "kind.ag"
      catch (Exception ex)
# 654 "kind.ag"
        {
# 655 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 656 "kind.ag"
        }
# 657 "kind.ag"
    }
# 658 "kind.ag"
}
# 659 "kind.ag"

# 660 "kind.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - archiving backup" << endl;
  cout << "Usage:" << endl;
  cout << ag_programName << " [<options>] vault_or_group " << endl;
  cout << "  vault_or_group - Vault to backup" << endl;
  cout << "Options:" << endl;
  cout << "  -f --full          Force full image == initial backup (default: false)" << endl;
  cout << "  -c --masterconfig  Master config file (default: \"\")" << endl;
  cout << "                     if not given or empty kind looks for" << endl;
  cout << "                     /etc/kind/master.conf" << endl;
  cout << "                     /ffp/etc/kind/master.conf" << endl;
  cout << "  -B --backuponly    Only backup/no expire (default: false)" << endl;
  cout << "  -E --expireonly    Only expire/no backup (default: false)" << endl;
  cout << "  -D --dryrun        Dry run (no real backup) (default: false)" << endl;
  cout << "  -v --verbose       Verbose (default: false)" << endl;
  cout << "  -d --debug         Debug output of many data (default: false)" << endl;
  cout << "  -q --quiet         Be quiet - no messages (default: false)" << endl;
  cout << "  -h --help          This help" << endl;
  exit(1);
}

void error(const string &msg)
{
  cout << endl << ag_programName << " - error: " << msg << endl << endl;
  usage();
}

int ptoi(const char *para)
{
  char *end;
  int res = strtol(para, &end, 10);
  if (end == para)
    error(string("no int: ") + para);
  if (*end != 0)
    error(string("garbage in int: ") + para);
  return res;
}

double ptod(const char *para)
{
  char *end;
  double res = strtod(para, &end);
  if (end == para)
    error(string("no double: ") + para);
  if (*end != 0)
    error(string("garbage in double: ") + para);
  return res;
}

int main(int argc, char **argv)
{
bool fullImage = false;
string masterConfig = "";
bool backupOnly = false;
bool expireOnly = false;

string vault = "";
  static struct option ag_long_options[] =
  {
    {"full", no_argument, 0, 'f' },
    {"masterconfig", required_argument, 0, 'c' },
    {"backuponly", no_argument, 0, 'B' },
    {"expireonly", no_argument, 0, 'E' },
    {"dryrun", no_argument, 0, 'D' },
    {"verbose", no_argument, 0, 'v' },
    {"debug", no_argument, 0, 'd' },
    {"quiet", no_argument, 0, 'q' },
    {"help", no_argument, 0, 'h' },
    {0,         0,                 0,  0 }
  };
  ag_programName = argv[0];
  int rc;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, ":fc:BEDvdqh", ag_long_options, NULL)) >= 0)
    {
      switch (rc)
        {
        case '?':
          error("Unknown option");
          break;
        case ':':
          error("Expecting option parameter");
          break;
        case 'f':
              fullImage = true;
              break;

        case 'c':
              masterConfig = optarg;
              break;

        case 'B':
              backupOnly = true;
              break;

        case 'E':
              expireOnly = true;
              break;

        case 'D':
              dryRun = true;
              break;

        case 'v':
              verbose = true;
              break;

        case 'd':
              debug = true;
              break;

        case 'q':
              quiet = true;
              break;

        case 'h':
              usage();
              break;

        default:
          error("error in options");
        }
    }
  if (optind < argc)
    vault = argv[optind++];
  else error("Parameter vault_or_group needed");

/*AppGen:MainEnd*/
# 664 "kind.ag"

# 665 "kind.ag"
  int exitCode = 0;
# 666 "kind.ag"
  string lockFile;
# 667 "kind.ag"
  try
# 668 "kind.ag"
    {
# 669 "kind.ag"
      if (debug)
# 670 "kind.ag"
        verbose = true;
# 671 "kind.ag"

# 672 "kind.ag"
      KindConfig conf;
# 673 "kind.ag"

# 674 "kind.ag"
      // default-values
# 675 "kind.ag"
      conf.add("imageName", "image");
# 676 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 677 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 678 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 679 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 680 "kind.ag"
      conf.add("remoteShell", "");
# 681 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 682 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 683 "kind.ag"
      conf.add("userExcludeCommand",
# 684 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 685 "kind.ag"

# 686 "kind.ag"
      conf.add("logSize", "");
# 687 "kind.ag"

# 688 "kind.ag"
      if (!masterConfig.empty())
# 689 "kind.ag"
        readMasterConfig(masterConfig, conf);
# 690 "kind.ag"
      else if (fileExists("/etc/kind/master.conf"))
# 691 "kind.ag"
        readMasterConfig("etc/kind/master.conf", conf);
# 692 "kind.ag"
      else if (fileExists("/ffp/etc/kind/master.conf"))
# 693 "kind.ag"
        readMasterConfig("/ffp/etc/kind/master.conf", conf);
# 694 "kind.ag"
      else
# 695 "kind.ag"
        throw Exception("MasterConfig", "no file");
# 696 "kind.ag"

# 697 "kind.ag"
      if (debug)
# 698 "kind.ag"
        {
# 699 "kind.ag"
          cout << "global config:" << endl;
# 700 "kind.ag"
          conf.print();
# 701 "kind.ag"
        }
# 702 "kind.ag"

# 703 "kind.ag"
      lockFile = conf.getString("lockfile");
# 704 "kind.ag"
      createLock(lockFile);
# 705 "kind.ag"

# 706 "kind.ag"
      DateTime imageTime = DateTime::now();
# 707 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 708 "kind.ag"
      if (!logSizeFile.empty() && fileExists(logSizeFile))
# 709 "kind.ag"
        {
# 710 "kind.ag"
          vector<string> ss;
# 711 "kind.ag"
          file2Strings(logSizeFile, ss);
# 712 "kind.ag"
          for (auto s : ss)
# 713 "kind.ag"
            {
# 714 "kind.ag"
              unsigned int i = 0;
# 715 "kind.ag"
              string v = getWord(s, i);
# 716 "kind.ag"
              long int s1 = getLongInt(s, i);
# 717 "kind.ag"
              long int s2 = getLongInt(s, i);
# 718 "kind.ag"
              sizes[v] = Sizes(s1, s2);
# 719 "kind.ag"
            }
# 720 "kind.ag"
        }
# 721 "kind.ag"

# 722 "kind.ag"
      vector<string> vaults;
# 723 "kind.ag"
      string groupname = "group_" + vault;
# 724 "kind.ag"
      if (conf.hasKey(groupname))
# 725 "kind.ag"
        vaults = conf.getStrings(groupname);
# 726 "kind.ag"
      else
# 727 "kind.ag"
        vaults.push_back(vault);
# 728 "kind.ag"

# 729 "kind.ag"
      if (!expireOnly)
# 730 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 731 "kind.ag"
          {
# 732 "kind.ag"
            backupVault(vaults[i], conf, imageTime, fullImage);
# 733 "kind.ag"
            if (!logSizeFile.empty())
# 734 "kind.ag"
              {
# 735 "kind.ag"
                Strings st;
# 736 "kind.ag"
                for (auto s : sizes)
# 737 "kind.ag"
                  {
# 738 "kind.ag"
                    string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 739 "kind.ag"
                    st.push_back(h);
# 740 "kind.ag"
                  }
# 741 "kind.ag"
                strings2File(st, logSizeFile);
# 742 "kind.ag"
              }
# 743 "kind.ag"
          }
# 744 "kind.ag"

# 745 "kind.ag"
      if (!backupOnly)
# 746 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 747 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 748 "kind.ag"

# 749 "kind.ag"
      if (!quiet)
# 750 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 751 "kind.ag"

# 752 "kind.ag"
    }
# 753 "kind.ag"
  catch (const Exception& ex)
# 754 "kind.ag"
    {
# 755 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 756 "kind.ag"
      exitCode = 1;
# 757 "kind.ag"
    }
# 758 "kind.ag"
  catch (const char* msg)
# 759 "kind.ag"
    {
# 760 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 761 "kind.ag"
      exitCode = 1;
# 762 "kind.ag"
    }
# 763 "kind.ag"
  catch (const string& msg)
# 764 "kind.ag"
    {
# 765 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 766 "kind.ag"
      exitCode = 1;
# 767 "kind.ag"
    }
# 768 "kind.ag"
  removeLock(lockFile);
# 769 "kind.ag"
  return exitCode;
# 770 "kind.ag"
}
