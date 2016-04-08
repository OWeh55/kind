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
    readMasterConfig1("/etc/kind/master.conf", conf);
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
          if (rc == 0 ||
# 337 "kind.ag"
          rc == 24 || // "no error" or "vanished source files" (ignored)
# 338 "kind.ag"
          rc == 6114) // workaround for wrong exit code ??!!
# 339 "kind.ag"
            {
# 340 "kind.ag"
              unlink(errorfile.c_str());
# 341 "kind.ag"
              string lastLink = vaultpath + "/last";
# 342 "kind.ag"
              unlink(lastLink.c_str());
# 343 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 344 "kind.ag"
              long int st = 0;
# 345 "kind.ag"
              long int sc = 0;
# 346 "kind.ag"
              for (auto bl : backupResult)
# 347 "kind.ag"
                {
# 348 "kind.ag"
                  if (bl.substr(0, 15) == "Total file size")
# 349 "kind.ag"
                    st = getNumber(bl);
# 350 "kind.ag"
                  else if (bl.substr(0, 27) == "Total transferred file size")
# 351 "kind.ag"
                    sc = getNumber(bl);
# 352 "kind.ag"
                }
# 353 "kind.ag"
              // sizes[vault] = pair<long int, long int>(st, sc);
# 354 "kind.ag"
              sizes[vault] = Sizes(st, sc);
# 355 "kind.ag"
              //  cout << vault << " " << st << " || " << sc << endl;
# 356 "kind.ag"
            }
# 357 "kind.ag"
          else
# 358 "kind.ag"
            throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 359 "kind.ag"
        }
# 360 "kind.ag"
      else
# 361 "kind.ag"
        cout << "Not executing " << rsyncCmd << endl;
# 362 "kind.ag"
    }
# 363 "kind.ag"
  catch (Exception ex)
# 364 "kind.ag"
    {
# 365 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 366 "kind.ag"
    }
# 367 "kind.ag"
}
# 368 "kind.ag"

# 369 "kind.ag"
DateTime stringToDate(const string& dateString)
# 370 "kind.ag"
{
# 371 "kind.ag"
  Strings ss;
# 372 "kind.ag"
  split(dateString, ss, '-');
# 373 "kind.ag"
  if (ss.size() < 5)
# 374 "kind.ag"
    throw Exception("stringToDate", "date format invalid");
# 375 "kind.ag"
  int Y = stoi(ss[1]);
# 376 "kind.ag"
  int M = stoi(ss[2]);
# 377 "kind.ag"
  int D = stoi(ss[3]);
# 378 "kind.ag"
  int h = stoi(ss[4]);
# 379 "kind.ag"
  int m = 0, s = 0;
# 380 "kind.ag"
  if (ss.size() > 5) // longImageName
# 381 "kind.ag"
    m = stoi(ss[5]);
# 382 "kind.ag"
  if (ss.size() > 6)
# 383 "kind.ag"
    s = stoi(ss[6]);
# 384 "kind.ag"
  return DateTime(Y, M, D, h, m, s);
# 385 "kind.ag"
}
# 386 "kind.ag"

# 387 "kind.ag"
DateTime imageDate(const string& image)
# 388 "kind.ag"
{
# 389 "kind.ag"
  FileName fn(image);
# 390 "kind.ag"
  return stringToDate(fn.getName());
# 391 "kind.ag"
}
# 392 "kind.ag"

# 393 "kind.ag"
void parseRule(string rule,
# 394 "kind.ag"
               set<int>& M, set<int>& D, set<int>& W, set<int>& h,
# 395 "kind.ag"
               time_t& exptime)
# 396 "kind.ag"
{
# 397 "kind.ag"
  for (unsigned int i = 0; i < rule.size(); ++i)
# 398 "kind.ag"
    rule[i] = tolower(rule[i]);
# 399 "kind.ag"

# 400 "kind.ag"
  substitute(rule, ' ', ',');
# 401 "kind.ag"
  reduceToOne(rule, ',');
# 402 "kind.ag"

# 403 "kind.ag"
  // rule = hour wday mday month <exptime>
# 404 "kind.ag"
  Lexer p(rule);
# 405 "kind.ag"

# 406 "kind.ag"
  h = getValues(p, 0, 23); // hour
# 407 "kind.ag"

# 408 "kind.ag"
  p.expect(',');
# 409 "kind.ag"

# 410 "kind.ag"
  W = getValues(p, 0, 7, 1); // wday
# 411 "kind.ag"

# 412 "kind.ag"
  p.expect(',');
# 413 "kind.ag"

# 414 "kind.ag"
  D = getValues(p, 1, 31); // day of month
# 415 "kind.ag"

# 416 "kind.ag"
  p.expect(',');
# 417 "kind.ag"

# 418 "kind.ag"
  M = getValues(p, 1, 12, 2); // month
# 419 "kind.ag"

# 420 "kind.ag"
#if 0
# 421 "kind.ag"
  // debug-output
# 422 "kind.ag"
  cout << "hour: ";
# 423 "kind.ag"
  for (int i : h)
# 424 "kind.ag"
    cout << i << " ";
# 425 "kind.ag"
  cout << endl;
# 426 "kind.ag"
  cout << "wday: ";
# 427 "kind.ag"
  for (int i : W)
# 428 "kind.ag"
    cout << i << " ";
# 429 "kind.ag"
  cout << endl;
# 430 "kind.ag"
  cout << "mday: ";
# 431 "kind.ag"
  for (int i : D)
# 432 "kind.ag"
    cout << i << " ";
# 433 "kind.ag"
  cout << endl;
# 434 "kind.ag"
  cout << "month: ";
# 435 "kind.ag"
  for (int i : M)
# 436 "kind.ag"
    cout << i << " ";
# 437 "kind.ag"
  cout << endl;
# 438 "kind.ag"
#endif
# 439 "kind.ag"

# 440 "kind.ag"
  string ts = p.getAll();
# 441 "kind.ag"
  substitute(ts, ',', ' ');
# 442 "kind.ag"
  exptime = stot(ts);
# 443 "kind.ag"
}
# 444 "kind.ag"

# 445 "kind.ag"
DateTime getExpireDate(const DateTime& imageTime, const KindConfig& conf, string& rule)
# 446 "kind.ag"
{
# 447 "kind.ag"
  DateTime expireTime;
# 448 "kind.ag"
  rule.clear();
# 449 "kind.ag"
  Strings expireRules = conf.getStrings("expireRule");
# 450 "kind.ag"
  for (unsigned int k = 0; k < expireRules.size(); ++k)
# 451 "kind.ag"
    {
# 452 "kind.ag"
      debugPrint("Checking rule " + expireRules[k]);
# 453 "kind.ag"

# 454 "kind.ag"
      set<int> M, D, W, h;
# 455 "kind.ag"
      set<int> Y, m, s;
# 456 "kind.ag"
      time_t expirePeriod;
# 457 "kind.ag"
      parseRule(expireRules[k], M, D, W, h, expirePeriod);
# 458 "kind.ag"

# 459 "kind.ag"
      if (imageTime.match(Y, M, D, W, h, m, s))
# 460 "kind.ag"
        {
# 461 "kind.ag"
          debugPrint("match");
# 462 "kind.ag"
          expireTime = imageTime + expirePeriod;
# 463 "kind.ag"
          rule = expireRules[k];
# 464 "kind.ag"
          // continue search: last rule matches
# 465 "kind.ag"
        }
# 466 "kind.ag"
    }
# 467 "kind.ag"
  if (rule.empty())
# 468 "kind.ag"
    throw Exception("expire", "no rule found");
# 469 "kind.ag"
  return expireTime;
# 470 "kind.ag"
}
# 471 "kind.ag"

# 472 "kind.ag"
DateTime expireDate(const string& image, const KindConfig& conf, string& rule)
# 473 "kind.ag"
{
# 474 "kind.ag"
  string expireFileName = image + "/expires";
# 475 "kind.ag"
  debugPrint("reading " + expireFileName);
# 476 "kind.ag"
#if 1
# 477 "kind.ag"
  if (fileExists(expireFileName))
# 478 "kind.ag"
    {
# 479 "kind.ag"
      Strings s;
# 480 "kind.ag"
      file2Strings(expireFileName, s);
# 481 "kind.ag"
      if (s.empty())
# 482 "kind.ag"
        throw Exception("expireDate", "expire empty");
# 483 "kind.ag"
      if (s.size() > 1)
# 484 "kind.ag"
        rule = s[1];
# 485 "kind.ag"
      return stringToDate(s[0]);
# 486 "kind.ag"
    }
# 487 "kind.ag"
#endif
# 488 "kind.ag"
  // epire date from image date + rules
# 489 "kind.ag"
  DateTime imageTime = imageDate(image);
# 490 "kind.ag"
  DateTime expireTime = getExpireDate(imageTime, conf, rule);
# 491 "kind.ag"

# 492 "kind.ag"
  // create file
# 493 "kind.ag"
  ofstream xfile(expireFileName);
# 494 "kind.ag"
  // we use prefix "expire-" to allow same parsing as for imagenames
# 495 "kind.ag"
  xfile << "expire-" << expireTime.getString('m') << endl;
# 496 "kind.ag"
  xfile << rule << endl;
# 497 "kind.ag"
  xfile.close();
# 498 "kind.ag"
  return expireTime;
# 499 "kind.ag"
}
# 500 "kind.ag"

# 501 "kind.ag"
int removeDir(const string& path)
# 502 "kind.ag"
{
# 503 "kind.ag"
  debugPrint("removeDir " + path);
# 504 "kind.ag"

# 505 "kind.ag"
  DIR* d = opendir(path.c_str());
# 506 "kind.ag"

# 507 "kind.ag"
  int r = -1;
# 508 "kind.ag"
  if (d)
# 509 "kind.ag"
    {
# 510 "kind.ag"
      struct dirent* p;
# 511 "kind.ag"

# 512 "kind.ag"
      r = 0;
# 513 "kind.ag"

# 514 "kind.ag"
      while (!r && (p = readdir(d)))
# 515 "kind.ag"
        {
# 516 "kind.ag"
          int r2 = 0;
# 517 "kind.ag"

# 518 "kind.ag"
          string fn = p->d_name;
# 519 "kind.ag"

# 520 "kind.ag"
          if (fn != "." && fn != "..")
# 521 "kind.ag"
            {
# 522 "kind.ag"
              fn = path + "/" + fn;
# 523 "kind.ag"

# 524 "kind.ag"
              debugPrint("-- " + fn);
# 525 "kind.ag"

# 526 "kind.ag"
              struct stat statbuf;
# 527 "kind.ag"
              if (lstat(fn.c_str(), &statbuf) == 0)
# 528 "kind.ag"
                {
# 529 "kind.ag"
                  if (S_ISLNK(statbuf.st_mode))
# 530 "kind.ag"
                    {
# 531 "kind.ag"
                      debugPrint("Remove link " + fn);
# 532 "kind.ag"
                      r2 = unlink(fn.c_str());
# 533 "kind.ag"
                    }
# 534 "kind.ag"
                  else if (S_ISDIR(statbuf.st_mode))
# 535 "kind.ag"
                    {
# 536 "kind.ag"
                      debugPrint("Remove dir " + fn);
# 537 "kind.ag"
                      r2 = removeDir(fn);
# 538 "kind.ag"
                    }
# 539 "kind.ag"
                  else
# 540 "kind.ag"
                    {
# 541 "kind.ag"
                      debugPrint("Remove file " + fn);
# 542 "kind.ag"
                      r2 = unlink(fn.c_str());
# 543 "kind.ag"
                    }
# 544 "kind.ag"
                }
# 545 "kind.ag"
              else
# 546 "kind.ag"
                {
# 547 "kind.ag"
                  cout << "stat(" << fn << ") failed" << endl;
# 548 "kind.ag"
                  // we assume "file" here
# 549 "kind.ag"
                  r2 = unlink(fn.c_str());
# 550 "kind.ag"
                }
# 551 "kind.ag"
            }
# 552 "kind.ag"
          r = r2;
# 553 "kind.ag"
        }
# 554 "kind.ag"

# 555 "kind.ag"
      closedir(d);
# 556 "kind.ag"
    }
# 557 "kind.ag"

# 558 "kind.ag"
  if (r == 0)
# 559 "kind.ag"
    {
# 560 "kind.ag"
      debugPrint("Remove Dir itself " + path);
# 561 "kind.ag"

# 562 "kind.ag"
      r = rmdir(path.c_str());
# 563 "kind.ag"
    }
# 564 "kind.ag"

# 565 "kind.ag"
  return r;
# 566 "kind.ag"
}
# 567 "kind.ag"

# 568 "kind.ag"
#if 0
# 569 "kind.ag"
int removeDir(const string& dname)
# 570 "kind.ag"
{
# 571 "kind.ag"
  int rc = 0;
# 572 "kind.ag"
  if (!dryRun)
# 573 "kind.ag"
    {
# 574 "kind.ag"
      Strings files;
# 575 "kind.ag"
      // subdirectories
# 576 "kind.ag"
      dirList(dname, files);
# 577 "kind.ag"
      for (unsigned int i = 0; i < files.size(); ++i)
# 578 "kind.ag"
        {
# 579 "kind.ag"
          debugPrint("Remove dir " + files[i]);
# 580 "kind.ag"
          for (unsigned int i = 0; i < files.size(); ++i)
# 581 "kind.ag"
            rc += removeDir(files[i]);
# 582 "kind.ag"
        }
# 583 "kind.ag"
      files.clear();
# 584 "kind.ag"

# 585 "kind.ag"
      // files in directory
# 586 "kind.ag"
      fileList(dname, files);
# 587 "kind.ag"
      for (unsigned int i = 0; i < files.size(); ++i)
# 588 "kind.ag"
        {
# 589 "kind.ag"
          debugPrint("unlink " + files[i]);
# 590 "kind.ag"
          if (!dryRun)
# 591 "kind.ag"
            {
# 592 "kind.ag"
              if (unlink(files[i].c_str()) != 0)
# 593 "kind.ag"
                rc++;
# 594 "kind.ag"
            }
# 595 "kind.ag"
        }
# 596 "kind.ag"
      debugPrint("rmdir " + dname);
# 597 "kind.ag"

# 598 "kind.ag"
      // directory
# 599 "kind.ag"
      if (rmdir(dname.c_str()) != 0)
# 600 "kind.ag"
        rc++;
# 601 "kind.ag"
    }
# 602 "kind.ag"

# 603 "kind.ag"
  return rc;
# 604 "kind.ag"
}
# 605 "kind.ag"
#endif
# 606 "kind.ag"

# 607 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 608 "kind.ag"
{
# 609 "kind.ag"
  if (!quiet)
# 610 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 611 "kind.ag"

# 612 "kind.ag"
  readVaultConfig(vault, conf);
# 613 "kind.ag"

# 614 "kind.ag"
  string vaultpath = findVault(vault);
# 615 "kind.ag"

# 616 "kind.ag"
  Strings dirlist; // list of subdirectories
# 617 "kind.ag"
  dirList(vaultpath, dirlist);
# 618 "kind.ag"

# 619 "kind.ag"
  Strings validImages;
# 620 "kind.ag"
  Strings invalidImages;
# 621 "kind.ag"
  string imgname = getImageName(conf);
# 622 "kind.ag"

# 623 "kind.ag"
  for (unsigned int i = 0; i < dirlist.size(); ++i)
# 624 "kind.ag"
    {
# 625 "kind.ag"
      FileName fn(dirlist[i]);
# 626 "kind.ag"
      if (startsWith(fn.getName(), imgname)) // dir is image ?
# 627 "kind.ag"
        {
# 628 "kind.ag"
          debugPrint(dirlist[i]);
# 629 "kind.ag"

# 630 "kind.ag"
          DateTime t = imageDate(dirlist[i]);
# 631 "kind.ag"

# 632 "kind.ag"
          if (t != now) // ignore just created image
# 633 "kind.ag"
            {
# 634 "kind.ag"
              if (!isValidImage(dirlist[i])) // invalid image?
# 635 "kind.ag"
                {
# 636 "kind.ag"
                  invalidImages.push_back(dirlist[i]);
# 637 "kind.ag"
                  debugPrint("- invalid image");
# 638 "kind.ag"
                }
# 639 "kind.ag"
              else
# 640 "kind.ag"
                {
# 641 "kind.ag"
                  validImages.push_back(dirlist[i]);
# 642 "kind.ag"
                  debugPrint("- valid image");
# 643 "kind.ag"
                }
# 644 "kind.ag"
            }
# 645 "kind.ag"
          else
# 646 "kind.ag"
            debugPrint("- current image - ignored");
# 647 "kind.ag"
        }
# 648 "kind.ag"
    }
# 649 "kind.ag"

# 650 "kind.ag"
  for (unsigned int i = 0; i < invalidImages.size(); ++i)
# 651 "kind.ag"
    {
# 652 "kind.ag"
      try
# 653 "kind.ag"
        {
# 654 "kind.ag"
          DateTime t = imageDate(invalidImages[i]);
# 655 "kind.ag"
          DateTime expireTime = t + stot(conf.getString("expireFailedImage"));
# 656 "kind.ag"
          if (debug)
# 657 "kind.ag"
            {
# 658 "kind.ag"
              cout << "image: " << t.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 659 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 660 "kind.ag"
            }
# 661 "kind.ag"
          if (expireTime < now)
# 662 "kind.ag"
            {
# 663 "kind.ag"
              if (!quiet)
# 664 "kind.ag"
                cout << "  removing invalid image " << invalidImages[i] << endl;
# 665 "kind.ag"
              if (removeDir(invalidImages[i]) != 0)
# 666 "kind.ag"
                cout << "Error removing " <<  invalidImages[i] << endl;
# 667 "kind.ag"
            }
# 668 "kind.ag"
        }
# 669 "kind.ag"
      catch (Exception ex)
# 670 "kind.ag"
        {
# 671 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 672 "kind.ag"
        }
# 673 "kind.ag"
    }
# 674 "kind.ag"

# 675 "kind.ag"
  sort(validImages.begin(), validImages.end()); // lexicographical order == temporal order
# 676 "kind.ag"
  for (unsigned int i = 0;
# 677 "kind.ag"
  i < validImages.size() - 1; // never expire latest image
# 678 "kind.ag"
  ++i)
# 679 "kind.ag"
    {
# 680 "kind.ag"
      try
# 681 "kind.ag"
        {
# 682 "kind.ag"
          string imageName = validImages[i];
# 683 "kind.ag"
          DateTime imageTime = imageDate(imageName);
# 684 "kind.ag"
          string rule;
# 685 "kind.ag"
          DateTime expireTime = expireDate(imageName, conf, rule);
# 686 "kind.ag"

# 687 "kind.ag"
          if (debug)
# 688 "kind.ag"
            {
# 689 "kind.ag"
              cout << "image: " << imageTime.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 690 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 691 "kind.ag"
            }
# 692 "kind.ag"
          if (now > expireTime)
# 693 "kind.ag"
            {
# 694 "kind.ag"
              if (!quiet)
# 695 "kind.ag"
                cout << "removing " << imageName << " rule=" << rule << endl;
# 696 "kind.ag"
              removeDir(imageName);
# 697 "kind.ag"
            }
# 698 "kind.ag"
        }
# 699 "kind.ag"
      catch (Exception ex)
# 700 "kind.ag"
        {
# 701 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 702 "kind.ag"
        }
# 703 "kind.ag"
    }
# 704 "kind.ag"
}
# 705 "kind.ag"

# 706 "kind.ag"
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

void error(const string& msg)
{
  cout << endl << ag_programName << " - error: " << msg << endl << endl;
  usage();
}

int ptoi(const char* para)
{
  char* end;
  int res = strtol(para, &end, 10);
  if (end == para)
    error(string("no int: ") + para);
  if (*end != 0)
    error(string("garbage in int: ") + para);
  return res;
}

double ptod(const char* para)
{
  char* end;
  double res = strtod(para, &end);
  if (end == para)
    error(string("no double: ") + para);
  if (*end != 0)
    error(string("garbage in double: ") + para);
  return res;
}

int main(int argc, char** argv)
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
# 710 "kind.ag"

# 711 "kind.ag"
  int exitCode = 0;
# 712 "kind.ag"
  string lockFile;
# 713 "kind.ag"
  try
# 714 "kind.ag"
    {
# 715 "kind.ag"
      if (debug)
# 716 "kind.ag"
        verbose = true;
# 717 "kind.ag"

# 718 "kind.ag"
      KindConfig conf;
# 719 "kind.ag"

# 720 "kind.ag"
      // default-values
# 721 "kind.ag"
      conf.add("imageName", "image");
# 722 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 723 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 724 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 725 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 726 "kind.ag"
      conf.add("remoteShell", "");
# 727 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 728 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 729 "kind.ag"
      conf.add("userExcludeCommand",
# 730 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 731 "kind.ag"
      conf.add("logSize", "");
# 732 "kind.ag"

# 733 "kind.ag"
      if (listConfig)
# 734 "kind.ag"
        {
# 735 "kind.ag"
          cout << "builtin config" << endl;
# 736 "kind.ag"
          conf.print(".   ");
# 737 "kind.ag"
        }
# 738 "kind.ag"

# 739 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 740 "kind.ag"

# 741 "kind.ag"
      banks = conf.getStrings("bank");
# 742 "kind.ag"
      if (banks.empty())
# 743 "kind.ag"
        throw Exception("read master config", "no banks defined");
# 744 "kind.ag"

# 745 "kind.ag"
      if (listConfig)
# 746 "kind.ag"
        {
# 747 "kind.ag"
          cout << "global config:" << endl;
# 748 "kind.ag"
          conf.print(".   ");
# 749 "kind.ag"
          readVaultConfig(vault, conf);
# 750 "kind.ag"
          cout << "vault config:" << endl;
# 751 "kind.ag"
          conf.print(".   ");
# 752 "kind.ag"
          exit(0);
# 753 "kind.ag"
        }
# 754 "kind.ag"

# 755 "kind.ag"
      lockFile = conf.getString("lockfile");
# 756 "kind.ag"
      createLock(lockFile);
# 757 "kind.ag"

# 758 "kind.ag"
      DateTime imageTime = DateTime::now();
# 759 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 760 "kind.ag"
      if (!logSizeFile.empty() && fileExists(logSizeFile))
# 761 "kind.ag"
        {
# 762 "kind.ag"
          vector<string> ss;
# 763 "kind.ag"
          file2Strings(logSizeFile, ss);
# 764 "kind.ag"
          for (auto s : ss)
# 765 "kind.ag"
            {
# 766 "kind.ag"
              unsigned int i = 0;
# 767 "kind.ag"
              string v = getWord(s, i);
# 768 "kind.ag"
              long int s1 = getLongInt(s, i);
# 769 "kind.ag"
              long int s2 = getLongInt(s, i);
# 770 "kind.ag"
              sizes[v] = Sizes(s1, s2);
# 771 "kind.ag"
            }
# 772 "kind.ag"
        }
# 773 "kind.ag"

# 774 "kind.ag"
      vector<string> vaults;
# 775 "kind.ag"
      string groupname = "group_" + vault;
# 776 "kind.ag"
      if (conf.hasKey(groupname))
# 777 "kind.ag"
        vaults = conf.getStrings(groupname);
# 778 "kind.ag"
      else
# 779 "kind.ag"
        vaults.push_back(vault);
# 780 "kind.ag"

# 781 "kind.ag"
      if (!expireOnly)
# 782 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 783 "kind.ag"
          {
# 784 "kind.ag"
            backupVault(vaults[i], conf, imageTime, fullImage);
# 785 "kind.ag"
            if (!logSizeFile.empty())
# 786 "kind.ag"
              {
# 787 "kind.ag"
                Strings st;
# 788 "kind.ag"
                for (auto s : sizes)
# 789 "kind.ag"
                  {
# 790 "kind.ag"
                    string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 791 "kind.ag"
                    st.push_back(h);
# 792 "kind.ag"
                  }
# 793 "kind.ag"
                strings2File(st, logSizeFile);
# 794 "kind.ag"
              }
# 795 "kind.ag"
          }
# 796 "kind.ag"

# 797 "kind.ag"
      if (!backupOnly)
# 798 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 799 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 800 "kind.ag"

# 801 "kind.ag"
      if (!quiet)
# 802 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 803 "kind.ag"

# 804 "kind.ag"
    }
# 805 "kind.ag"
  catch (const Exception& ex)
# 806 "kind.ag"
    {
# 807 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 808 "kind.ag"
      exitCode = 1;
# 809 "kind.ag"
    }
# 810 "kind.ag"
  catch (const char* msg)
# 811 "kind.ag"
    {
# 812 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 813 "kind.ag"
      exitCode = 1;
# 814 "kind.ag"
    }
# 815 "kind.ag"
  catch (const string& msg)
# 816 "kind.ag"
    {
# 817 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 818 "kind.ag"
      exitCode = 1;
# 819 "kind.ag"
    }
# 820 "kind.ag"
  removeLock(lockFile);
# 821 "kind.ag"
  return exitCode;
# 822 "kind.ag"
}
