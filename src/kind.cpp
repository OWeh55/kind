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
  opt: C, listconfig, void, listConfig, Show configuration, false
# 44 "kind.ag"
  opt: v, verbose, Void, verbose,  Verbose,  false
# 45 "kind.ag"
  opt: d, debug, Void, debug, Debug output of many data, false
# 46 "kind.ag"
  opt: q, quiet, Void, quiet, Be quiet - no messages, false
# 47 "kind.ag"
  opt: h, help, usage, ignored , This help
# 48 "kind.ag"
AppGen*/
# 49 "kind.ag"

# 50 "kind.ag"
using namespace std;
# 51 "kind.ag"

# 52 "kind.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <string>
bool dryRun = false;
bool verbose = false;
bool debug = false;
bool quiet = false;
/*AppGen:GlobalEnd*/
# 53 "kind.ag"

# 54 "kind.ag"
Strings banks;
# 55 "kind.ag"

# 56 "kind.ag"
typedef pair<long int, long int> Sizes;
# 57 "kind.ag"
map<string, Sizes> sizes;
# 58 "kind.ag"

# 59 "kind.ag"
void verbosePrint(const string& text)
# 60 "kind.ag"
{
# 61 "kind.ag"
  if (verbose)
# 62 "kind.ag"
    cout << "  " << text << endl;
# 63 "kind.ag"
}
# 64 "kind.ag"

# 65 "kind.ag"
void debugPrint(const string& text)
# 66 "kind.ag"
{
# 67 "kind.ag"
  if (verbose)
# 68 "kind.ag"
    cout << "    " << text << endl;
# 69 "kind.ag"
}
# 70 "kind.ag"

# 71 "kind.ag"
void readMasterConfig1(const string& fn, KindConfig& conf)
# 72 "kind.ag"
{
# 73 "kind.ag"
  verbosePrint("reading master config " + fn);
# 74 "kind.ag"
  conf.addFile(fn);
# 75 "kind.ag"
}
# 76 "kind.ag"

# 77 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 78 "kind.ag"
{
# 79 "kind.ag"
  if (!fn.empty())  // master config given by user on commandline
# 80 "kind.ag"
    readMasterConfig1(fn, conf);
# 81 "kind.ag"
  else if (fileExists("/etc/kind/master.conf"))
# 82 "kind.ag"
    readMasterConfig1("etc/kind/master.conf", conf);
# 83 "kind.ag"
  else if (fileExists("/ffp/etc/kind/master.conf"))
# 84 "kind.ag"
    readMasterConfig1("/ffp/etc/kind/master.conf", conf);
# 85 "kind.ag"
  else
# 86 "kind.ag"
    throw Exception("MasterConfig", "no file");
# 87 "kind.ag"
}
# 88 "kind.ag"

# 89 "kind.ag"
string findVault(const string& v)
# 90 "kind.ag"
{
# 91 "kind.ag"
  bool found = false;
# 92 "kind.ag"
  FileName fn;
# 93 "kind.ag"
  fn.setName(v);
# 94 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 95 "kind.ag"
    {
# 96 "kind.ag"
      fn.setPath(banks[i]);
# 97 "kind.ag"
      if (dirExists(fn.getFileName()))
# 98 "kind.ag"
        found = true;
# 99 "kind.ag"
    }
# 100 "kind.ag"
  if (!found)
# 101 "kind.ag"
    throw Exception("find vault", v + " not found");
# 102 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 103 "kind.ag"
  return fn.getFileName();
# 104 "kind.ag"
}
# 105 "kind.ag"

# 106 "kind.ag"
void readVaultConfig(const string& vault, KindConfig& conf)
# 107 "kind.ag"
{
# 108 "kind.ag"
  string vaultpath = findVault(vault);
# 109 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 110 "kind.ag"
  verbosePrint("reading vault config:");
# 111 "kind.ag"
  verbosePrint("  " + vaultConfigName);
# 112 "kind.ag"
  conf.addFile(vaultConfigName);
# 113 "kind.ag"
}
# 114 "kind.ag"

# 115 "kind.ag"
string getImageName(const KindConfig& conf)
# 116 "kind.ag"
{
# 117 "kind.ag"
  bool nonPortable = false;
# 118 "kind.ag"
  string res = conf.getString("imageName");
# 119 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < res.size(); ++i)
# 120 "kind.ag"
    {
# 121 "kind.ag"
      char c = res[i];
# 122 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 123 "kind.ag"
        nonPortable = true;
# 124 "kind.ag"
    }
# 125 "kind.ag"
  if (nonPortable)
# 126 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + res);
# 127 "kind.ag"
  return res;
# 128 "kind.ag"
}
# 129 "kind.ag"

# 130 "kind.ag"
bool isValidImage(const string& imageName)
# 131 "kind.ag"
{
# 132 "kind.ag"
  return dirExists(imageName) &&
# 133 "kind.ag"
         !fileExists(imageName + "/error") &&
# 134 "kind.ag"
         dirExists(imageName + "/tree");
# 135 "kind.ag"
}
# 136 "kind.ag"

# 137 "kind.ag"
Strings findValidImages(const string& vaultpath, const KindConfig& conf)
# 138 "kind.ag"
{
# 139 "kind.ag"
  Strings imageList;
# 140 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 141 "kind.ag"
  dirList(vaultpath, imageList);
# 142 "kind.ag"

# 143 "kind.ag"
  Strings validImageList;
# 144 "kind.ag"
  for (unsigned int i = 0; i < imageList.size(); ++i)
# 145 "kind.ag"
    {
# 146 "kind.ag"
      FileName fn(imageList[i]);
# 147 "kind.ag"
      string imgname = getImageName(conf);
# 148 "kind.ag"
      int len = imgname.length();
# 149 "kind.ag"
      if (fn.getName().substr(0, len) == imgname)
# 150 "kind.ag"
        {
# 151 "kind.ag"
          debugPrint("Checking " + imageList[i]);
# 152 "kind.ag"
          if (isValidImage(imageList[i]))
# 153 "kind.ag"
            validImageList.push_back(imageList[i]);
# 154 "kind.ag"
        }
# 155 "kind.ag"
    }
# 156 "kind.ag"
  if (validImageList.empty())
# 157 "kind.ag"
    throw Exception("Find reference", "No reference found");
# 158 "kind.ag"
  sort(validImageList.begin(), validImageList.end());
# 159 "kind.ag"
  return validImageList;
# 160 "kind.ag"
}
# 161 "kind.ag"

# 162 "kind.ag"
void backupVault(const string& vault,
# 163 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 164 "kind.ag"
                 const DateTime& imageTime,
# 165 "kind.ag"
                 bool fullImage)
# 166 "kind.ag"
{
# 167 "kind.ag"
  if (!quiet)
# 168 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 169 "kind.ag"
  try
# 170 "kind.ag"
    {
# 171 "kind.ag"
      sizes[vault].second = 0; // nothing backed up yet
# 172 "kind.ag"

# 173 "kind.ag"
      readVaultConfig(vault, conf);
# 174 "kind.ag"

# 175 "kind.ag"
      string vaultpath = findVault(vault);
# 176 "kind.ag"

# 177 "kind.ag"
      string imageName = getImageName(conf);
# 178 "kind.ag"
      if (!imageName.empty())
# 179 "kind.ag"
        imageName += '-';
# 180 "kind.ag"
      string imageFullName =  vaultpath + "/" + imageName ;
# 181 "kind.ag"

# 182 "kind.ag"
      if (conf.getBool("longImageName"))
# 183 "kind.ag"
        imageFullName += imageTime.getString('m');
# 184 "kind.ag"
      else
# 185 "kind.ag"
        imageFullName += imageTime.getString('s');
# 186 "kind.ag"

# 187 "kind.ag"
      verbosePrint("backup to \"" + imageFullName + "\"");
# 188 "kind.ag"

# 189 "kind.ag"
      // find reference image
# 190 "kind.ag"
      string referenceImage;
# 191 "kind.ag"
      if (!fullImage)
# 192 "kind.ag"
        {
# 193 "kind.ag"
          Strings validImageList = findValidImages(vaultpath, conf);
# 194 "kind.ag"
          // last image is newest image
# 195 "kind.ag"
          referenceImage = validImageList.back();
# 196 "kind.ag"
        }
# 197 "kind.ag"

# 198 "kind.ag"
      // create image path
# 199 "kind.ag"
      if (!dryRun)
# 200 "kind.ag"
        if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 201 "kind.ag"
          throw Exception("Create image", "failed to create " + imageFullName);
# 202 "kind.ag"

# 203 "kind.ag"
      // error message
# 204 "kind.ag"
      // we write an generic error message to mark backup as unsuccessful
# 205 "kind.ag"
      // will be deleted at successful end of rsync
# 206 "kind.ag"
      string errorfile = imageFullName + "/error";
# 207 "kind.ag"
      if (!dryRun)
# 208 "kind.ag"
        {
# 209 "kind.ag"
          ofstream error(errorfile);
# 210 "kind.ag"
          error << "failed" << endl;
# 211 "kind.ag"
          error.close();
# 212 "kind.ag"
        }
# 213 "kind.ag"

# 214 "kind.ag"
      string host;
# 215 "kind.ag"
      if (conf.hasKey("host"))
# 216 "kind.ag"
        host = conf.getString("host");
# 217 "kind.ag"

# 218 "kind.ag"
      string server;
# 219 "kind.ag"
      if (conf.hasKey("server"))
# 220 "kind.ag"
        server = conf.getString("server");
# 221 "kind.ag"

# 222 "kind.ag"
      if (!host.empty() && !server.empty())
# 223 "kind.ag"
        throw Exception("backupVault", "Cannot have host and server");
# 224 "kind.ag"

# 225 "kind.ag"
      string path = conf.getString("path");
# 226 "kind.ag"
      if (path.empty())
# 227 "kind.ag"
        throw Exception("rsync", "empty source path");
# 228 "kind.ag"
      if (path.back() != '/')
# 229 "kind.ag"
        path += '/';
# 230 "kind.ag"

# 231 "kind.ag"
      string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 232 "kind.ag"
      if (!conf.getBool("ignorePermission"))
# 233 "kind.ag"
        rsyncCmd += "-pgo";
# 234 "kind.ag"
      vector<string> rso = conf.getStrings("rsyncOption");
# 235 "kind.ag"
      for (string opt : rso)
# 236 "kind.ag"
        rsyncCmd += opt + " ";
# 237 "kind.ag"

# 238 "kind.ag"
      if (!host.empty())  // shell mode
# 239 "kind.ag"
        {
# 240 "kind.ag"
          // cout << "USING SHELLMODE '" << host << "'" << endl;
# 241 "kind.ag"
          string remoteShell = conf.getString("remoteShell");
# 242 "kind.ag"
          string userAtHost = conf.getString("user") + "@" +
# 243 "kind.ag"
                              conf.getString("host");
# 244 "kind.ag"
          string rshCommand = remoteShell;
# 245 "kind.ag"
          if (remoteShell.empty())
# 246 "kind.ag"
            rshCommand = "ssh";
# 247 "kind.ag"

# 248 "kind.ag"
          rshCommand += " " + userAtHost;
# 249 "kind.ag"

# 250 "kind.ag"
          // excludes
# 251 "kind.ag"
          Strings excluded;
# 252 "kind.ag"

# 253 "kind.ag"
          string userExcludeCommand = conf.getString("userExcludeCommand");
# 254 "kind.ag"

# 255 "kind.ag"
          if (!userExcludeCommand.empty())
# 256 "kind.ag"
            {
# 257 "kind.ag"
              replacePlaceHolder(userExcludeCommand, "%path", path);
# 258 "kind.ag"
              string excludeCommand = rshCommand + " " + userExcludeCommand;
# 259 "kind.ag"

# 260 "kind.ag"
              verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 261 "kind.ag"

# 262 "kind.ag"
              int rc;
# 263 "kind.ag"
              excluded = myPopen(excludeCommand, rc, debug);
# 264 "kind.ag"
              if (rc > 0)
# 265 "kind.ag"
                throw Exception("Find exludes", "Search for excludes failed");
# 266 "kind.ag"

# 267 "kind.ag"
              for (unsigned int i = 0; i < excluded.size(); ++i)
# 268 "kind.ag"
                {
# 269 "kind.ag"
                  FileName fn(excluded[i]);
# 270 "kind.ag"
                  excluded[i] = '/' + fn.getPath();
# 271 "kind.ag"
                  debugPrint("Excluding: " + excluded[i]);
# 272 "kind.ag"
                }
# 273 "kind.ag"
            }
# 274 "kind.ag"

# 275 "kind.ag"
          string userExcludeFile = conf.getString("userExcludeFile");
# 276 "kind.ag"
          if (!userExcludeFile.empty())
# 277 "kind.ag"
            {
# 278 "kind.ag"
              userExcludeFile = path + userExcludeFile;
# 279 "kind.ag"
              string getExcludeFileCommand = rshCommand;
# 280 "kind.ag"
              getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 281 "kind.ag"
              getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 282 "kind.ag"
              // cout << getExcludeFileCommand << endl;
# 283 "kind.ag"
              int rc;
# 284 "kind.ag"
              Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 285 "kind.ag"
              if (rc == 0)
# 286 "kind.ag"
                excluded += excludes2;
# 287 "kind.ag"
            }
# 288 "kind.ag"

# 289 "kind.ag"
          if (conf.hasKey("exclude"))
# 290 "kind.ag"
            excluded += conf.getStrings("exclude");
# 291 "kind.ag"

# 292 "kind.ag"
          if (!dryRun)
# 293 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 294 "kind.ag"

# 295 "kind.ag"
          // rsync image
# 296 "kind.ag"

# 297 "kind.ag"
          if (!remoteShell.empty())
# 298 "kind.ag"
            rsyncCmd += " -e \'" + remoteShell + "\' ";
# 299 "kind.ag"

# 300 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 301 "kind.ag"
          if (!referenceImage.empty())
# 302 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 303 "kind.ag"
          rsyncCmd += userAtHost + ":" + path + " ";
# 304 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 305 "kind.ag"
        } // shell mode
# 306 "kind.ag"
      else
# 307 "kind.ag"
        {
# 308 "kind.ag"
          // cout << "USING SERVERMODE" << endl;
# 309 "kind.ag"
          vector<string> excluded;
# 310 "kind.ag"
          if (conf.hasKey("exclude"))
# 311 "kind.ag"
            {
# 312 "kind.ag"
              Strings excludes = conf.getStrings("exclude");
# 313 "kind.ag"
              for (string s : excludes)
# 314 "kind.ag"
                excluded.push_back(s);
# 315 "kind.ag"
            }
# 316 "kind.ag"

# 317 "kind.ag"
          if (!dryRun)
# 318 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 319 "kind.ag"

# 320 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 321 "kind.ag"
          if (!referenceImage.empty())
# 322 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 323 "kind.ag"
          rsyncCmd += conf.getString("server") + "::" + path + " ";
# 324 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 325 "kind.ag"
        }
# 326 "kind.ag"

# 327 "kind.ag"
      debugPrint("Action: " + rsyncCmd);
# 328 "kind.ag"

# 329 "kind.ag"
      vector<string> backupResult;
# 330 "kind.ag"
      if (!dryRun)
# 331 "kind.ag"
        {
# 332 "kind.ag"
          verbosePrint("syncing (" + rsyncCmd + ")");
# 333 "kind.ag"
          int rc;
# 334 "kind.ag"
          backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 335 "kind.ag"
          // strings2File(backupResult, imageFullName + "/rsync-log");
# 336 "kind.ag"
          if (rc == 0 || rc == 24) // "no error" or "vanished source files" (ignored)
# 337 "kind.ag"
            {
# 338 "kind.ag"
              unlink(errorfile.c_str());
# 339 "kind.ag"
              string lastLink = vaultpath + "/last";
# 340 "kind.ag"
              unlink(lastLink.c_str());
# 341 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 342 "kind.ag"
              long int st = 0;
# 343 "kind.ag"
              long int sc = 0;
# 344 "kind.ag"
              for (auto bl : backupResult)
# 345 "kind.ag"
                {
# 346 "kind.ag"
                  if (bl.substr(0, 15) == "Total file size")
# 347 "kind.ag"
                    st = getNumber(bl);
# 348 "kind.ag"
                  else if (bl.substr(0, 27) == "Total transferred file size")
# 349 "kind.ag"
                    sc = getNumber(bl);
# 350 "kind.ag"
                }
# 351 "kind.ag"
              // sizes[vault] = pair<long int, long int>(st, sc);
# 352 "kind.ag"
              sizes[vault] = Sizes(st, sc);
# 353 "kind.ag"
              //  cout << vault << " " << st << " || " << sc << endl;
# 354 "kind.ag"
            }
# 355 "kind.ag"
          else
# 356 "kind.ag"
            throw Exception("Backup", "Failed to execute rsync");
# 357 "kind.ag"
        }
# 358 "kind.ag"
      else
# 359 "kind.ag"
        cout << "Not executing " << rsyncCmd << endl;
# 360 "kind.ag"
    }
# 361 "kind.ag"
  catch (Exception ex)
# 362 "kind.ag"
    {
# 363 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 364 "kind.ag"
    }
# 365 "kind.ag"
}
# 366 "kind.ag"

# 367 "kind.ag"
DateTime imageDate(const string& image)
# 368 "kind.ag"
{
# 369 "kind.ag"
  FileName fn(image);
# 370 "kind.ag"
  Strings ss;
# 371 "kind.ag"
  split(fn.getName(), ss, '-');
# 372 "kind.ag"
  if (ss.size() < 5)
# 373 "kind.ag"
    throw Exception("imageDate", "image date not available");
# 374 "kind.ag"
  int Y = stoi(ss[1]);
# 375 "kind.ag"
  int M = stoi(ss[2]);
# 376 "kind.ag"
  int D = stoi(ss[3]);
# 377 "kind.ag"
  int h = stoi(ss[4]);
# 378 "kind.ag"
  int m = 0, s = 0;
# 379 "kind.ag"
  if (ss.size() > 5) // longImageName
# 380 "kind.ag"
    m = stoi(ss[5]);
# 381 "kind.ag"
  if (ss.size() > 6)
# 382 "kind.ag"
    s = stoi(ss[6]);
# 383 "kind.ag"
  return DateTime(Y, M, D, h, m, s);
# 384 "kind.ag"
}
# 385 "kind.ag"

# 386 "kind.ag"
void parseRule(string rule,
# 387 "kind.ag"
               set<int>& M, set<int>& D, set<int>& W, set<int>& h,
# 388 "kind.ag"
               time_t& exptime)
# 389 "kind.ag"
{
# 390 "kind.ag"
  for (unsigned int i = 0; i < rule.size(); ++i)
# 391 "kind.ag"
    rule[i] = tolower(rule[i]);
# 392 "kind.ag"

# 393 "kind.ag"
  substitute(rule, ' ', ',');
# 394 "kind.ag"
  reduceToOne(rule, ',');
# 395 "kind.ag"

# 396 "kind.ag"
  // rule = hour wday mday month <exptime>
# 397 "kind.ag"
  Lexer p(rule);
# 398 "kind.ag"

# 399 "kind.ag"
  h = getValues(p, 0, 23); // hour
# 400 "kind.ag"

# 401 "kind.ag"
  p.expect(',');
# 402 "kind.ag"

# 403 "kind.ag"
  W = getValues(p, 0, 7, 1); // wday
# 404 "kind.ag"

# 405 "kind.ag"
  p.expect(',');
# 406 "kind.ag"

# 407 "kind.ag"
  D = getValues(p, 1, 31); // day of month
# 408 "kind.ag"

# 409 "kind.ag"
  p.expect(',');
# 410 "kind.ag"

# 411 "kind.ag"
  M = getValues(p, 1, 12, 2); // month
# 412 "kind.ag"

# 413 "kind.ag"
#if 0
# 414 "kind.ag"
  // debug-output
# 415 "kind.ag"
  cout << "hour: ";
# 416 "kind.ag"
  for (int i : h)
# 417 "kind.ag"
    cout << i << " ";
# 418 "kind.ag"
  cout << endl;
# 419 "kind.ag"
  cout << "wday: ";
# 420 "kind.ag"
  for (int i : W)
# 421 "kind.ag"
    cout << i << " ";
# 422 "kind.ag"
  cout << endl;
# 423 "kind.ag"
  cout << "mday: ";
# 424 "kind.ag"
  for (int i : D)
# 425 "kind.ag"
    cout << i << " ";
# 426 "kind.ag"
  cout << endl;
# 427 "kind.ag"
  cout << "month: ";
# 428 "kind.ag"
  for (int i : M)
# 429 "kind.ag"
    cout << i << " ";
# 430 "kind.ag"
  cout << endl;
# 431 "kind.ag"
#endif
# 432 "kind.ag"

# 433 "kind.ag"
  string ts = p.getAll();
# 434 "kind.ag"
  substitute(ts, ',', ' ');
# 435 "kind.ag"
  exptime = stot(ts);
# 436 "kind.ag"
}
# 437 "kind.ag"

# 438 "kind.ag"
int removeDir(const string& path)
# 439 "kind.ag"
{
# 440 "kind.ag"
  debugPrint("removeDir " + path);
# 441 "kind.ag"

# 442 "kind.ag"
  DIR* d = opendir(path.c_str());
# 443 "kind.ag"

# 444 "kind.ag"
  int r = -1;
# 445 "kind.ag"
  if (d)
# 446 "kind.ag"
    {
# 447 "kind.ag"
      struct dirent* p;
# 448 "kind.ag"

# 449 "kind.ag"
      r = 0;
# 450 "kind.ag"

# 451 "kind.ag"
      while (!r && (p = readdir(d)))
# 452 "kind.ag"
        {
# 453 "kind.ag"
          int r2 = 0;
# 454 "kind.ag"

# 455 "kind.ag"
          string fn = p->d_name;
# 456 "kind.ag"

# 457 "kind.ag"
          if (fn != "." && fn != "..")
# 458 "kind.ag"
            {
# 459 "kind.ag"
              fn = path + "/" + fn;
# 460 "kind.ag"

# 461 "kind.ag"
              debugPrint("-- " + fn);
# 462 "kind.ag"

# 463 "kind.ag"
              struct stat statbuf;
# 464 "kind.ag"
              if (lstat(fn.c_str(), &statbuf) == 0)
# 465 "kind.ag"
                {
# 466 "kind.ag"
                  if (S_ISLNK(statbuf.st_mode))
# 467 "kind.ag"
                    {
# 468 "kind.ag"
                      debugPrint("Remove link " + fn);
# 469 "kind.ag"
                      r2 = unlink(fn.c_str());
# 470 "kind.ag"
                    }
# 471 "kind.ag"
                  else if (S_ISDIR(statbuf.st_mode))
# 472 "kind.ag"
                    {
# 473 "kind.ag"
                      debugPrint("Remove dir " + fn);
# 474 "kind.ag"
                      r2 = removeDir(fn);
# 475 "kind.ag"
                    }
# 476 "kind.ag"
                  else
# 477 "kind.ag"
                    {
# 478 "kind.ag"
                      debugPrint("Remove file " + fn);
# 479 "kind.ag"
                      r2 = unlink(fn.c_str());
# 480 "kind.ag"
                    }
# 481 "kind.ag"
                }
# 482 "kind.ag"
              else
# 483 "kind.ag"
                {
# 484 "kind.ag"
                  cout << "stat(" << fn << ") failed" << endl;
# 485 "kind.ag"
                  // we assume "file" here
# 486 "kind.ag"
                  r2 = unlink(fn.c_str());
# 487 "kind.ag"
                }
# 488 "kind.ag"
            }
# 489 "kind.ag"
          r = r2;
# 490 "kind.ag"
        }
# 491 "kind.ag"

# 492 "kind.ag"
      closedir(d);
# 493 "kind.ag"
    }
# 494 "kind.ag"

# 495 "kind.ag"
  if (r == 0)
# 496 "kind.ag"
    {
# 497 "kind.ag"
      debugPrint("Remove Dir itself " + path);
# 498 "kind.ag"

# 499 "kind.ag"
      r = rmdir(path.c_str());
# 500 "kind.ag"
    }
# 501 "kind.ag"

# 502 "kind.ag"
  return r;
# 503 "kind.ag"
}
# 504 "kind.ag"

# 505 "kind.ag"
#if 0
# 506 "kind.ag"
int removeDir(const string& dname)
# 507 "kind.ag"
{
# 508 "kind.ag"
  int rc = 0;
# 509 "kind.ag"
  if (!dryRun)
# 510 "kind.ag"
    {
# 511 "kind.ag"
      Strings files;
# 512 "kind.ag"
      // subdirectories
# 513 "kind.ag"
      dirList(dname, files);
# 514 "kind.ag"
      for (unsigned int i = 0; i < files.size(); ++i)
# 515 "kind.ag"
        {
# 516 "kind.ag"
          debugPrint("Remove dir " + files[i]);
# 517 "kind.ag"
          for (unsigned int i = 0; i < files.size(); ++i)
# 518 "kind.ag"
            rc += removeDir(files[i]);
# 519 "kind.ag"
        }
# 520 "kind.ag"
      files.clear();
# 521 "kind.ag"

# 522 "kind.ag"
      // files in directory
# 523 "kind.ag"
      fileList(dname, files);
# 524 "kind.ag"
      for (unsigned int i = 0; i < files.size(); ++i)
# 525 "kind.ag"
        {
# 526 "kind.ag"
          debugPrint("unlink " + files[i]);
# 527 "kind.ag"
          if (!dryRun)
# 528 "kind.ag"
            {
# 529 "kind.ag"
              if (unlink(files[i].c_str()) != 0)
# 530 "kind.ag"
                rc++;
# 531 "kind.ag"
            }
# 532 "kind.ag"
        }
# 533 "kind.ag"
      debugPrint("rmdir " + dname);
# 534 "kind.ag"

# 535 "kind.ag"
      // directory
# 536 "kind.ag"
      if (rmdir(dname.c_str()) != 0)
# 537 "kind.ag"
        rc++;
# 538 "kind.ag"
    }
# 539 "kind.ag"

# 540 "kind.ag"
  return rc;
# 541 "kind.ag"
}
# 542 "kind.ag"
#endif
# 543 "kind.ag"

# 544 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 545 "kind.ag"
{
# 546 "kind.ag"
  if (!quiet)
# 547 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 548 "kind.ag"

# 549 "kind.ag"
  readVaultConfig(vault, conf);
# 550 "kind.ag"

# 551 "kind.ag"
  string vaultpath = findVault(vault);
# 552 "kind.ag"

# 553 "kind.ag"
  Strings dirlist; // list of subdirectories
# 554 "kind.ag"
  dirList(vaultpath, dirlist);
# 555 "kind.ag"

# 556 "kind.ag"
  Strings validImages;
# 557 "kind.ag"
  Strings invalidImages;
# 558 "kind.ag"
  string imgname = getImageName(conf);
# 559 "kind.ag"

# 560 "kind.ag"
  for (unsigned int i = 0; i < dirlist.size(); ++i)
# 561 "kind.ag"
    {
# 562 "kind.ag"
      FileName fn(dirlist[i]);
# 563 "kind.ag"
      if (startsWith(fn.getName(), imgname)) // dir is image ?
# 564 "kind.ag"
        {
# 565 "kind.ag"
          debugPrint(dirlist[i]);
# 566 "kind.ag"

# 567 "kind.ag"
          DateTime t = imageDate(dirlist[i]);
# 568 "kind.ag"

# 569 "kind.ag"
          if (t != now) // ignore just created image
# 570 "kind.ag"
            {
# 571 "kind.ag"
              if (!isValidImage(dirlist[i])) // invalid image?
# 572 "kind.ag"
                {
# 573 "kind.ag"
                  invalidImages.push_back(dirlist[i]);
# 574 "kind.ag"
                  debugPrint("- invalid image");
# 575 "kind.ag"
                }
# 576 "kind.ag"
              else
# 577 "kind.ag"
                {
# 578 "kind.ag"
                  validImages.push_back(dirlist[i]);
# 579 "kind.ag"
                  debugPrint("- valid image");
# 580 "kind.ag"
                }
# 581 "kind.ag"
            }
# 582 "kind.ag"
          else
# 583 "kind.ag"
            debugPrint("- current image - ignored");
# 584 "kind.ag"
        }
# 585 "kind.ag"
    }
# 586 "kind.ag"

# 587 "kind.ag"
  for (unsigned int i = 0; i < invalidImages.size(); ++i)
# 588 "kind.ag"
    {
# 589 "kind.ag"
      try
# 590 "kind.ag"
        {
# 591 "kind.ag"
          DateTime t = imageDate(invalidImages[i]);
# 592 "kind.ag"
          DateTime expireTime = t + stot(conf.getString("expireFailedImage"));
# 593 "kind.ag"
          if (debug)
# 594 "kind.ag"
            {
# 595 "kind.ag"
              cout << "image: " << t.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 596 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 597 "kind.ag"
            }
# 598 "kind.ag"
          if (expireTime < now)
# 599 "kind.ag"
            {
# 600 "kind.ag"
              if (!quiet)
# 601 "kind.ag"
                cout << "  removing invalid image " << invalidImages[i] << endl;
# 602 "kind.ag"
              if (removeDir(invalidImages[i]) != 0)
# 603 "kind.ag"
                cout << "Error removing " <<  invalidImages[i] << endl;
# 604 "kind.ag"
            }
# 605 "kind.ag"
        }
# 606 "kind.ag"
      catch (Exception ex)
# 607 "kind.ag"
        {
# 608 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 609 "kind.ag"
        }
# 610 "kind.ag"
    }
# 611 "kind.ag"

# 612 "kind.ag"
  sort(validImages.begin(), validImages.end()); // lexicographical order == temporal order
# 613 "kind.ag"
  for (unsigned int i = 0;
# 614 "kind.ag"
       i < validImages.size() - 1; // never expire latest image
# 615 "kind.ag"
       ++i)
# 616 "kind.ag"
    {
# 617 "kind.ag"
      try
# 618 "kind.ag"
        {
# 619 "kind.ag"
          DateTime imageTime = imageDate(validImages[i]);
# 620 "kind.ag"
          DateTime expireTime = DateTime::now() + 100; // don't expire if  no rule found
# 621 "kind.ag"
          Strings expireRules = conf.getStrings("expireRule");
# 622 "kind.ag"
          int ruleNr = 0;
# 623 "kind.ag"
          for (unsigned int k = 0; k < expireRules.size(); ++k)
# 624 "kind.ag"
            {
# 625 "kind.ag"
              debugPrint("Checking rule " + expireRules[k]);
# 626 "kind.ag"

# 627 "kind.ag"
              set<int> M, D, W, h;
# 628 "kind.ag"
              set<int> Y, m, s;
# 629 "kind.ag"
              time_t expirePeriod;
# 630 "kind.ag"
              parseRule(expireRules[k], M, D, W, h, expirePeriod);
# 631 "kind.ag"
              //    cout << M << " " << D << " " << W << " " << h << " " << expirePeriod << endl;
# 632 "kind.ag"

# 633 "kind.ag"
              if (imageTime.match(Y, M, D, W, h, m, s))
# 634 "kind.ag"
                {
# 635 "kind.ag"
                  debugPrint("match");
# 636 "kind.ag"
                  expireTime = imageTime + expirePeriod;
# 637 "kind.ag"
                  ruleNr = k;
# 638 "kind.ag"
                }
# 639 "kind.ag"
            }
# 640 "kind.ag"
          if (debug)
# 641 "kind.ag"
            {
# 642 "kind.ag"
              cout << "image: " << imageTime.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 643 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 644 "kind.ag"
            }
# 645 "kind.ag"
          if (now > expireTime)
# 646 "kind.ag"
            {
# 647 "kind.ag"
              if (!quiet)
# 648 "kind.ag"
                cout << "removing " << validImages[i] << " rule=" << expireRules[ruleNr] << endl;
# 649 "kind.ag"
              removeDir(validImages[i]);
# 650 "kind.ag"
            }
# 651 "kind.ag"
        }
# 652 "kind.ag"
      catch (Exception ex)
# 653 "kind.ag"
        {
# 654 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 655 "kind.ag"
        }
# 656 "kind.ag"
    }
# 657 "kind.ag"
}
# 658 "kind.ag"

# 659 "kind.ag"
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
  cout << "  -C --listconfig    Show configuration (default: false)" << endl;
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
bool listConfig = false;

string vault = "";
  static struct option ag_long_options[] =
  {
    {"full", no_argument, 0, 'f' },
    {"masterconfig", required_argument, 0, 'c' },
    {"backuponly", no_argument, 0, 'B' },
    {"expireonly", no_argument, 0, 'E' },
    {"dryrun", no_argument, 0, 'D' },
    {"listconfig", no_argument, 0, 'C' },
    {"verbose", no_argument, 0, 'v' },
    {"debug", no_argument, 0, 'd' },
    {"quiet", no_argument, 0, 'q' },
    {"help", no_argument, 0, 'h' },
    {0,         0,                 0,  0 }
  };
  ag_programName = argv[0];
  int rc;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, ":fc:BEDCvdqh", ag_long_options, NULL)) >= 0)
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

        case 'C':
              listConfig = true;
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
# 663 "kind.ag"

# 664 "kind.ag"
  int exitCode = 0;
# 665 "kind.ag"
  string lockFile;
# 666 "kind.ag"
  try
# 667 "kind.ag"
    {
# 668 "kind.ag"
      if (debug)
# 669 "kind.ag"
        verbose = true;
# 670 "kind.ag"

# 671 "kind.ag"
      KindConfig conf;
# 672 "kind.ag"

# 673 "kind.ag"
      // default-values
# 674 "kind.ag"
      conf.add("imageName", "image");
# 675 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 676 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 677 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 678 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 679 "kind.ag"
      conf.add("remoteShell", "");
# 680 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 681 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 682 "kind.ag"
      conf.add("userExcludeCommand",
# 683 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 684 "kind.ag"
      conf.add("logSize", "");
# 685 "kind.ag"

# 686 "kind.ag"
      if (listConfig)
# 687 "kind.ag"
        {
# 688 "kind.ag"
          cout << "builtin config" << endl;
# 689 "kind.ag"
          conf.print(".   ");
# 690 "kind.ag"
        }
# 691 "kind.ag"

# 692 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 693 "kind.ag"

# 694 "kind.ag"
      banks = conf.getStrings("bank");
# 695 "kind.ag"
      if (banks.empty())
# 696 "kind.ag"
        throw Exception("read master config", "no banks defined");
# 697 "kind.ag"

# 698 "kind.ag"
      if (listConfig)
# 699 "kind.ag"
        {
# 700 "kind.ag"
          cout << "global config:" << endl;
# 701 "kind.ag"
          conf.print(".   ");
# 702 "kind.ag"
          readVaultConfig(vault, conf);
# 703 "kind.ag"
          cout << "vault config:" << endl;
# 704 "kind.ag"
          conf.print(".   ");
# 705 "kind.ag"
          exit(0);
# 706 "kind.ag"
        }
# 707 "kind.ag"

# 708 "kind.ag"
      lockFile = conf.getString("lockfile");
# 709 "kind.ag"
      createLock(lockFile);
# 710 "kind.ag"

# 711 "kind.ag"
      DateTime imageTime = DateTime::now();
# 712 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 713 "kind.ag"
      if (!logSizeFile.empty() && fileExists(logSizeFile))
# 714 "kind.ag"
        {
# 715 "kind.ag"
          vector<string> ss;
# 716 "kind.ag"
          file2Strings(logSizeFile, ss);
# 717 "kind.ag"
          for (auto s : ss)
# 718 "kind.ag"
            {
# 719 "kind.ag"
              unsigned int i = 0;
# 720 "kind.ag"
              string v = getWord(s, i);
# 721 "kind.ag"
              long int s1 = getLongInt(s, i);
# 722 "kind.ag"
              long int s2 = getLongInt(s, i);
# 723 "kind.ag"
              sizes[v] = Sizes(s1, s2);
# 724 "kind.ag"
            }
# 725 "kind.ag"
        }
# 726 "kind.ag"

# 727 "kind.ag"
      vector<string> vaults;
# 728 "kind.ag"
      string groupname = "group_" + vault;
# 729 "kind.ag"
      if (conf.hasKey(groupname))
# 730 "kind.ag"
        vaults = conf.getStrings(groupname);
# 731 "kind.ag"
      else
# 732 "kind.ag"
        vaults.push_back(vault);
# 733 "kind.ag"

# 734 "kind.ag"
      if (!expireOnly)
# 735 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 736 "kind.ag"
          {
# 737 "kind.ag"
            backupVault(vaults[i], conf, imageTime, fullImage);
# 738 "kind.ag"
            if (!logSizeFile.empty())
# 739 "kind.ag"
              {
# 740 "kind.ag"
                Strings st;
# 741 "kind.ag"
                for (auto s : sizes)
# 742 "kind.ag"
                  {
# 743 "kind.ag"
                    string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 744 "kind.ag"
                    st.push_back(h);
# 745 "kind.ag"
                  }
# 746 "kind.ag"
                strings2File(st, logSizeFile);
# 747 "kind.ag"
              }
# 748 "kind.ag"
          }
# 749 "kind.ag"

# 750 "kind.ag"
      if (!backupOnly)
# 751 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 752 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 753 "kind.ag"

# 754 "kind.ag"
      if (!quiet)
# 755 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 756 "kind.ag"

# 757 "kind.ag"
    }
# 758 "kind.ag"
  catch (const Exception& ex)
# 759 "kind.ag"
    {
# 760 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 761 "kind.ag"
      exitCode = 1;
# 762 "kind.ag"
    }
# 763 "kind.ag"
  catch (const char* msg)
# 764 "kind.ag"
    {
# 765 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 766 "kind.ag"
      exitCode = 1;
# 767 "kind.ag"
    }
# 768 "kind.ag"
  catch (const string& msg)
# 769 "kind.ag"
    {
# 770 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 771 "kind.ag"
      exitCode = 1;
# 772 "kind.ag"
    }
# 773 "kind.ag"
  removeLock(lockFile);
# 774 "kind.ag"
  return exitCode;
# 775 "kind.ag"
}
