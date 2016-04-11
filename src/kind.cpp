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
#include "Strings.h"
# 20 "kind.ag"
#include "FileName.h"
# 21 "kind.ag"

# 22 "kind.ag"
#include "KindConfig.h"
# 23 "kind.ag"
#include "filetools.h"
# 24 "kind.ag"
#include "Lexer.h"
# 25 "kind.ag"
#include "rulecomp.h"
# 26 "kind.ag"

# 27 "kind.ag"
#include "expiretools.h"
# 28 "kind.ag"

# 29 "kind.ag"
/*AppGen
# 30 "kind.ag"
  %%  Beschreibung des Programmes:
# 31 "kind.ag"
  prog: archiving backup
# 32 "kind.ag"
  %% Beschreibung Parameter
# 33 "kind.ag"
  % symbolischerName, Art, Typ,   Variablenname, Erklärung, Default-Wert
# 34 "kind.ag"
  para: vault_or_group, required, string, vault, Vault to backup
# 35 "kind.ag"
  %% Beschreibung der Optionen
# 36 "kind.ag"
  % kurz-Option, lang-Option, Typ, Variablenname, Erklärung, Default-Wert
# 37 "kind.ag"

# 38 "kind.ag"
  opt: f, full, void, fullImage, Force full image == initial backup, false
# 39 "kind.ag"
  opt: c, masterconfig, string, masterConfig, Master config file, ""
# 40 "kind.ag"
  opt2: if not given or empty kind looks for
# 41 "kind.ag"
  opt2:   /etc/kind/master.conf
# 42 "kind.ag"
  opt2:   /ffp/etc/kind/master.conf
# 43 "kind.ag"
  opt: B, backup, void, doBackup, Backup, false
# 44 "kind.ag"
  opt: E, expire, void, doExpire, Expire, false
# 45 "kind.ag"
  opt: C, listconfig, void, listConfig, Show configuration, false
# 46 "kind.ag"
  opt2:   if none of backup, expire or listconfig is specified,
# 47 "kind.ag"
  opt2:   backup and expire is assumed.
# 48 "kind.ag"
  opt: D, dryrun, Void, dryRun, Dry run (no real backup), false
# 49 "kind.ag"
  opt: v, verbose, Void, verbose,  Verbose,  false
# 50 "kind.ag"
  opt: d, debug, Void, debug, Debug output of many data, false
# 51 "kind.ag"
  opt: q, quiet, Void, quiet, Be quiet - no messages, false
# 52 "kind.ag"
  opt: h, help, usage, ignored , This help
# 53 "kind.ag"
AppGen*/
# 54 "kind.ag"

# 55 "kind.ag"
using namespace std;
# 56 "kind.ag"

# 57 "kind.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <string>
bool dryRun = false;
bool verbose = false;
bool debug = false;
bool quiet = false;
/*AppGen:GlobalEnd*/
# 58 "kind.ag"

# 59 "kind.ag"
void createExpireFile(const string& image, const KindConfig& conf, string& rule);
# 60 "kind.ag"

# 61 "kind.ag"
Strings banks;
# 62 "kind.ag"

# 63 "kind.ag"
typedef pair<long int, long int> Sizes;
# 64 "kind.ag"
map<string, Sizes> sizes;
# 65 "kind.ag"

# 66 "kind.ag"
void verbosePrint(const string& text)
# 67 "kind.ag"
{
# 68 "kind.ag"
  if (verbose)
# 69 "kind.ag"
    cout << "  " << text << endl;
# 70 "kind.ag"
}
# 71 "kind.ag"

# 72 "kind.ag"
void debugPrint(const string& text)
# 73 "kind.ag"
{
# 74 "kind.ag"
  if (verbose)
# 75 "kind.ag"
    cout << "    " << text << endl;
# 76 "kind.ag"
}
# 77 "kind.ag"

# 78 "kind.ag"
void readMasterConfig1(const string& fn, KindConfig& conf)
# 79 "kind.ag"
{
# 80 "kind.ag"
  verbosePrint("reading master config " + fn);
# 81 "kind.ag"
  conf.addFile(fn);
# 82 "kind.ag"
}
# 83 "kind.ag"

# 84 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 85 "kind.ag"
{
# 86 "kind.ag"
  if (!fn.empty())  // master config given by user on commandline
# 87 "kind.ag"
    readMasterConfig1(fn, conf);
# 88 "kind.ag"
  else if (fileExists("/etc/kind/master.conf"))
# 89 "kind.ag"
    readMasterConfig1("/etc/kind/master.conf", conf);
# 90 "kind.ag"
  else if (fileExists("/ffp/etc/kind/master.conf"))
# 91 "kind.ag"
    readMasterConfig1("/ffp/etc/kind/master.conf", conf);
# 92 "kind.ag"
  else
# 93 "kind.ag"
    throw Exception("MasterConfig", "no file");
# 94 "kind.ag"
}
# 95 "kind.ag"

# 96 "kind.ag"
string findVault(const string& v)
# 97 "kind.ag"
{
# 98 "kind.ag"
  bool found = false;
# 99 "kind.ag"
  FileName fn;
# 100 "kind.ag"
  fn.setName(v);
# 101 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 102 "kind.ag"
    {
# 103 "kind.ag"
      fn.setPath(banks[i]);
# 104 "kind.ag"
      if (dirExists(fn.getFileName()))
# 105 "kind.ag"
        found = true;
# 106 "kind.ag"
    }
# 107 "kind.ag"
  if (!found)
# 108 "kind.ag"
    throw Exception("find vault", v + " not found");
# 109 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 110 "kind.ag"
  return fn.getFileName();
# 111 "kind.ag"
}
# 112 "kind.ag"

# 113 "kind.ag"
void readVaultConfig(const string& vault, KindConfig& conf)
# 114 "kind.ag"
{
# 115 "kind.ag"
  string vaultpath = findVault(vault);
# 116 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 117 "kind.ag"
  verbosePrint("reading vault config:");
# 118 "kind.ag"
  verbosePrint("  " + vaultConfigName);
# 119 "kind.ag"
  conf.addFile(vaultConfigName);
# 120 "kind.ag"
}
# 121 "kind.ag"

# 122 "kind.ag"
string getImageName(const KindConfig& conf)
# 123 "kind.ag"
{
# 124 "kind.ag"
  bool nonPortable = false;
# 125 "kind.ag"
  string res = conf.getString("imageName");
# 126 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < res.size(); ++i)
# 127 "kind.ag"
    {
# 128 "kind.ag"
      char c = res[i];
# 129 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 130 "kind.ag"
        nonPortable = true;
# 131 "kind.ag"
    }
# 132 "kind.ag"
  if (nonPortable)
# 133 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + res);
# 134 "kind.ag"
  return res;
# 135 "kind.ag"
}
# 136 "kind.ag"

# 137 "kind.ag"
bool isValidImage(const string& imageName)
# 138 "kind.ag"
{
# 139 "kind.ag"
  return dirExists(imageName) &&
# 140 "kind.ag"
         !fileExists(imageName + "/error") &&
# 141 "kind.ag"
         dirExists(imageName + "/tree");
# 142 "kind.ag"
}
# 143 "kind.ag"

# 144 "kind.ag"
Strings findValidImages(const string& vaultpath, const KindConfig& conf)
# 145 "kind.ag"
{
# 146 "kind.ag"
  Strings imageList;
# 147 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 148 "kind.ag"
  dirList(vaultpath, imageList);
# 149 "kind.ag"

# 150 "kind.ag"
  Strings validImageList;
# 151 "kind.ag"
  for (unsigned int i = 0; i < imageList.size(); ++i)
# 152 "kind.ag"
    {
# 153 "kind.ag"
      FileName fn(imageList[i]);
# 154 "kind.ag"
      string imgname = getImageName(conf);
# 155 "kind.ag"
      int len = imgname.length();
# 156 "kind.ag"
      if (fn.getName().substr(0, len) == imgname)
# 157 "kind.ag"
        {
# 158 "kind.ag"
          debugPrint("Checking " + imageList[i]);
# 159 "kind.ag"
          if (isValidImage(imageList[i]))
# 160 "kind.ag"
            validImageList.push_back(imageList[i]);
# 161 "kind.ag"
        }
# 162 "kind.ag"
    }
# 163 "kind.ag"
  if (validImageList.empty())
# 164 "kind.ag"
    throw Exception("Find reference", "No reference found");
# 165 "kind.ag"
  sort(validImageList.begin(), validImageList.end());
# 166 "kind.ag"
  return validImageList;
# 167 "kind.ag"
}
# 168 "kind.ag"

# 169 "kind.ag"
void backupVault(const string& vault,
# 170 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 171 "kind.ag"
                 const DateTime& imageTime,
# 172 "kind.ag"
                 bool fullImage)
# 173 "kind.ag"
{
# 174 "kind.ag"
  if (!quiet)
# 175 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 176 "kind.ag"
  try
# 177 "kind.ag"
    {
# 178 "kind.ag"
      sizes[vault].second = 0; // nothing backed up yet
# 179 "kind.ag"

# 180 "kind.ag"
      readVaultConfig(vault, conf);
# 181 "kind.ag"

# 182 "kind.ag"
      // where to store
# 183 "kind.ag"
      string vaultpath = findVault(vault);
# 184 "kind.ag"

# 185 "kind.ag"
      // image path
# 186 "kind.ag"
      string imageName = getImageName(conf);
# 187 "kind.ag"
      if (!imageName.empty())
# 188 "kind.ag"
        imageName += '-';
# 189 "kind.ag"
      string imageFullName =  vaultpath + "/" + imageName ;
# 190 "kind.ag"

# 191 "kind.ag"
      if (conf.getBool("longImageName"))
# 192 "kind.ag"
        imageFullName += imageTime.getString('m');
# 193 "kind.ag"
      else
# 194 "kind.ag"
        imageFullName += imageTime.getString('s');
# 195 "kind.ag"

# 196 "kind.ag"
      verbosePrint("backup to \"" + imageFullName + "\"");
# 197 "kind.ag"

# 198 "kind.ag"
      // find reference image
# 199 "kind.ag"
      string referenceImage;
# 200 "kind.ag"
      if (!fullImage)
# 201 "kind.ag"
        {
# 202 "kind.ag"
          Strings validImageList = findValidImages(vaultpath, conf);
# 203 "kind.ag"
          // last image is newest image
# 204 "kind.ag"
          referenceImage = validImageList.back();
# 205 "kind.ag"
        }
# 206 "kind.ag"

# 207 "kind.ag"
      // create image path
# 208 "kind.ag"
      if (!dryRun)
# 209 "kind.ag"
        if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 210 "kind.ag"
          throw Exception("Create image", "failed to create " + imageFullName);
# 211 "kind.ag"

# 212 "kind.ag"
      // error message
# 213 "kind.ag"
      // we write an generic error message to mark backup as unsuccessful
# 214 "kind.ag"
      // will be deleted at successful end of rsync
# 215 "kind.ag"
      string errorfile = imageFullName + "/error";
# 216 "kind.ag"
      if (!dryRun)
# 217 "kind.ag"
        {
# 218 "kind.ag"
          ofstream error(errorfile);
# 219 "kind.ag"
          error << "failed" << endl;
# 220 "kind.ag"
          error.close();
# 221 "kind.ag"
        }
# 222 "kind.ag"

# 223 "kind.ag"
      // create source descriptor
# 224 "kind.ag"
      string host;
# 225 "kind.ag"
      if (conf.hasKey("host"))
# 226 "kind.ag"
        host = conf.getString("host");
# 227 "kind.ag"

# 228 "kind.ag"
      string server;
# 229 "kind.ag"
      if (conf.hasKey("server"))
# 230 "kind.ag"
        server = conf.getString("server");
# 231 "kind.ag"

# 232 "kind.ag"
      if (!host.empty() && !server.empty())
# 233 "kind.ag"
        throw Exception("backupVault", "Cannot have host and server");
# 234 "kind.ag"

# 235 "kind.ag"
      string path = conf.getString("path");
# 236 "kind.ag"
      if (path.empty())
# 237 "kind.ag"
        throw Exception("rsync", "empty source path");
# 238 "kind.ag"
      if (path.back() != '/')
# 239 "kind.ag"
        path += '/';
# 240 "kind.ag"

# 241 "kind.ag"
      string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 242 "kind.ag"
      if (!conf.getBool("ignorePermission"))
# 243 "kind.ag"
        rsyncCmd += "-pgo";
# 244 "kind.ag"
      vector<string> rso = conf.getStrings("rsyncOption");
# 245 "kind.ag"
      for (const string& opt : rso)
# 246 "kind.ag"
        rsyncCmd += opt + " ";
# 247 "kind.ag"

# 248 "kind.ag"
      // excludes
# 249 "kind.ag"
      Strings excluded;
# 250 "kind.ag"

# 251 "kind.ag"
      if (conf.hasKey("exclude"))
# 252 "kind.ag"
        excluded += conf.getStrings("exclude");
# 253 "kind.ag"

# 254 "kind.ag"
      if (!host.empty())  // shell mode
# 255 "kind.ag"
        {
# 256 "kind.ag"
          // cout << "USING SHELLMODE '" << host << "'" << endl;
# 257 "kind.ag"
          string remoteShell = conf.getString("remoteShell");
# 258 "kind.ag"
          string userAtHost = conf.getString("user") + "@" +
# 259 "kind.ag"
                              conf.getString("host");
# 260 "kind.ag"
          string rshCommand = remoteShell;
# 261 "kind.ag"
          if (remoteShell.empty())
# 262 "kind.ag"
            rshCommand = "ssh";
# 263 "kind.ag"

# 264 "kind.ag"
          rshCommand += " " + userAtHost;
# 265 "kind.ag"

# 266 "kind.ag"
          string userExcludeCommand = conf.getString("userExcludeCommand");
# 267 "kind.ag"

# 268 "kind.ag"
          if (!userExcludeCommand.empty())
# 269 "kind.ag"
            {
# 270 "kind.ag"
              replacePlaceHolder(userExcludeCommand, "%path", path);
# 271 "kind.ag"
              string excludeCommand = rshCommand + " " + userExcludeCommand;
# 272 "kind.ag"

# 273 "kind.ag"
              verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 274 "kind.ag"

# 275 "kind.ag"
              int rc;
# 276 "kind.ag"
              excluded = myPopen(excludeCommand, rc, debug);
# 277 "kind.ag"
              if (rc > 0)
# 278 "kind.ag"
                throw Exception("Find exludes", "Search for excludes failed");
# 279 "kind.ag"

# 280 "kind.ag"
              for (unsigned int i = 0; i < excluded.size(); ++i)
# 281 "kind.ag"
                {
# 282 "kind.ag"
                  FileName fn(excluded[i]);
# 283 "kind.ag"
                  excluded[i] = '/' + fn.getPath();
# 284 "kind.ag"
                  debugPrint("Excluding: " + excluded[i]);
# 285 "kind.ag"
                }
# 286 "kind.ag"
            }
# 287 "kind.ag"

# 288 "kind.ag"
          string userExcludeFile = conf.getString("userExcludeFile");
# 289 "kind.ag"
          if (!userExcludeFile.empty())
# 290 "kind.ag"
            {
# 291 "kind.ag"
              userExcludeFile = path + userExcludeFile;
# 292 "kind.ag"
              string getExcludeFileCommand = rshCommand;
# 293 "kind.ag"
              getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 294 "kind.ag"
              getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 295 "kind.ag"
              // cout << getExcludeFileCommand << endl;
# 296 "kind.ag"
              int rc;
# 297 "kind.ag"
              Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 298 "kind.ag"
              if (rc == 0)
# 299 "kind.ag"
                excluded += excludes2;
# 300 "kind.ag"
            }
# 301 "kind.ag"

# 302 "kind.ag"
          if (!dryRun)
# 303 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 304 "kind.ag"

# 305 "kind.ag"
          // rsync image
# 306 "kind.ag"

# 307 "kind.ag"
          if (!remoteShell.empty())
# 308 "kind.ag"
            rsyncCmd += " -e \'" + remoteShell + "\' ";
# 309 "kind.ag"

# 310 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 311 "kind.ag"
          if (!referenceImage.empty())
# 312 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 313 "kind.ag"
          rsyncCmd += userAtHost + ":" + path + " ";
# 314 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 315 "kind.ag"
        } // shell mode
# 316 "kind.ag"
      else
# 317 "kind.ag"
        {
# 318 "kind.ag"
          // cout << "USING SERVERMODE" << endl;
# 319 "kind.ag"

# 320 "kind.ag"
          if (!dryRun)
# 321 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 322 "kind.ag"

# 323 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 324 "kind.ag"
          if (!referenceImage.empty())
# 325 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 326 "kind.ag"
          rsyncCmd += conf.getString("server") + "::" + path + " ";
# 327 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 328 "kind.ag"
        }
# 329 "kind.ag"

# 330 "kind.ag"
      debugPrint("Action: " + rsyncCmd);
# 331 "kind.ag"

# 332 "kind.ag"
      vector<string> backupResult;
# 333 "kind.ag"
      if (!dryRun)
# 334 "kind.ag"
        {
# 335 "kind.ag"
          verbosePrint("syncing (" + rsyncCmd + ")");
# 336 "kind.ag"
          int rc;
# 337 "kind.ag"
          backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 338 "kind.ag"
          // strings2File(backupResult, imageFullName + "/rsync-log");
# 339 "kind.ag"
          if (rc == 0 ||
# 340 "kind.ag"
              rc == 24 || // "no error" or "vanished source files" (ignored)
# 341 "kind.ag"
              rc == 6144) // workaround for wrong exit code ??!!
# 342 "kind.ag"
            {
# 343 "kind.ag"
              unlink(errorfile.c_str());
# 344 "kind.ag"
              string lastLink = vaultpath + "/last";
# 345 "kind.ag"
              unlink(lastLink.c_str());
# 346 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 347 "kind.ag"
              long int st = 0;
# 348 "kind.ag"
              long int sc = 0;
# 349 "kind.ag"
              for (auto bl : backupResult)
# 350 "kind.ag"
                {
# 351 "kind.ag"
                  if (bl.substr(0, 15) == "Total file size")
# 352 "kind.ag"
                    st = getNumber(bl);
# 353 "kind.ag"
                  else if (bl.substr(0, 27) == "Total transferred file size")
# 354 "kind.ag"
                    sc = getNumber(bl);
# 355 "kind.ag"
                }
# 356 "kind.ag"
              // sizes[vault] = pair<long int, long int>(st, sc);
# 357 "kind.ag"
              sizes[vault] = Sizes(st, sc);
# 358 "kind.ag"
              //  cout << vault << " " << st << " || " << sc << endl;
# 359 "kind.ag"
            }
# 360 "kind.ag"
          else
# 361 "kind.ag"
            throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 362 "kind.ag"
        }
# 363 "kind.ag"
      else
# 364 "kind.ag"
        cout << "Not executing " << rsyncCmd << endl;
# 365 "kind.ag"

# 366 "kind.ag"
      string rule;
# 367 "kind.ag"
      createExpireFile(imageFullName, conf, rule);
# 368 "kind.ag"
    }
# 369 "kind.ag"
  catch (Exception ex)
# 370 "kind.ag"
    {
# 371 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 372 "kind.ag"
    }
# 373 "kind.ag"
}
# 374 "kind.ag"

# 375 "kind.ag"
int removeDir(const string& path)
# 376 "kind.ag"
{
# 377 "kind.ag"
  debugPrint("removeDir " + path);
# 378 "kind.ag"
  DIR* d = opendir(path.c_str());
# 379 "kind.ag"

# 380 "kind.ag"
  int r = -1;
# 381 "kind.ag"
  if (d)
# 382 "kind.ag"
    {
# 383 "kind.ag"
      struct dirent* p;
# 384 "kind.ag"

# 385 "kind.ag"
      r = 0;
# 386 "kind.ag"
      while (!r && (p = readdir(d)))
# 387 "kind.ag"
        {
# 388 "kind.ag"
          int r2 = 0;
# 389 "kind.ag"
          string fn = p->d_name;
# 390 "kind.ag"
          if (fn != "." && fn != "..")
# 391 "kind.ag"
            {
# 392 "kind.ag"
              fn = path + "/" + fn;
# 393 "kind.ag"
              debugPrint("-- " + fn);
# 394 "kind.ag"
              struct stat statbuf;
# 395 "kind.ag"
              if (lstat(fn.c_str(), &statbuf) == 0)
# 396 "kind.ag"
                {
# 397 "kind.ag"
                  if (S_ISLNK(statbuf.st_mode))
# 398 "kind.ag"
                    {
# 399 "kind.ag"
                      debugPrint("Remove link " + fn);
# 400 "kind.ag"
                      r2 = unlink(fn.c_str());
# 401 "kind.ag"
                    }
# 402 "kind.ag"
                  else if (S_ISDIR(statbuf.st_mode))
# 403 "kind.ag"
                    {
# 404 "kind.ag"
                      debugPrint("Remove dir " + fn);
# 405 "kind.ag"
                      r2 = removeDir(fn);
# 406 "kind.ag"
                    }
# 407 "kind.ag"
                  else
# 408 "kind.ag"
                    {
# 409 "kind.ag"
                      debugPrint("Remove file " + fn);
# 410 "kind.ag"
                      r2 = unlink(fn.c_str());
# 411 "kind.ag"
                    }
# 412 "kind.ag"
                }
# 413 "kind.ag"
              else
# 414 "kind.ag"
                {
# 415 "kind.ag"
                  cout << "stat(" << fn << ") failed" << endl;
# 416 "kind.ag"
                  // we assume "file" here
# 417 "kind.ag"
                  r2 = unlink(fn.c_str());
# 418 "kind.ag"
                }
# 419 "kind.ag"
            }
# 420 "kind.ag"
          r = r2;
# 421 "kind.ag"
        }
# 422 "kind.ag"
      closedir(d);
# 423 "kind.ag"
    }
# 424 "kind.ag"

# 425 "kind.ag"
  if (r == 0)
# 426 "kind.ag"
    {
# 427 "kind.ag"
      debugPrint("Remove Dir itself " + path);
# 428 "kind.ag"
      r = rmdir(path.c_str());
# 429 "kind.ag"
    }
# 430 "kind.ag"

# 431 "kind.ag"
  return r;
# 432 "kind.ag"
}
# 433 "kind.ag"

# 434 "kind.ag"
#if 0
# 435 "kind.ag"
int removeDir(const string& dname)
# 436 "kind.ag"
{
# 437 "kind.ag"
  int rc = 0;
# 438 "kind.ag"
  if (!dryRun)
# 439 "kind.ag"
    {
# 440 "kind.ag"
      Strings files;
# 441 "kind.ag"
      // subdirectories
# 442 "kind.ag"
      dirList(dname, files);
# 443 "kind.ag"
      for (unsigned int i = 0; i < files.size(); ++i)
# 444 "kind.ag"
        {
# 445 "kind.ag"
          debugPrint("Remove dir " + files[i]);
# 446 "kind.ag"
          for (unsigned int i = 0; i < files.size(); ++i)
# 447 "kind.ag"
            rc += removeDir(files[i]);
# 448 "kind.ag"
        }
# 449 "kind.ag"
      files.clear();
# 450 "kind.ag"

# 451 "kind.ag"
      // files in directory
# 452 "kind.ag"
      fileList(dname, files);
# 453 "kind.ag"
      for (unsigned int i = 0; i < files.size(); ++i)
# 454 "kind.ag"
        {
# 455 "kind.ag"
          debugPrint("unlink " + files[i]);
# 456 "kind.ag"
          if (!dryRun)
# 457 "kind.ag"
            {
# 458 "kind.ag"
              if (unlink(files[i].c_str()) != 0)
# 459 "kind.ag"
                rc++;
# 460 "kind.ag"
            }
# 461 "kind.ag"
        }
# 462 "kind.ag"
      debugPrint("rmdir " + dname);
# 463 "kind.ag"

# 464 "kind.ag"
      // directory
# 465 "kind.ag"
      if (rmdir(dname.c_str()) != 0)
# 466 "kind.ag"
        rc++;
# 467 "kind.ag"
    }
# 468 "kind.ag"

# 469 "kind.ag"
  return rc;
# 470 "kind.ag"
}
# 471 "kind.ag"
#endif
# 472 "kind.ag"

# 473 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 474 "kind.ag"
{
# 475 "kind.ag"
  if (!quiet)
# 476 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 477 "kind.ag"

# 478 "kind.ag"
  readVaultConfig(vault, conf);
# 479 "kind.ag"

# 480 "kind.ag"
  string vaultpath = findVault(vault);
# 481 "kind.ag"

# 482 "kind.ag"
  Strings dirlist; // list of subdirectories
# 483 "kind.ag"
  dirList(vaultpath, dirlist);
# 484 "kind.ag"

# 485 "kind.ag"
  Strings validImages;
# 486 "kind.ag"
  Strings invalidImages;
# 487 "kind.ag"
  string imgname = getImageName(conf);
# 488 "kind.ag"

# 489 "kind.ag"
  for (unsigned int i = 0; i < dirlist.size(); ++i)
# 490 "kind.ag"
    {
# 491 "kind.ag"
      FileName fn(dirlist[i]);
# 492 "kind.ag"
      if (startsWith(fn.getName(), imgname)) // dir is image ?
# 493 "kind.ag"
        {
# 494 "kind.ag"
          debugPrint(dirlist[i]);
# 495 "kind.ag"

# 496 "kind.ag"
          DateTime t = imageDate(dirlist[i]);
# 497 "kind.ag"

# 498 "kind.ag"
          if (t != now) // ignore just created image
# 499 "kind.ag"
            {
# 500 "kind.ag"
              if (!isValidImage(dirlist[i])) // invalid image?
# 501 "kind.ag"
                {
# 502 "kind.ag"
                  invalidImages.push_back(dirlist[i]);
# 503 "kind.ag"
                  debugPrint("- invalid image");
# 504 "kind.ag"
                }
# 505 "kind.ag"
              else
# 506 "kind.ag"
                {
# 507 "kind.ag"
                  validImages.push_back(dirlist[i]);
# 508 "kind.ag"
                  debugPrint("- valid image");
# 509 "kind.ag"
                }
# 510 "kind.ag"
            }
# 511 "kind.ag"
          else
# 512 "kind.ag"
            debugPrint("- current image - ignored");
# 513 "kind.ag"
        }
# 514 "kind.ag"
    }
# 515 "kind.ag"

# 516 "kind.ag"
  for (unsigned int i = 0; i < invalidImages.size(); ++i)
# 517 "kind.ag"
    {
# 518 "kind.ag"
      try
# 519 "kind.ag"
        {
# 520 "kind.ag"
          DateTime t = imageDate(invalidImages[i]);
# 521 "kind.ag"
          DateTime expireTime = t + stot(conf.getString("expireFailedImage"));
# 522 "kind.ag"
          if (debug)
# 523 "kind.ag"
            {
# 524 "kind.ag"
              cout << "image: " << t.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 525 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 526 "kind.ag"
            }
# 527 "kind.ag"
          if (expireTime < now)
# 528 "kind.ag"
            {
# 529 "kind.ag"
              if (!quiet)
# 530 "kind.ag"
                cout << "  removing invalid image " << invalidImages[i] << endl;
# 531 "kind.ag"
              if (removeDir(invalidImages[i]) != 0)
# 532 "kind.ag"
                cout << "Error removing " <<  invalidImages[i] << endl;
# 533 "kind.ag"
            }
# 534 "kind.ag"
        }
# 535 "kind.ag"
      catch (Exception ex)
# 536 "kind.ag"
        {
# 537 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 538 "kind.ag"
        }
# 539 "kind.ag"
    }
# 540 "kind.ag"

# 541 "kind.ag"
  sort(validImages.begin(), validImages.end()); // lexicographical order == temporal order
# 542 "kind.ag"
  for (unsigned int i = 0;
# 543 "kind.ag"
       i < validImages.size() - 1; // never expire latest image
# 544 "kind.ag"
       ++i)
# 545 "kind.ag"
    {
# 546 "kind.ag"
      try
# 547 "kind.ag"
        {
# 548 "kind.ag"
          string imageName = validImages[i];
# 549 "kind.ag"
          DateTime imageTime = imageDate(imageName);
# 550 "kind.ag"
          string rule;
# 551 "kind.ag"
          DateTime expireTime = expireDate(imageName, conf, rule);
# 552 "kind.ag"

# 553 "kind.ag"
          if (debug)
# 554 "kind.ag"
            {
# 555 "kind.ag"
              cout << "image: " << imageTime.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 556 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 557 "kind.ag"
            }
# 558 "kind.ag"
          if (now > expireTime)
# 559 "kind.ag"
            {
# 560 "kind.ag"
              if (!quiet)
# 561 "kind.ag"
                cout << "removing " << imageName << " rule=" << rule << endl;
# 562 "kind.ag"
              removeDir(imageName);
# 563 "kind.ag"
            }
# 564 "kind.ag"
        }
# 565 "kind.ag"
      catch (Exception ex)
# 566 "kind.ag"
        {
# 567 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 568 "kind.ag"
        }
# 569 "kind.ag"
    }
# 570 "kind.ag"
}
# 571 "kind.ag"

# 572 "kind.ag"
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
  cout << "  -B --backup        Backup (default: false)" << endl;
  cout << "  -E --expire        Expire (default: false)" << endl;
  cout << "  -C --listconfig    Show configuration (default: false)" << endl;
  cout << "                     if none of backup, expire or listconfig is specified," << endl;
  cout << "                     backup and expire is assumed." << endl;
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
bool doBackup = false;
bool doExpire = false;
bool listConfig = false;

string vault = "";
  static struct option ag_long_options[] =
  {
    {"full", no_argument, 0, 'f' },
    {"masterconfig", required_argument, 0, 'c' },
    {"backup", no_argument, 0, 'B' },
    {"expire", no_argument, 0, 'E' },
    {"listconfig", no_argument, 0, 'C' },
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
  while ((rc = getopt_long(argc, argv, ":fc:BECDvdqh", ag_long_options, NULL)) >= 0)
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
              doBackup = true;
              break;

        case 'E':
              doExpire = true;
              break;

        case 'C':
              listConfig = true;
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
# 576 "kind.ag"

# 577 "kind.ag"
  int exitCode = 0;
# 578 "kind.ag"
  string lockFile;
# 579 "kind.ag"
  try
# 580 "kind.ag"
    {
# 581 "kind.ag"
      // handling of parameters and switches
# 582 "kind.ag"
      if (debug)        // debug implies verbose
# 583 "kind.ag"
        verbose = true;
# 584 "kind.ag"

# 585 "kind.ag"
      if (!doBackup && !doExpire && !listConfig)
# 586 "kind.ag"
        {
# 587 "kind.ag"
          doBackup = true;
# 588 "kind.ag"
          doExpire = true;
# 589 "kind.ag"
        }
# 590 "kind.ag"

# 591 "kind.ag"
      KindConfig conf;
# 592 "kind.ag"

# 593 "kind.ag"
      // default-values
# 594 "kind.ag"
      conf.add("imageName", "image");
# 595 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 596 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 597 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 598 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 599 "kind.ag"
      conf.add("remoteShell", "");
# 600 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 601 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 602 "kind.ag"
      conf.add("userExcludeCommand",
# 603 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 604 "kind.ag"
      conf.add("logSize", "");
# 605 "kind.ag"

# 606 "kind.ag"
      if (listConfig)
# 607 "kind.ag"
        {
# 608 "kind.ag"
          cout << "builtin config" << endl;
# 609 "kind.ag"
          conf.print(".   ");
# 610 "kind.ag"
        }
# 611 "kind.ag"

# 612 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 613 "kind.ag"

# 614 "kind.ag"
      banks = conf.getStrings("bank");
# 615 "kind.ag"
      if (banks.empty())
# 616 "kind.ag"
        throw Exception("read master config", "no banks defined");
# 617 "kind.ag"

# 618 "kind.ag"
      if (listConfig)
# 619 "kind.ag"
        {
# 620 "kind.ag"
          cout << "global config:" << endl;
# 621 "kind.ag"
          conf.print(".   ");
# 622 "kind.ag"
          readVaultConfig(vault, conf);
# 623 "kind.ag"
          cout << "vault config:" << endl;
# 624 "kind.ag"
          conf.print(".   ");
# 625 "kind.ag"
          exit(0);
# 626 "kind.ag"
        }
# 627 "kind.ag"

# 628 "kind.ag"
      lockFile = conf.getString("lockfile");
# 629 "kind.ag"
      createLock(lockFile);
# 630 "kind.ag"

# 631 "kind.ag"
      DateTime imageTime = DateTime::now();
# 632 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 633 "kind.ag"
      if (!logSizeFile.empty() && fileExists(logSizeFile))
# 634 "kind.ag"
        {
# 635 "kind.ag"
          vector<string> ss;
# 636 "kind.ag"
          file2Strings(logSizeFile, ss);
# 637 "kind.ag"
          for (auto s : ss)
# 638 "kind.ag"
            {
# 639 "kind.ag"
              unsigned int i = 0;
# 640 "kind.ag"
              string v = getWord(s, i);
# 641 "kind.ag"
              long int s1 = getLongInt(s, i);
# 642 "kind.ag"
              long int s2 = getLongInt(s, i);
# 643 "kind.ag"
              sizes[v] = Sizes(s1, s2);
# 644 "kind.ag"
            }
# 645 "kind.ag"
        }
# 646 "kind.ag"

# 647 "kind.ag"
      vector<string> vaults;
# 648 "kind.ag"
      string groupname = "group_" + vault;
# 649 "kind.ag"
      if (conf.hasKey(groupname))
# 650 "kind.ag"
        vaults = conf.getStrings(groupname);
# 651 "kind.ag"
      else
# 652 "kind.ag"
        vaults.push_back(vault);
# 653 "kind.ag"

# 654 "kind.ag"
      if (doBackup)
# 655 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 656 "kind.ag"
          {
# 657 "kind.ag"
            backupVault(vaults[i], conf, imageTime, fullImage);
# 658 "kind.ag"
            if (!logSizeFile.empty())
# 659 "kind.ag"
              {
# 660 "kind.ag"
                Strings st;
# 661 "kind.ag"
                for (auto s : sizes)
# 662 "kind.ag"
                  {
# 663 "kind.ag"
                    string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 664 "kind.ag"
                    st.push_back(h);
# 665 "kind.ag"
                  }
# 666 "kind.ag"
                strings2File(st, logSizeFile);
# 667 "kind.ag"
              }
# 668 "kind.ag"
          }
# 669 "kind.ag"

# 670 "kind.ag"
      if (doExpire)
# 671 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 672 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 673 "kind.ag"

# 674 "kind.ag"
      if (!quiet)
# 675 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 676 "kind.ag"

# 677 "kind.ag"
    }
# 678 "kind.ag"
  catch (const Exception& ex)
# 679 "kind.ag"
    {
# 680 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 681 "kind.ag"
      exitCode = 1;
# 682 "kind.ag"
    }
# 683 "kind.ag"
  catch (const char* msg)
# 684 "kind.ag"
    {
# 685 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 686 "kind.ag"
      exitCode = 1;
# 687 "kind.ag"
    }
# 688 "kind.ag"
  catch (const string& msg)
# 689 "kind.ag"
    {
# 690 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 691 "kind.ag"
      exitCode = 1;
# 692 "kind.ag"
    }
# 693 "kind.ag"
  removeLock(lockFile);
# 694 "kind.ag"
  return exitCode;
# 695 "kind.ag"
}
