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
#include "Image.h"
# 22 "kind.ag"

# 23 "kind.ag"
#include "KindConfig.h"
# 24 "kind.ag"
#include "filetools.h"
# 25 "kind.ag"
#include "Lexer.h"
# 26 "kind.ag"
#include "rulecomp.h"
# 27 "kind.ag"

# 28 "kind.ag"
#include "expiretools.h"
# 29 "kind.ag"

# 30 "kind.ag"
/*AppGen
# 31 "kind.ag"
  %%  Beschreibung des Programmes:
# 32 "kind.ag"
  prog: archiving backup
# 33 "kind.ag"
  %% Beschreibung Parameter
# 34 "kind.ag"
  % symbolischerName, Art, Typ,   Variablenname, Erklärung, Default-Wert
# 35 "kind.ag"
  para: vault_or_group, required, string, vault, Vault to backup
# 36 "kind.ag"
  %% Beschreibung der Optionen
# 37 "kind.ag"
  % kurz-Option, lang-Option, Typ, Variablenname, Erklärung, Default-Wert
# 38 "kind.ag"
  opt: c, masterconfig, string, masterConfig, Master config file, ""
# 39 "kind.ag"
  opt2: if not given or empty kind looks for
# 40 "kind.ag"
  opt2:   /etc/kind/master.conf
# 41 "kind.ag"
  opt2:   /ffp/etc/kind/master.conf
# 42 "kind.ag"
  opt: f, full, void, fullImage, Force full image == initial backup, false
# 43 "kind.ag"
  opt: B, backup, void, doBackup, Backup, false
# 44 "kind.ag"
  opt: E, expire, void, doExpire, Expire, false
# 45 "kind.ag"
  opt: C, listconfig, void, listConfig, Show configuration, false
# 46 "kind.ag"
  opt: I, listimages, void, listImages, List data of images, false
# 47 "kind.ag"
  opt2:   if none of backup, expire, listconfig and listimages is specified,
# 48 "kind.ag"
  opt2:   backup and expire is assumed.
# 49 "kind.ag"
  opt2:   listconfig and listimages cannot be combined with other actions
# 50 "kind.ag"
  opt: D, dryrun, Void, dryRun, Dry run (no real backup), false
# 51 "kind.ag"
  opt: F, forcebackup, string, forcedBackupSet, Create image for specified backup set, ""
# 52 "kind.ag"
  opt: v, verbose, Void, verbose,  Verbose,  false
# 53 "kind.ag"
  opt: d, debug, Void, debug, Debug output of many data, false
# 54 "kind.ag"
  opt: q, quiet, Void, quiet, Be quiet - no messages, false
# 55 "kind.ag"
  opt: h, help, usage, ignored , This help
# 56 "kind.ag"
AppGen*/
# 57 "kind.ag"

# 58 "kind.ag"
using namespace std;
# 59 "kind.ag"

# 60 "kind.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <string>
#include <string>
bool dryRun = false;
bool verbose = false;
bool debug = false;
bool quiet = false;
/*AppGen:GlobalEnd*/
# 61 "kind.ag"

# 62 "kind.ag"
Strings banks;
# 63 "kind.ag"

# 64 "kind.ag"
typedef pair<long int, long int> Sizes;
# 65 "kind.ag"
map<string, Sizes> sizes;
# 66 "kind.ag"

# 67 "kind.ag"
void readSizes(const string& logSizeFile)
# 68 "kind.ag"
{
# 69 "kind.ag"
  if (fileExists(logSizeFile))
# 70 "kind.ag"
    {
# 71 "kind.ag"
      vector<string> ss;
# 72 "kind.ag"
      file2Strings(logSizeFile, ss);
# 73 "kind.ag"
      for (const string& s : ss)
# 74 "kind.ag"
        {
# 75 "kind.ag"
          unsigned int i = 0;
# 76 "kind.ag"
          string v = getWord(s, i);
# 77 "kind.ag"
          long int s1 = getLongInt(s, i);
# 78 "kind.ag"
          long int s2 = getLongInt(s, i);
# 79 "kind.ag"
          sizes[v] = Sizes(s1, s2);
# 80 "kind.ag"
        }
# 81 "kind.ag"
    }
# 82 "kind.ag"
}
# 83 "kind.ag"

# 84 "kind.ag"
void writeSizes(const string logSizeFile)
# 85 "kind.ag"
{
# 86 "kind.ag"
  if (!logSizeFile.empty())
# 87 "kind.ag"
    {
# 88 "kind.ag"
      Strings st;
# 89 "kind.ag"
      for (auto s : sizes)
# 90 "kind.ag"
        {
# 91 "kind.ag"
          string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 92 "kind.ag"
          st.push_back(h);
# 93 "kind.ag"
        }
# 94 "kind.ag"
      strings2File(st, logSizeFile);
# 95 "kind.ag"
    }
# 96 "kind.ag"
}
# 97 "kind.ag"

# 98 "kind.ag"
void verbosePrint(const string& text)
# 99 "kind.ag"
{
# 100 "kind.ag"
  if (verbose)
# 101 "kind.ag"
    cout << "  " << text << endl;
# 102 "kind.ag"
}
# 103 "kind.ag"

# 104 "kind.ag"
void debugPrint(const string& text)
# 105 "kind.ag"
{
# 106 "kind.ag"
  if (debug)
# 107 "kind.ag"
    cout << "    " << text << endl;
# 108 "kind.ag"
}
# 109 "kind.ag"

# 110 "kind.ag"
void readMasterConfig1(const string& fn, KindConfig& conf)
# 111 "kind.ag"
{
# 112 "kind.ag"
  verbosePrint("reading master config " + fn);
# 113 "kind.ag"
  conf.addFile(fn);
# 114 "kind.ag"
}
# 115 "kind.ag"

# 116 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 117 "kind.ag"
{
# 118 "kind.ag"
  if (!fn.empty())  // master config given by user on commandline
# 119 "kind.ag"
    readMasterConfig1(fn, conf);
# 120 "kind.ag"
  else if (fileExists("/etc/kind/master.conf"))
# 121 "kind.ag"
    readMasterConfig1("/etc/kind/master.conf", conf);
# 122 "kind.ag"
  else if (fileExists("/ffp/etc/kind/master.conf"))
# 123 "kind.ag"
    readMasterConfig1("/ffp/etc/kind/master.conf", conf);
# 124 "kind.ag"
  else
# 125 "kind.ag"
    throw Exception("MasterConfig", "no file");
# 126 "kind.ag"
}
# 127 "kind.ag"

# 128 "kind.ag"
string findVault(const string& v)
# 129 "kind.ag"
{
# 130 "kind.ag"
  bool found = false;
# 131 "kind.ag"
  FileName fn;
# 132 "kind.ag"
  fn.setName(v);
# 133 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 134 "kind.ag"
    {
# 135 "kind.ag"
      fn.setPath(banks[i]);
# 136 "kind.ag"
      if (dirExists(fn.getFileName()))
# 137 "kind.ag"
        found = true;
# 138 "kind.ag"
    }
# 139 "kind.ag"
  if (!found)
# 140 "kind.ag"
    throw Exception("find vault", v + " not found");
# 141 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 142 "kind.ag"
  return fn.getFileName();
# 143 "kind.ag"
}
# 144 "kind.ag"

# 145 "kind.ag"
void readVaultConfig(const string& vault, KindConfig& conf)
# 146 "kind.ag"
{
# 147 "kind.ag"
  string vaultpath = findVault(vault);
# 148 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 149 "kind.ag"
  verbosePrint("reading vault config:");
# 150 "kind.ag"
  verbosePrint("  " + vaultConfigName);
# 151 "kind.ag"
  conf.addFile(vaultConfigName);
# 152 "kind.ag"
}
# 153 "kind.ag"

# 154 "kind.ag"
string getImageName(const KindConfig& conf,
# 155 "kind.ag"
                    const string& vaultPath,
# 156 "kind.ag"
                    const DateTime& imageTime)
# 157 "kind.ag"
{
# 158 "kind.ag"
  bool nonPortable = false;
# 159 "kind.ag"
  string imageName = conf.getString("imageName");
# 160 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < imageName.size(); ++i)
# 161 "kind.ag"
    {
# 162 "kind.ag"
      char c = imageName[i];
# 163 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 164 "kind.ag"
        nonPortable = true;
# 165 "kind.ag"
    }
# 166 "kind.ag"
  if (nonPortable)
# 167 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + imageName);
# 168 "kind.ag"

# 169 "kind.ag"
  if (!imageName.empty())
# 170 "kind.ag"
    imageName += '-';
# 171 "kind.ag"

# 172 "kind.ag"
  string imageFullName =  vaultPath + "/" + imageName ;
# 173 "kind.ag"

# 174 "kind.ag"
  if (conf.getBool("longImageName"))
# 175 "kind.ag"
    imageFullName += imageTime.getString('m');
# 176 "kind.ag"
  else
# 177 "kind.ag"
    imageFullName += imageTime.getString('s');
# 178 "kind.ag"

# 179 "kind.ag"
  return imageFullName;
# 180 "kind.ag"
}
# 181 "kind.ag"

# 182 "kind.ag"
Images findImages(const string& vaultpath, const KindConfig& conf, bool all)
# 183 "kind.ag"
{
# 184 "kind.ag"
  Strings dirs;
# 185 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 186 "kind.ag"
  dirList(vaultpath, dirs);
# 187 "kind.ag"

# 188 "kind.ag"
  Images imageList;
# 189 "kind.ag"
  for (string dir : dirs)
# 190 "kind.ag"
    {
# 191 "kind.ag"
      FileName fn(dir);
# 192 "kind.ag"
      string imgname = conf.getString("imageName");
# 193 "kind.ag"
      if (startsWith(fn.getName(), imgname))
# 194 "kind.ag"
        {
# 195 "kind.ag"
          debugPrint("Checking " + dir);
# 196 "kind.ag"
          Image image(dir);
# 197 "kind.ag"

# 198 "kind.ag"
          if (all || image.valid)
# 199 "kind.ag"
            imageList.push_back(image);
# 200 "kind.ag"
        }
# 201 "kind.ag"
    }
# 202 "kind.ag"
  if (imageList.size() > 1)
# 203 "kind.ag"
    sort(imageList.begin(), imageList.end());
# 204 "kind.ag"
  return imageList;
# 205 "kind.ag"
}
# 206 "kind.ag"

# 207 "kind.ag"
void listImageInfo(const string& vault,
# 208 "kind.ag"
		   KindConfig conf /*Copy!*/ ,
# 209 "kind.ag"
		   const DateTime& imageTime,
# 210 "kind.ag"
		   const string& backupSet)
# 211 "kind.ag"
{
# 212 "kind.ag"
  readVaultConfig(vault, conf);
# 213 "kind.ag"
  string vaultPath = findVault(vault);
# 214 "kind.ag"
  Images imageList = findImages(vaultPath, conf, true);
# 215 "kind.ag"
  for (auto img:imageList)
# 216 "kind.ag"
    {
# 217 "kind.ag"
      cout << img.name << endl;
# 218 "kind.ag"
    }
# 219 "kind.ag"
}
# 220 "kind.ag"

# 221 "kind.ag"
void doBackup(const string& vault,
# 222 "kind.ag"
              const string& imageFullName,
# 223 "kind.ag"
              const string& referenceImage,
# 224 "kind.ag"
              const KindConfig& conf)
# 225 "kind.ag"
{
# 226 "kind.ag"
  // create image path
# 227 "kind.ag"
  if (!dryRun)
# 228 "kind.ag"
    if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 229 "kind.ag"
      throw Exception("Create image", "failed to create " + imageFullName);
# 230 "kind.ag"

# 231 "kind.ag"
  // error message
# 232 "kind.ag"
  // we write an generic error message to mark backup as unsuccessful
# 233 "kind.ag"
  // will be deleted at successful end of rsync
# 234 "kind.ag"
  string errorfile = imageFullName + "/error";
# 235 "kind.ag"
  if (!dryRun)
# 236 "kind.ag"
    {
# 237 "kind.ag"
      ofstream error(errorfile);
# 238 "kind.ag"
      error << "failed" << endl;
# 239 "kind.ag"
      error.close();
# 240 "kind.ag"
    }
# 241 "kind.ag"

# 242 "kind.ag"
  // create source descriptor
# 243 "kind.ag"
  string host;
# 244 "kind.ag"
  if (conf.hasKey("host"))
# 245 "kind.ag"
    host = conf.getString("host");
# 246 "kind.ag"

# 247 "kind.ag"
  string server;
# 248 "kind.ag"
  if (conf.hasKey("server"))
# 249 "kind.ag"
    server = conf.getString("server");
# 250 "kind.ag"

# 251 "kind.ag"
  if (!host.empty() && !server.empty())
# 252 "kind.ag"
    throw Exception("backupVault", "Cannot have host and server");
# 253 "kind.ag"

# 254 "kind.ag"
  if (host.empty() && server.empty())
# 255 "kind.ag"
    throw Exception("backupVault", "No host or server specified");
# 256 "kind.ag"

# 257 "kind.ag"
  // ping host / server
# 258 "kind.ag"
  // ping -c 1 -W 5 -q $HOST
# 259 "kind.ag"
  string pingCommand = conf.getString("ping");
# 260 "kind.ag"
  debugPrint("PingCommand: " + pingCommand);
# 261 "kind.ag"
  if (!pingCommand.empty())
# 262 "kind.ag"
    {
# 263 "kind.ag"
      if (!host.empty())
# 264 "kind.ag"
        replacePlaceHolder(pingCommand, "%host", host);
# 265 "kind.ag"
      else
# 266 "kind.ag"
        replacePlaceHolder(pingCommand, "%host", server);
# 267 "kind.ag"
      int rc = 0;
# 268 "kind.ag"
      Strings pingResult = myPopen(pingCommand, rc, debug);
# 269 "kind.ag"
      if (rc != 0)
# 270 "kind.ag"
        {
# 271 "kind.ag"
          strings2File(pingResult, errorfile);
# 272 "kind.ag"
          throw Exception("Host not available", pingCommand);
# 273 "kind.ag"
        }
# 274 "kind.ag"
    }
# 275 "kind.ag"

# 276 "kind.ag"
  string path = conf.getString("path");
# 277 "kind.ag"
  if (path.empty())
# 278 "kind.ag"
    throw Exception("rsync", "empty source path");
# 279 "kind.ag"
  if (path.back() != '/')
# 280 "kind.ag"
    path += '/';
# 281 "kind.ag"

# 282 "kind.ag"
  string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 283 "kind.ag"
  if (!conf.getBool("ignorePermission"))
# 284 "kind.ag"
    rsyncCmd += "-pgo";
# 285 "kind.ag"
  vector<string> rso = conf.getStrings("rsyncOption");
# 286 "kind.ag"
  for (const string& opt : rso)
# 287 "kind.ag"
    rsyncCmd += opt + " ";
# 288 "kind.ag"

# 289 "kind.ag"
  // excludes
# 290 "kind.ag"
  Strings excluded;
# 291 "kind.ag"

# 292 "kind.ag"
  if (conf.hasKey("exclude"))
# 293 "kind.ag"
    excluded += conf.getStrings("exclude");
# 294 "kind.ag"

# 295 "kind.ag"
  if (!host.empty())  // shell mode
# 296 "kind.ag"
    {
# 297 "kind.ag"
      // cout << "USING SHELLMODE '" << host << "'" << endl;
# 298 "kind.ag"
      string remoteShell = conf.getString("remoteShell");
# 299 "kind.ag"
      string userAtHost = conf.getString("user") + "@" +
# 300 "kind.ag"
                          conf.getString("host");
# 301 "kind.ag"
      string rshCommand = remoteShell;
# 302 "kind.ag"
      if (remoteShell.empty())
# 303 "kind.ag"
        rshCommand = "ssh";
# 304 "kind.ag"

# 305 "kind.ag"
      rshCommand += " " + userAtHost;
# 306 "kind.ag"

# 307 "kind.ag"
      string userExcludeCommand = conf.getString("userExcludeCommand");
# 308 "kind.ag"

# 309 "kind.ag"
      if (!userExcludeCommand.empty())
# 310 "kind.ag"
        {
# 311 "kind.ag"
          replacePlaceHolder(userExcludeCommand, "%path", path);
# 312 "kind.ag"
          string excludeCommand = rshCommand + " " + userExcludeCommand;
# 313 "kind.ag"

# 314 "kind.ag"
          verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 315 "kind.ag"

# 316 "kind.ag"
          int rc;
# 317 "kind.ag"
          Strings excludedFiles = myPopen(excludeCommand, rc, debug);
# 318 "kind.ag"
          if (rc > 0)
# 319 "kind.ag"
            {
# 320 "kind.ag"
              // return Strings should contain error messages
# 321 "kind.ag"
              strings2File(excludedFiles, errorfile);
# 322 "kind.ag"
              throw Exception("Find exludes", "Search for excludes failed");
# 323 "kind.ag"
            }
# 324 "kind.ag"

# 325 "kind.ag"
          for (unsigned int i = 0; i < excludedFiles.size(); ++i)
# 326 "kind.ag"
            {
# 327 "kind.ag"
              FileName fn(excludedFiles[i]);
# 328 "kind.ag"
              excluded.push_back('/' + fn.getPath());
# 329 "kind.ag"
              debugPrint("Excluding: " + excluded.back());
# 330 "kind.ag"
            }
# 331 "kind.ag"
        }
# 332 "kind.ag"

# 333 "kind.ag"
      string userExcludeFile = conf.getString("userExcludeFile");
# 334 "kind.ag"
      if (!userExcludeFile.empty())
# 335 "kind.ag"
        {
# 336 "kind.ag"
          userExcludeFile = path + userExcludeFile;
# 337 "kind.ag"
          string getExcludeFileCommand = rshCommand;
# 338 "kind.ag"
          getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 339 "kind.ag"
          getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 340 "kind.ag"
          // cout << getExcludeFileCommand << endl;
# 341 "kind.ag"
          int rc;
# 342 "kind.ag"
          Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 343 "kind.ag"
          if (rc == 0)
# 344 "kind.ag"
            excluded += excludes2;
# 345 "kind.ag"
        }
# 346 "kind.ag"

# 347 "kind.ag"
      if (!dryRun)
# 348 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 349 "kind.ag"

# 350 "kind.ag"
      // rsync image
# 351 "kind.ag"

# 352 "kind.ag"
      if (!remoteShell.empty())
# 353 "kind.ag"
        rsyncCmd += " -e \'" + remoteShell + "\' ";
# 354 "kind.ag"

# 355 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 356 "kind.ag"
      if (!referenceImage.empty())
# 357 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 358 "kind.ag"
      rsyncCmd += userAtHost + ":" + path + " ";
# 359 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 360 "kind.ag"
    } // shell mode
# 361 "kind.ag"
  else
# 362 "kind.ag"
    {
# 363 "kind.ag"
      // cout << "USING SERVERMODE" << endl;
# 364 "kind.ag"
      // we cannot use find without shell access
# 365 "kind.ag"
      // and do not read an exclude file on client side
# 366 "kind.ag"

# 367 "kind.ag"
      if (!dryRun)
# 368 "kind.ag"
        strings2File(excluded, imageFullName + "/exclude");
# 369 "kind.ag"

# 370 "kind.ag"
      rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 371 "kind.ag"
      if (!referenceImage.empty())
# 372 "kind.ag"
        rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 373 "kind.ag"
      rsyncCmd += conf.getString("server") + "::" + path + " ";
# 374 "kind.ag"
      rsyncCmd += imageFullName + "/tree";
# 375 "kind.ag"
    }
# 376 "kind.ag"

# 377 "kind.ag"
  debugPrint("Action: " + rsyncCmd);
# 378 "kind.ag"

# 379 "kind.ag"
  vector<string> backupResult;
# 380 "kind.ag"
  if (!dryRun)
# 381 "kind.ag"
    {
# 382 "kind.ag"
      verbosePrint("syncing (" + rsyncCmd + ")");
# 383 "kind.ag"
      int rc;
# 384 "kind.ag"
      backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 385 "kind.ag"
      if (rc == 0 ||
# 386 "kind.ag"
          rc == 24 || // "no error" or "vanished source files" (ignored)
# 387 "kind.ag"
          rc == 6144) // workaround for wrong exit code ??!!
# 388 "kind.ag"
        {
# 389 "kind.ag"
          unlink(errorfile.c_str());
# 390 "kind.ag"
          long int st = 0;
# 391 "kind.ag"
          long int sc = 0;
# 392 "kind.ag"
          for (auto bl : backupResult)
# 393 "kind.ag"
            {
# 394 "kind.ag"
              if (startsWith(bl, "Total file size"))
# 395 "kind.ag"
                st = getNumber(bl);
# 396 "kind.ag"
              else if (startsWith(bl, "Total transferred file size"))
# 397 "kind.ag"
                sc = getNumber(bl);
# 398 "kind.ag"
            }
# 399 "kind.ag"
          // sizes[vault] = pair<long int, long int>(st, sc);
# 400 "kind.ag"
          sizes[vault] = Sizes(st, sc);
# 401 "kind.ag"
          //  cout << vault << " " << st << " || " << sc << endl;
# 402 "kind.ag"
        }
# 403 "kind.ag"
      else
# 404 "kind.ag"
        throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 405 "kind.ag"
    }
# 406 "kind.ag"
  else
# 407 "kind.ag"
    cout << "Not executing " << rsyncCmd << endl;
# 408 "kind.ag"
}
# 409 "kind.ag"

# 410 "kind.ag"
void backupVault(const string& vault,
# 411 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 412 "kind.ag"
                 const DateTime& imageTime,
# 413 "kind.ag"
                 bool fullImage,
# 414 "kind.ag"
                 const string& forcedBackupSet)
# 415 "kind.ag"
{
# 416 "kind.ag"
  if (!quiet)
# 417 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 418 "kind.ag"
  try
# 419 "kind.ag"
    {
# 420 "kind.ag"
      readVaultConfig(vault, conf);
# 421 "kind.ag"

# 422 "kind.ag"
      // where to store
# 423 "kind.ag"
      string vaultPath = findVault(vault);
# 424 "kind.ag"

# 425 "kind.ag"
      // image path
# 426 "kind.ag"
      string imageFullName = getImageName(conf, vaultPath, imageTime);
# 427 "kind.ag"

# 428 "kind.ag"
      bool backupNow = true;
# 429 "kind.ag"

# 430 "kind.ag"
      // existing images
# 431 "kind.ag"
      Images validImageList = findImages(vaultPath, conf, false);
# 432 "kind.ag"
      string currentSet = "expire"; // we are not using backupSets
# 433 "kind.ag"

# 434 "kind.ag"
      // check if we are using backup sets
# 435 "kind.ag"

# 436 "kind.ag"
      map<string, int> setIdx;
# 437 "kind.ag"
      vector<SetRule> backupSetRule;
# 438 "kind.ag"
      int setRuleIdx = -1;
# 439 "kind.ag"

# 440 "kind.ag"
      if (conf.hasKey("setRule"))
# 441 "kind.ag"
        {
# 442 "kind.ag"
          readSetRules(conf, setIdx, backupSetRule);
# 443 "kind.ag"
          if (!setIdx.empty())
# 444 "kind.ag"
            {
# 445 "kind.ag"
              if (forcedBackupSet.empty())
# 446 "kind.ag"
                {
# 447 "kind.ag"
                  backupNow = false;
# 448 "kind.ag"

# 449 "kind.ag"
                  // find time for nextBackup for every backupSet
# 450 "kind.ag"
                  // defaults to now == imageTime;
# 451 "kind.ag"
                  vector<DateTime> nextBackup(backupSetRule.size(), imageTime);
# 452 "kind.ag"

# 453 "kind.ag"
                  // find time for next backup
# 454 "kind.ag"

# 455 "kind.ag"
                  for (const Image& image : validImageList)
# 456 "kind.ag"
                    {
# 457 "kind.ag"
                      if (image.series != "expire")
# 458 "kind.ag"
                        {
# 459 "kind.ag"
                          string s = image.series;
# 460 "kind.ag"
                          if (setIdx.count(s) > 0) // rule for set exists?
# 461 "kind.ag"
                            {
# 462 "kind.ag"
                              int rIdx = setIdx[s];
# 463 "kind.ag"
                              // image is valid for this and "lower level" backupSets
# 464 "kind.ag"
                              for (unsigned int i = rIdx; i < backupSetRule.size(); ++i)
# 465 "kind.ag"
                                if (nextBackup[i] < image.time + backupSetRule[i].distance)
# 466 "kind.ag"
                                  nextBackup[i] =  image.time + backupSetRule[i].distance;
# 467 "kind.ag"
                            }
# 468 "kind.ag"
                        }
# 469 "kind.ag"
                    }
# 470 "kind.ag"
                  if (debug)
# 471 "kind.ag"
                    for (unsigned int i = 0; i < backupSetRule.size(); ++i)
# 472 "kind.ag"
                      cout << "       Next backup for " << backupSetRule[i].name << " at " <<  nextBackup[i].getString('h') << endl;
# 473 "kind.ag"

# 474 "kind.ag"
                  // find backupSet that
# 475 "kind.ag"
                  //    - needs backup
# 476 "kind.ag"
                  //    - has longest time to keep
# 477 "kind.ag"
                  // because of ordered list backupSetRule this is the first set, that need
# 478 "kind.ag"

# 479 "kind.ag"
                  currentSet = "";
# 480 "kind.ag"
                  for (unsigned int i = 0; i < backupSetRule.size() && currentSet.empty(); ++i)
# 481 "kind.ag"
                    {
# 482 "kind.ag"
                      string name = backupSetRule[i].name;
# 483 "kind.ag"
                      if (nextBackup[i] <= imageTime + 5) // small offset of 5s for "jitter"
# 484 "kind.ag"
                        {
# 485 "kind.ag"
                          backupNow = true;
# 486 "kind.ag"
                          currentSet = name;
# 487 "kind.ag"
                          setRuleIdx = i;
# 488 "kind.ag"
                        }
# 489 "kind.ag"
                    }
# 490 "kind.ag"
                }
# 491 "kind.ag"
              else
# 492 "kind.ag"
                {
# 493 "kind.ag"
                  if (setIdx.count(forcedBackupSet) > 0)
# 494 "kind.ag"
                    {
# 495 "kind.ag"
                      currentSet = forcedBackupSet;
# 496 "kind.ag"
                      setRuleIdx = setIdx[forcedBackupSet];
# 497 "kind.ag"
                    }
# 498 "kind.ag"
                  else
# 499 "kind.ag"
                    throw Exception("force backup of set " + forcedBackupSet, " set not exists");
# 500 "kind.ag"
                }
# 501 "kind.ag"
            } // if (!setIdx.empty())
# 502 "kind.ag"
        } // (conf.hasKey("setRule"))
# 503 "kind.ag"

# 504 "kind.ag"
      if (backupNow)
# 505 "kind.ag"
        {
# 506 "kind.ag"
          verbosePrint("backup to \"" + imageFullName + "\"");
# 507 "kind.ag"
          if (setRuleIdx >= 0 && !quiet)
# 508 "kind.ag"
            cout << "  backup set is \"" << currentSet << "\"" << endl;
# 509 "kind.ag"
        }
# 510 "kind.ag"
      else if (!quiet)
# 511 "kind.ag"
        cout << "  no backup set needs update" << endl;
# 512 "kind.ag"

# 513 "kind.ag"
      if (backupNow)
# 514 "kind.ag"
        {
# 515 "kind.ag"

# 516 "kind.ag"
          // find reference image
# 517 "kind.ag"
          string referenceImage;
# 518 "kind.ag"
          if (!fullImage)
# 519 "kind.ag"
            {
# 520 "kind.ag"
              if (validImageList.empty())
# 521 "kind.ag"
                throw Exception("backupVault", "no reference image found");
# 522 "kind.ag"
              // last image is newest image
# 523 "kind.ag"
              referenceImage = validImageList.back().name;
# 524 "kind.ag"
            }
# 525 "kind.ag"

# 526 "kind.ag"
          doBackup(vault, imageFullName, referenceImage, conf);
# 527 "kind.ag"

# 528 "kind.ag"
          if (!dryRun)
# 529 "kind.ag"
            {
# 530 "kind.ag"
              string lastPath = vaultPath + "/last";
# 531 "kind.ag"
              struct stat fstat;
# 532 "kind.ag"

# 533 "kind.ag"
              // remove last (dir or symlink)
# 534 "kind.ag"
              if (lstat(lastPath.c_str(), &fstat) == 0) // last exists
# 535 "kind.ag"
                {
# 536 "kind.ag"
                  if (S_ISDIR(fstat.st_mode))
# 537 "kind.ag"
                    removeDir(lastPath);
# 538 "kind.ag"
                  else
# 539 "kind.ag"
                    unlink(lastPath.c_str());
# 540 "kind.ag"
                }
# 541 "kind.ag"

# 542 "kind.ag"
	      string linkType=conf.getString("lastLink");
# 543 "kind.ag"
              if (linkType=="hardLink")
# 544 "kind.ag"
                {
# 545 "kind.ag"
                  int rc;
# 546 "kind.ag"
                  string hardLinkCommand = "cp -al " + imageFullName + " " + lastPath;
# 547 "kind.ag"
                  Strings res = myPopen(hardLinkCommand, rc, debug);
# 548 "kind.ag"
                }
# 549 "kind.ag"
              else if (linkType=="symLink")
# 550 "kind.ag"
                {
# 551 "kind.ag"
                  // set symlink to last image
# 552 "kind.ag"
                  symlink(imageFullName.c_str(), lastPath.c_str());
# 553 "kind.ag"
                }
# 554 "kind.ag"
	      else if (linkType!="null")
# 555 "kind.ag"
		{
# 556 "kind.ag"
		  cerr << "invalid Value in \"lastLink\"" << endl;
# 557 "kind.ag"
		}
# 558 "kind.ag"

# 559 "kind.ag"
              // write expire date to file
# 560 "kind.ag"
              DateTime expireTime;
# 561 "kind.ag"
              string rule;
# 562 "kind.ag"
              if (setRuleIdx < 0) // not backup set based
# 563 "kind.ag"
                expireTime = getExpireDate(imageTime, conf, rule);
# 564 "kind.ag"
              else
# 565 "kind.ag"
                {
# 566 "kind.ag"
                  expireTime = imageTime + backupSetRule[setRuleIdx].keep;
# 567 "kind.ag"
                  rule =  backupSetRule[setRuleIdx].rule;
# 568 "kind.ag"
                }
# 569 "kind.ag"

# 570 "kind.ag"
              ofstream expireFile(imageFullName + "/expires");
# 571 "kind.ag"
              expireFile << currentSet << "-" << expireTime.getString('m') << endl;
# 572 "kind.ag"
              expireFile << rule << endl;
# 573 "kind.ag"
            }
# 574 "kind.ag"
        }
# 575 "kind.ag"
    }
# 576 "kind.ag"
  catch (Exception ex)
# 577 "kind.ag"
    {
# 578 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 579 "kind.ag"
    }
# 580 "kind.ag"
}
# 581 "kind.ag"

# 582 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 583 "kind.ag"
{
# 584 "kind.ag"
  if (!quiet)
# 585 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 586 "kind.ag"

# 587 "kind.ag"
  readVaultConfig(vault, conf);
# 588 "kind.ag"

# 589 "kind.ag"
  string vaultpath = findVault(vault);
# 590 "kind.ag"

# 591 "kind.ag"
  Images imagelist = findImages(vaultpath, conf, true);
# 592 "kind.ag"

# 593 "kind.ag"
  string lastValidImage;
# 594 "kind.ag"
  for (Image image : imagelist)
# 595 "kind.ag"
    {
# 596 "kind.ag"
      if (image.valid)
# 597 "kind.ag"
        lastValidImage = image.name;
# 598 "kind.ag"
    }
# 599 "kind.ag"

# 600 "kind.ag"
  for (Image image : imagelist)
# 601 "kind.ag"
    {
# 602 "kind.ag"
      debugPrint(image.name);
# 603 "kind.ag"

# 604 "kind.ag"
      DateTime imageTime = image.time;
# 605 "kind.ag"

# 606 "kind.ag"
      if (imageTime != now &&          // ignore just created image
# 607 "kind.ag"
          image.name != lastValidImage // ignore last valid image
# 608 "kind.ag"
         )
# 609 "kind.ag"
        {
# 610 "kind.ag"
          DateTime expireTime;
# 611 "kind.ag"
          string expireRule;
# 612 "kind.ag"
          if (!image.valid) // invalid image?
# 613 "kind.ag"
            {
# 614 "kind.ag"
              time_t expPeriod = stot(conf.getString("expireFailedImage"));
# 615 "kind.ag"
              if (expPeriod < 0)
# 616 "kind.ag"
                throw Exception("expireFailedImage", "Time period must be positive");
# 617 "kind.ag"
              expireTime = imageTime + stot(conf.getString("expireFailedImage"));
# 618 "kind.ag"
              expireRule = "invalid image: " + conf.getString("expireFailedImage");
# 619 "kind.ag"
              debugPrint("- invalid image");
# 620 "kind.ag"
            }
# 621 "kind.ag"
          else
# 622 "kind.ag"
            {
# 623 "kind.ag"
              debugPrint("- valid image");
# 624 "kind.ag"
              expireTime = image.expire;
# 625 "kind.ag"
              expireRule = image.expireRule;
# 626 "kind.ag"
            }
# 627 "kind.ag"

# 628 "kind.ag"
          if (debug)
# 629 "kind.ag"
            {
# 630 "kind.ag"
              cout << "    image: " << imageTime.getString('h') << endl;
# 631 "kind.ag"
              cout << "      expire: " << expireTime.getString('h') << " " << expireRule << endl;
# 632 "kind.ag"
              cout << "      now: " << now.getString('h') << endl;
# 633 "kind.ag"
            }
# 634 "kind.ag"

# 635 "kind.ag"
          if (expireTime < now)
# 636 "kind.ag"
            {
# 637 "kind.ag"
              if (!quiet)
# 638 "kind.ag"
                cout << "  removing image " << image.name << endl;
# 639 "kind.ag"
              try
# 640 "kind.ag"
                {
# 641 "kind.ag"
                  if (removeDir(image.name) != 0)
# 642 "kind.ag"
                    cout << "Error removing " <<  image.name << endl;
# 643 "kind.ag"
                }
# 644 "kind.ag"
              catch (Exception ex)
# 645 "kind.ag"
                {
# 646 "kind.ag"
                  cerr << "Exception: " << ex.what() << endl;
# 647 "kind.ag"
                }
# 648 "kind.ag"
            }
# 649 "kind.ag"
        }
# 650 "kind.ag"
      else
# 651 "kind.ag"
        debugPrint("- current image - ignored");
# 652 "kind.ag"
    }
# 653 "kind.ag"
}
# 654 "kind.ag"

# 655 "kind.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - archiving backup" << endl;
  cout << "Aufruf:" << endl;
  cout << ag_programName << " [<options>] vault_or_group " << endl;
  cout << "  vault_or_group - Vault to backup" << endl;
  cout << "Optionen:" << endl;
  cout << "  -c --masterconfig  Master config file (default: \"\")" << endl;
  cout << "                     if not given or empty kind looks for" << endl;
  cout << "                     /etc/kind/master.conf" << endl;
  cout << "                     /ffp/etc/kind/master.conf" << endl;
  cout << "  -f --full          Force full image == initial backup (default: false)" << endl;
  cout << "  -B --backup        Backup (default: false)" << endl;
  cout << "  -E --expire        Expire (default: false)" << endl;
  cout << "  -C --listconfig    Show configuration (default: false)" << endl;
  cout << "  -I --listimages    List data of images (default: false)" << endl;
  cout << "                     if none of backup, expire, listconfig and listimages is specified," << endl;
  cout << "                     backup and expire is assumed." << endl;
  cout << "                     listconfig and listimages cannot be combined with other actions" << endl;
  cout << "  -D --dryrun        Dry run (no real backup) (default: false)" << endl;
  cout << "  -F --forcebackup   Create image for specified backup set (default: \"\")" << endl;
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
string masterConfig = "";
bool fullImage = false;
bool doBackup = false;
bool doExpire = false;
bool listConfig = false;
bool listImages = false;
string forcedBackupSet = "";

string vault = "";
  static struct option ag_long_options[] =
  {
    {"masterconfig", required_argument, 0, 'c' },
    {"full", no_argument, 0, 'f' },
    {"backup", no_argument, 0, 'B' },
    {"expire", no_argument, 0, 'E' },
    {"listconfig", no_argument, 0, 'C' },
    {"listimages", no_argument, 0, 'I' },
    {"dryrun", no_argument, 0, 'D' },
    {"forcebackup", required_argument, 0, 'F' },
    {"verbose", no_argument, 0, 'v' },
    {"debug", no_argument, 0, 'd' },
    {"quiet", no_argument, 0, 'q' },
    {"help", no_argument, 0, 'h' },
    {0,         0,                 0,  0 }
  };
  ag_programName = argv[0];
  int rc;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, ":c:fBECIDF:vdqh", ag_long_options, NULL)) >= 0)
    {
      switch (rc)
        {
        case '?':
          error("Unbekannte Option");
          break;
        case ':':
          error("erwarte Option-Parameter");
          break;
        case 'c':
              masterConfig = optarg;
              break;

        case 'f':
              fullImage = true;
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

        case 'I':
              listImages = true;
              break;

        case 'D':
              dryRun = true;
              break;

        case 'F':
              forcedBackupSet = optarg;
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
          error("Unbekannter Fehler in Optionen");
        }
    }
  if (optind < argc)
    vault = argv[optind++];
  else error("Erwarte Parameter vault_or_group");

/*AppGen:MainEnd*/
# 659 "kind.ag"

# 660 "kind.ag"
  int exitCode = 0;
# 661 "kind.ag"
  string lockFile;
# 662 "kind.ag"
  try
# 663 "kind.ag"
    {
# 664 "kind.ag"
      // handling of parameters and switches
# 665 "kind.ag"
      if (debug)        // debug implies verbose
# 666 "kind.ag"
        verbose = true;
# 667 "kind.ag"

# 668 "kind.ag"
      if (!doBackup && !doExpire && !listConfig && !listImages)
# 669 "kind.ag"
        {
# 670 "kind.ag"
          doBackup = true;
# 671 "kind.ag"
          doExpire = true;
# 672 "kind.ag"
        }
# 673 "kind.ag"

# 674 "kind.ag"
      KindConfig conf;
# 675 "kind.ag"

# 676 "kind.ag"
      // default-values
# 677 "kind.ag"
      conf.add("imageName", "image");
# 678 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 679 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 680 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 681 "kind.ag"
      conf.add("ping", "ping -c 1 -W 5 %host");
# 682 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 683 "kind.ag"
      conf.add("remoteShell", "");
# 684 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 685 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 686 "kind.ag"
      conf.add("userExcludeCommand",
# 687 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 688 "kind.ag"
      conf.add("logSize", "");
# 689 "kind.ag"
      conf.add("lastLink","symLink");
# 690 "kind.ag"

# 691 "kind.ag"
      if (listConfig)
# 692 "kind.ag"
        {
# 693 "kind.ag"
          cout << "builtin config" << endl;
# 694 "kind.ag"
          conf.print(".   ");
# 695 "kind.ag"
        }
# 696 "kind.ag"

# 697 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 698 "kind.ag"

# 699 "kind.ag"
      banks = conf.getStrings("bank");
# 700 "kind.ag"
      if (banks.empty())
# 701 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 702 "kind.ag"

# 703 "kind.ag"
      if (listConfig)
# 704 "kind.ag"
        {
# 705 "kind.ag"
          cout << "global config:" << endl;
# 706 "kind.ag"
          conf.print(".   ");
# 707 "kind.ag"
          readVaultConfig(vault, conf);
# 708 "kind.ag"
          cout << "vault config:" << endl;
# 709 "kind.ag"
          conf.print(".   ");
# 710 "kind.ag"
          exit(0);
# 711 "kind.ag"
        }
# 712 "kind.ag"

# 713 "kind.ag"
      DateTime imageTime = DateTime::now();
# 714 "kind.ag"

# 715 "kind.ag"
      if (listImages)
# 716 "kind.ag"
        {
# 717 "kind.ag"
	  listImageInfo(vault, conf, imageTime, forcedBackupSet);
# 718 "kind.ag"
          exit(0);
# 719 "kind.ag"
        }
# 720 "kind.ag"

# 721 "kind.ag"
      lockFile = conf.getString("lockfile");
# 722 "kind.ag"
      createLock(lockFile);
# 723 "kind.ag"

# 724 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 725 "kind.ag"
      if (!logSizeFile.empty())
# 726 "kind.ag"
        readSizes(logSizeFile);
# 727 "kind.ag"

# 728 "kind.ag"
      vector<string> vaults;
# 729 "kind.ag"
      string groupname = "group_" + vault;
# 730 "kind.ag"
      if (conf.hasKey(groupname))
# 731 "kind.ag"
        vaults = conf.getStrings(groupname);
# 732 "kind.ag"
      else
# 733 "kind.ag"
        vaults.push_back(vault);
# 734 "kind.ag"

# 735 "kind.ag"
      if (doBackup)
# 736 "kind.ag"
        for (string vault : vaults)
# 737 "kind.ag"
          {
# 738 "kind.ag"
            backupVault(vault, conf, imageTime, fullImage, forcedBackupSet);
# 739 "kind.ag"
            writeSizes(logSizeFile);
# 740 "kind.ag"
          }
# 741 "kind.ag"

# 742 "kind.ag"
      if (doExpire)
# 743 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 744 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 745 "kind.ag"

# 746 "kind.ag"
      if (!quiet)
# 747 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 748 "kind.ag"

# 749 "kind.ag"
    }
# 750 "kind.ag"
  catch (const Exception& ex)
# 751 "kind.ag"
    {
# 752 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 753 "kind.ag"
      exitCode = 1;
# 754 "kind.ag"
    }
# 755 "kind.ag"
  catch (const char* msg)
# 756 "kind.ag"
    {
# 757 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 758 "kind.ag"
      exitCode = 1;
# 759 "kind.ag"
    }
# 760 "kind.ag"
  catch (const string& msg)
# 761 "kind.ag"
    {
# 762 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 763 "kind.ag"
      exitCode = 1;
# 764 "kind.ag"
    }
# 765 "kind.ag"
  removeLock(lockFile);
# 766 "kind.ag"
  return exitCode;
# 767 "kind.ag"
}
