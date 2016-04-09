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
  opt: B, backuponly, void, backupOnly, Only backup/no expire, false
# 44 "kind.ag"
  opt: E, expireonly, void, expireOnly, Only expire/no backup, false
# 45 "kind.ag"
  opt: D, dryrun, Void, dryRun, Dry run (no real backup), false
# 46 "kind.ag"
  opt: C, listconfig, void, listConfig, Show configuration, false
# 47 "kind.ag"
  opt: v, verbose, Void, verbose,  Verbose,  false
# 48 "kind.ag"
  opt: d, debug, Void, debug, Debug output of many data, false
# 49 "kind.ag"
  opt: q, quiet, Void, quiet, Be quiet - no messages, false
# 50 "kind.ag"
  opt: h, help, usage, ignored , This help
# 51 "kind.ag"
AppGen*/
# 52 "kind.ag"

# 53 "kind.ag"
using namespace std;
# 54 "kind.ag"

# 55 "kind.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <string>
bool dryRun = false;
bool verbose = false;
bool debug = false;
bool quiet = false;
/*AppGen:GlobalEnd*/
# 56 "kind.ag"

# 57 "kind.ag"
void createExpireFile(const string& image, const KindConfig& conf, string& rule);
# 58 "kind.ag"

# 59 "kind.ag"
Strings banks;
# 60 "kind.ag"

# 61 "kind.ag"
typedef pair<long int, long int> Sizes;
# 62 "kind.ag"
map<string, Sizes> sizes;
# 63 "kind.ag"

# 64 "kind.ag"
void verbosePrint(const string& text)
# 65 "kind.ag"
{
# 66 "kind.ag"
  if (verbose)
# 67 "kind.ag"
    cout << "  " << text << endl;
# 68 "kind.ag"
}
# 69 "kind.ag"

# 70 "kind.ag"
void debugPrint(const string& text)
# 71 "kind.ag"
{
# 72 "kind.ag"
  if (verbose)
# 73 "kind.ag"
    cout << "    " << text << endl;
# 74 "kind.ag"
}
# 75 "kind.ag"

# 76 "kind.ag"
void readMasterConfig1(const string& fn, KindConfig& conf)
# 77 "kind.ag"
{
# 78 "kind.ag"
  verbosePrint("reading master config " + fn);
# 79 "kind.ag"
  conf.addFile(fn);
# 80 "kind.ag"
}
# 81 "kind.ag"

# 82 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 83 "kind.ag"
{
# 84 "kind.ag"
  if (!fn.empty())  // master config given by user on commandline
# 85 "kind.ag"
    readMasterConfig1(fn, conf);
# 86 "kind.ag"
  else if (fileExists("/etc/kind/master.conf"))
# 87 "kind.ag"
    readMasterConfig1("/etc/kind/master.conf", conf);
# 88 "kind.ag"
  else if (fileExists("/ffp/etc/kind/master.conf"))
# 89 "kind.ag"
    readMasterConfig1("/ffp/etc/kind/master.conf", conf);
# 90 "kind.ag"
  else
# 91 "kind.ag"
    throw Exception("MasterConfig", "no file");
# 92 "kind.ag"
}
# 93 "kind.ag"

# 94 "kind.ag"
string findVault(const string& v)
# 95 "kind.ag"
{
# 96 "kind.ag"
  bool found = false;
# 97 "kind.ag"
  FileName fn;
# 98 "kind.ag"
  fn.setName(v);
# 99 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 100 "kind.ag"
    {
# 101 "kind.ag"
      fn.setPath(banks[i]);
# 102 "kind.ag"
      if (dirExists(fn.getFileName()))
# 103 "kind.ag"
        found = true;
# 104 "kind.ag"
    }
# 105 "kind.ag"
  if (!found)
# 106 "kind.ag"
    throw Exception("find vault", v + " not found");
# 107 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 108 "kind.ag"
  return fn.getFileName();
# 109 "kind.ag"
}
# 110 "kind.ag"

# 111 "kind.ag"
void readVaultConfig(const string& vault, KindConfig& conf)
# 112 "kind.ag"
{
# 113 "kind.ag"
  string vaultpath = findVault(vault);
# 114 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 115 "kind.ag"
  verbosePrint("reading vault config:");
# 116 "kind.ag"
  verbosePrint("  " + vaultConfigName);
# 117 "kind.ag"
  conf.addFile(vaultConfigName);
# 118 "kind.ag"
}
# 119 "kind.ag"

# 120 "kind.ag"
string getImageName(const KindConfig& conf)
# 121 "kind.ag"
{
# 122 "kind.ag"
  bool nonPortable = false;
# 123 "kind.ag"
  string res = conf.getString("imageName");
# 124 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < res.size(); ++i)
# 125 "kind.ag"
    {
# 126 "kind.ag"
      char c = res[i];
# 127 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 128 "kind.ag"
        nonPortable = true;
# 129 "kind.ag"
    }
# 130 "kind.ag"
  if (nonPortable)
# 131 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + res);
# 132 "kind.ag"
  return res;
# 133 "kind.ag"
}
# 134 "kind.ag"

# 135 "kind.ag"
bool isValidImage(const string& imageName)
# 136 "kind.ag"
{
# 137 "kind.ag"
  return dirExists(imageName) &&
# 138 "kind.ag"
         !fileExists(imageName + "/error") &&
# 139 "kind.ag"
         dirExists(imageName + "/tree");
# 140 "kind.ag"
}
# 141 "kind.ag"

# 142 "kind.ag"
Strings findValidImages(const string& vaultpath, const KindConfig& conf)
# 143 "kind.ag"
{
# 144 "kind.ag"
  Strings imageList;
# 145 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 146 "kind.ag"
  dirList(vaultpath, imageList);
# 147 "kind.ag"

# 148 "kind.ag"
  Strings validImageList;
# 149 "kind.ag"
  for (unsigned int i = 0; i < imageList.size(); ++i)
# 150 "kind.ag"
    {
# 151 "kind.ag"
      FileName fn(imageList[i]);
# 152 "kind.ag"
      string imgname = getImageName(conf);
# 153 "kind.ag"
      int len = imgname.length();
# 154 "kind.ag"
      if (fn.getName().substr(0, len) == imgname)
# 155 "kind.ag"
        {
# 156 "kind.ag"
          debugPrint("Checking " + imageList[i]);
# 157 "kind.ag"
          if (isValidImage(imageList[i]))
# 158 "kind.ag"
            validImageList.push_back(imageList[i]);
# 159 "kind.ag"
        }
# 160 "kind.ag"
    }
# 161 "kind.ag"
  if (validImageList.empty())
# 162 "kind.ag"
    throw Exception("Find reference", "No reference found");
# 163 "kind.ag"
  sort(validImageList.begin(), validImageList.end());
# 164 "kind.ag"
  return validImageList;
# 165 "kind.ag"
}
# 166 "kind.ag"

# 167 "kind.ag"
void backupVault(const string& vault,
# 168 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 169 "kind.ag"
                 const DateTime& imageTime,
# 170 "kind.ag"
                 bool fullImage)
# 171 "kind.ag"
{
# 172 "kind.ag"
  if (!quiet)
# 173 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 174 "kind.ag"
  try
# 175 "kind.ag"
    {
# 176 "kind.ag"
      sizes[vault].second = 0; // nothing backed up yet
# 177 "kind.ag"

# 178 "kind.ag"
      readVaultConfig(vault, conf);
# 179 "kind.ag"

# 180 "kind.ag"
      string vaultpath = findVault(vault);
# 181 "kind.ag"

# 182 "kind.ag"
      string imageName = getImageName(conf);
# 183 "kind.ag"
      if (!imageName.empty())
# 184 "kind.ag"
        imageName += '-';
# 185 "kind.ag"
      string imageFullName =  vaultpath + "/" + imageName ;
# 186 "kind.ag"

# 187 "kind.ag"
      if (conf.getBool("longImageName"))
# 188 "kind.ag"
        imageFullName += imageTime.getString('m');
# 189 "kind.ag"
      else
# 190 "kind.ag"
        imageFullName += imageTime.getString('s');
# 191 "kind.ag"

# 192 "kind.ag"
      verbosePrint("backup to \"" + imageFullName + "\"");
# 193 "kind.ag"

# 194 "kind.ag"
      // find reference image
# 195 "kind.ag"
      string referenceImage;
# 196 "kind.ag"
      if (!fullImage)
# 197 "kind.ag"
        {
# 198 "kind.ag"
          Strings validImageList = findValidImages(vaultpath, conf);
# 199 "kind.ag"
          // last image is newest image
# 200 "kind.ag"
          referenceImage = validImageList.back();
# 201 "kind.ag"
        }
# 202 "kind.ag"

# 203 "kind.ag"
      // create image path
# 204 "kind.ag"
      if (!dryRun)
# 205 "kind.ag"
        if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 206 "kind.ag"
          throw Exception("Create image", "failed to create " + imageFullName);
# 207 "kind.ag"

# 208 "kind.ag"
      // error message
# 209 "kind.ag"
      // we write an generic error message to mark backup as unsuccessful
# 210 "kind.ag"
      // will be deleted at successful end of rsync
# 211 "kind.ag"
      string errorfile = imageFullName + "/error";
# 212 "kind.ag"
      if (!dryRun)
# 213 "kind.ag"
        {
# 214 "kind.ag"
          ofstream error(errorfile);
# 215 "kind.ag"
          error << "failed" << endl;
# 216 "kind.ag"
          error.close();
# 217 "kind.ag"
        }
# 218 "kind.ag"

# 219 "kind.ag"
      string host;
# 220 "kind.ag"
      if (conf.hasKey("host"))
# 221 "kind.ag"
        host = conf.getString("host");
# 222 "kind.ag"

# 223 "kind.ag"
      string server;
# 224 "kind.ag"
      if (conf.hasKey("server"))
# 225 "kind.ag"
        server = conf.getString("server");
# 226 "kind.ag"

# 227 "kind.ag"
      if (!host.empty() && !server.empty())
# 228 "kind.ag"
        throw Exception("backupVault", "Cannot have host and server");
# 229 "kind.ag"

# 230 "kind.ag"
      string path = conf.getString("path");
# 231 "kind.ag"
      if (path.empty())
# 232 "kind.ag"
        throw Exception("rsync", "empty source path");
# 233 "kind.ag"
      if (path.back() != '/')
# 234 "kind.ag"
        path += '/';
# 235 "kind.ag"

# 236 "kind.ag"
      string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 237 "kind.ag"
      if (!conf.getBool("ignorePermission"))
# 238 "kind.ag"
        rsyncCmd += "-pgo";
# 239 "kind.ag"
      vector<string> rso = conf.getStrings("rsyncOption");
# 240 "kind.ag"
      for (string opt : rso)
# 241 "kind.ag"
        rsyncCmd += opt + " ";
# 242 "kind.ag"

# 243 "kind.ag"
      if (!host.empty())  // shell mode
# 244 "kind.ag"
        {
# 245 "kind.ag"
          // cout << "USING SHELLMODE '" << host << "'" << endl;
# 246 "kind.ag"
          string remoteShell = conf.getString("remoteShell");
# 247 "kind.ag"
          string userAtHost = conf.getString("user") + "@" +
# 248 "kind.ag"
                              conf.getString("host");
# 249 "kind.ag"
          string rshCommand = remoteShell;
# 250 "kind.ag"
          if (remoteShell.empty())
# 251 "kind.ag"
            rshCommand = "ssh";
# 252 "kind.ag"

# 253 "kind.ag"
          rshCommand += " " + userAtHost;
# 254 "kind.ag"

# 255 "kind.ag"
          // excludes
# 256 "kind.ag"
          Strings excluded;
# 257 "kind.ag"

# 258 "kind.ag"
          string userExcludeCommand = conf.getString("userExcludeCommand");
# 259 "kind.ag"

# 260 "kind.ag"
          if (!userExcludeCommand.empty())
# 261 "kind.ag"
            {
# 262 "kind.ag"
              replacePlaceHolder(userExcludeCommand, "%path", path);
# 263 "kind.ag"
              string excludeCommand = rshCommand + " " + userExcludeCommand;
# 264 "kind.ag"

# 265 "kind.ag"
              verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 266 "kind.ag"

# 267 "kind.ag"
              int rc;
# 268 "kind.ag"
              excluded = myPopen(excludeCommand, rc, debug);
# 269 "kind.ag"
              if (rc > 0)
# 270 "kind.ag"
                throw Exception("Find exludes", "Search for excludes failed");
# 271 "kind.ag"

# 272 "kind.ag"
              for (unsigned int i = 0; i < excluded.size(); ++i)
# 273 "kind.ag"
                {
# 274 "kind.ag"
                  FileName fn(excluded[i]);
# 275 "kind.ag"
                  excluded[i] = '/' + fn.getPath();
# 276 "kind.ag"
                  debugPrint("Excluding: " + excluded[i]);
# 277 "kind.ag"
                }
# 278 "kind.ag"
            }
# 279 "kind.ag"

# 280 "kind.ag"
          string userExcludeFile = conf.getString("userExcludeFile");
# 281 "kind.ag"
          if (!userExcludeFile.empty())
# 282 "kind.ag"
            {
# 283 "kind.ag"
              userExcludeFile = path + userExcludeFile;
# 284 "kind.ag"
              string getExcludeFileCommand = rshCommand;
# 285 "kind.ag"
              getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 286 "kind.ag"
              getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 287 "kind.ag"
              // cout << getExcludeFileCommand << endl;
# 288 "kind.ag"
              int rc;
# 289 "kind.ag"
              Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 290 "kind.ag"
              if (rc == 0)
# 291 "kind.ag"
                excluded += excludes2;
# 292 "kind.ag"
            }
# 293 "kind.ag"

# 294 "kind.ag"
          if (conf.hasKey("exclude"))
# 295 "kind.ag"
            excluded += conf.getStrings("exclude");
# 296 "kind.ag"

# 297 "kind.ag"
          if (!dryRun)
# 298 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 299 "kind.ag"

# 300 "kind.ag"
          // rsync image
# 301 "kind.ag"

# 302 "kind.ag"
          if (!remoteShell.empty())
# 303 "kind.ag"
            rsyncCmd += " -e \'" + remoteShell + "\' ";
# 304 "kind.ag"

# 305 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 306 "kind.ag"
          if (!referenceImage.empty())
# 307 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 308 "kind.ag"
          rsyncCmd += userAtHost + ":" + path + " ";
# 309 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 310 "kind.ag"
        } // shell mode
# 311 "kind.ag"
      else
# 312 "kind.ag"
        {
# 313 "kind.ag"
          // cout << "USING SERVERMODE" << endl;
# 314 "kind.ag"
          vector<string> excluded;
# 315 "kind.ag"
          if (conf.hasKey("exclude"))
# 316 "kind.ag"
            {
# 317 "kind.ag"
              Strings excludes = conf.getStrings("exclude");
# 318 "kind.ag"
              for (string s : excludes)
# 319 "kind.ag"
                excluded.push_back(s);
# 320 "kind.ag"
            }
# 321 "kind.ag"

# 322 "kind.ag"
          if (!dryRun)
# 323 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 324 "kind.ag"

# 325 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 326 "kind.ag"
          if (!referenceImage.empty())
# 327 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 328 "kind.ag"
          rsyncCmd += conf.getString("server") + "::" + path + " ";
# 329 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 330 "kind.ag"
        }
# 331 "kind.ag"

# 332 "kind.ag"
      debugPrint("Action: " + rsyncCmd);
# 333 "kind.ag"

# 334 "kind.ag"
      vector<string> backupResult;
# 335 "kind.ag"
      if (!dryRun)
# 336 "kind.ag"
        {
# 337 "kind.ag"
          verbosePrint("syncing (" + rsyncCmd + ")");
# 338 "kind.ag"
          int rc;
# 339 "kind.ag"
          backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 340 "kind.ag"
          // strings2File(backupResult, imageFullName + "/rsync-log");
# 341 "kind.ag"
          if (rc == 0 ||
# 342 "kind.ag"
              rc == 24 || // "no error" or "vanished source files" (ignored)
# 343 "kind.ag"
              rc == 6114) // workaround for wrong exit code ??!!
# 344 "kind.ag"
            {
# 345 "kind.ag"
              unlink(errorfile.c_str());
# 346 "kind.ag"
              string lastLink = vaultpath + "/last";
# 347 "kind.ag"
              unlink(lastLink.c_str());
# 348 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 349 "kind.ag"
              long int st = 0;
# 350 "kind.ag"
              long int sc = 0;
# 351 "kind.ag"
              for (auto bl : backupResult)
# 352 "kind.ag"
                {
# 353 "kind.ag"
                  if (bl.substr(0, 15) == "Total file size")
# 354 "kind.ag"
                    st = getNumber(bl);
# 355 "kind.ag"
                  else if (bl.substr(0, 27) == "Total transferred file size")
# 356 "kind.ag"
                    sc = getNumber(bl);
# 357 "kind.ag"
                }
# 358 "kind.ag"
              // sizes[vault] = pair<long int, long int>(st, sc);
# 359 "kind.ag"
              sizes[vault] = Sizes(st, sc);
# 360 "kind.ag"
              //  cout << vault << " " << st << " || " << sc << endl;
# 361 "kind.ag"
            }
# 362 "kind.ag"
          else
# 363 "kind.ag"
            throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 364 "kind.ag"
        }
# 365 "kind.ag"
      else
# 366 "kind.ag"
        cout << "Not executing " << rsyncCmd << endl;
# 367 "kind.ag"

# 368 "kind.ag"
      string rule;
# 369 "kind.ag"
      createExpireFile(imageFullName, conf, rule);
# 370 "kind.ag"
    }
# 371 "kind.ag"
  catch (Exception ex)
# 372 "kind.ag"
    {
# 373 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 374 "kind.ag"
    }
# 375 "kind.ag"
}
# 376 "kind.ag"

# 377 "kind.ag"
int removeDir(const string& path)
# 378 "kind.ag"
{
# 379 "kind.ag"
  debugPrint("removeDir " + path);
# 380 "kind.ag"
  DIR* d = opendir(path.c_str());
# 381 "kind.ag"

# 382 "kind.ag"
  int r = -1;
# 383 "kind.ag"
  if (d)
# 384 "kind.ag"
    {
# 385 "kind.ag"
      struct dirent* p;
# 386 "kind.ag"

# 387 "kind.ag"
      r = 0;
# 388 "kind.ag"
      while (!r && (p = readdir(d)))
# 389 "kind.ag"
        {
# 390 "kind.ag"
          int r2 = 0;
# 391 "kind.ag"
          string fn = p->d_name;
# 392 "kind.ag"
          if (fn != "." && fn != "..")
# 393 "kind.ag"
            {
# 394 "kind.ag"
              fn = path + "/" + fn;
# 395 "kind.ag"
              debugPrint("-- " + fn);
# 396 "kind.ag"
              struct stat statbuf;
# 397 "kind.ag"
              if (lstat(fn.c_str(), &statbuf) == 0)
# 398 "kind.ag"
                {
# 399 "kind.ag"
                  if (S_ISLNK(statbuf.st_mode))
# 400 "kind.ag"
                    {
# 401 "kind.ag"
                      debugPrint("Remove link " + fn);
# 402 "kind.ag"
                      r2 = unlink(fn.c_str());
# 403 "kind.ag"
                    }
# 404 "kind.ag"
                  else if (S_ISDIR(statbuf.st_mode))
# 405 "kind.ag"
                    {
# 406 "kind.ag"
                      debugPrint("Remove dir " + fn);
# 407 "kind.ag"
                      r2 = removeDir(fn);
# 408 "kind.ag"
                    }
# 409 "kind.ag"
                  else
# 410 "kind.ag"
                    {
# 411 "kind.ag"
                      debugPrint("Remove file " + fn);
# 412 "kind.ag"
                      r2 = unlink(fn.c_str());
# 413 "kind.ag"
                    }
# 414 "kind.ag"
                }
# 415 "kind.ag"
              else
# 416 "kind.ag"
                {
# 417 "kind.ag"
                  cout << "stat(" << fn << ") failed" << endl;
# 418 "kind.ag"
                  // we assume "file" here
# 419 "kind.ag"
                  r2 = unlink(fn.c_str());
# 420 "kind.ag"
                }
# 421 "kind.ag"
            }
# 422 "kind.ag"
          r = r2;
# 423 "kind.ag"
        }
# 424 "kind.ag"
      closedir(d);
# 425 "kind.ag"
    }
# 426 "kind.ag"

# 427 "kind.ag"
  if (r == 0)
# 428 "kind.ag"
    {
# 429 "kind.ag"
      debugPrint("Remove Dir itself " + path);
# 430 "kind.ag"
      r = rmdir(path.c_str());
# 431 "kind.ag"
    }
# 432 "kind.ag"

# 433 "kind.ag"
  return r;
# 434 "kind.ag"
}
# 435 "kind.ag"

# 436 "kind.ag"
#if 0
# 437 "kind.ag"
int removeDir(const string& dname)
# 438 "kind.ag"
{
# 439 "kind.ag"
  int rc = 0;
# 440 "kind.ag"
  if (!dryRun)
# 441 "kind.ag"
    {
# 442 "kind.ag"
      Strings files;
# 443 "kind.ag"
      // subdirectories
# 444 "kind.ag"
      dirList(dname, files);
# 445 "kind.ag"
      for (unsigned int i = 0; i < files.size(); ++i)
# 446 "kind.ag"
        {
# 447 "kind.ag"
          debugPrint("Remove dir " + files[i]);
# 448 "kind.ag"
          for (unsigned int i = 0; i < files.size(); ++i)
# 449 "kind.ag"
            rc += removeDir(files[i]);
# 450 "kind.ag"
        }
# 451 "kind.ag"
      files.clear();
# 452 "kind.ag"

# 453 "kind.ag"
      // files in directory
# 454 "kind.ag"
      fileList(dname, files);
# 455 "kind.ag"
      for (unsigned int i = 0; i < files.size(); ++i)
# 456 "kind.ag"
        {
# 457 "kind.ag"
          debugPrint("unlink " + files[i]);
# 458 "kind.ag"
          if (!dryRun)
# 459 "kind.ag"
            {
# 460 "kind.ag"
              if (unlink(files[i].c_str()) != 0)
# 461 "kind.ag"
                rc++;
# 462 "kind.ag"
            }
# 463 "kind.ag"
        }
# 464 "kind.ag"
      debugPrint("rmdir " + dname);
# 465 "kind.ag"

# 466 "kind.ag"
      // directory
# 467 "kind.ag"
      if (rmdir(dname.c_str()) != 0)
# 468 "kind.ag"
        rc++;
# 469 "kind.ag"
    }
# 470 "kind.ag"

# 471 "kind.ag"
  return rc;
# 472 "kind.ag"
}
# 473 "kind.ag"
#endif
# 474 "kind.ag"

# 475 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 476 "kind.ag"
{
# 477 "kind.ag"
  if (!quiet)
# 478 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 479 "kind.ag"

# 480 "kind.ag"
  readVaultConfig(vault, conf);
# 481 "kind.ag"

# 482 "kind.ag"
  string vaultpath = findVault(vault);
# 483 "kind.ag"

# 484 "kind.ag"
  Strings dirlist; // list of subdirectories
# 485 "kind.ag"
  dirList(vaultpath, dirlist);
# 486 "kind.ag"

# 487 "kind.ag"
  Strings validImages;
# 488 "kind.ag"
  Strings invalidImages;
# 489 "kind.ag"
  string imgname = getImageName(conf);
# 490 "kind.ag"

# 491 "kind.ag"
  for (unsigned int i = 0; i < dirlist.size(); ++i)
# 492 "kind.ag"
    {
# 493 "kind.ag"
      FileName fn(dirlist[i]);
# 494 "kind.ag"
      if (startsWith(fn.getName(), imgname)) // dir is image ?
# 495 "kind.ag"
        {
# 496 "kind.ag"
          debugPrint(dirlist[i]);
# 497 "kind.ag"

# 498 "kind.ag"
          DateTime t = imageDate(dirlist[i]);
# 499 "kind.ag"

# 500 "kind.ag"
          if (t != now) // ignore just created image
# 501 "kind.ag"
            {
# 502 "kind.ag"
              if (!isValidImage(dirlist[i])) // invalid image?
# 503 "kind.ag"
                {
# 504 "kind.ag"
                  invalidImages.push_back(dirlist[i]);
# 505 "kind.ag"
                  debugPrint("- invalid image");
# 506 "kind.ag"
                }
# 507 "kind.ag"
              else
# 508 "kind.ag"
                {
# 509 "kind.ag"
                  validImages.push_back(dirlist[i]);
# 510 "kind.ag"
                  debugPrint("- valid image");
# 511 "kind.ag"
                }
# 512 "kind.ag"
            }
# 513 "kind.ag"
          else
# 514 "kind.ag"
            debugPrint("- current image - ignored");
# 515 "kind.ag"
        }
# 516 "kind.ag"
    }
# 517 "kind.ag"

# 518 "kind.ag"
  for (unsigned int i = 0; i < invalidImages.size(); ++i)
# 519 "kind.ag"
    {
# 520 "kind.ag"
      try
# 521 "kind.ag"
        {
# 522 "kind.ag"
          DateTime t = imageDate(invalidImages[i]);
# 523 "kind.ag"
          DateTime expireTime = t + stot(conf.getString("expireFailedImage"));
# 524 "kind.ag"
          if (debug)
# 525 "kind.ag"
            {
# 526 "kind.ag"
              cout << "image: " << t.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 527 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 528 "kind.ag"
            }
# 529 "kind.ag"
          if (expireTime < now)
# 530 "kind.ag"
            {
# 531 "kind.ag"
              if (!quiet)
# 532 "kind.ag"
                cout << "  removing invalid image " << invalidImages[i] << endl;
# 533 "kind.ag"
              if (removeDir(invalidImages[i]) != 0)
# 534 "kind.ag"
                cout << "Error removing " <<  invalidImages[i] << endl;
# 535 "kind.ag"
            }
# 536 "kind.ag"
        }
# 537 "kind.ag"
      catch (Exception ex)
# 538 "kind.ag"
        {
# 539 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 540 "kind.ag"
        }
# 541 "kind.ag"
    }
# 542 "kind.ag"

# 543 "kind.ag"
  sort(validImages.begin(), validImages.end()); // lexicographical order == temporal order
# 544 "kind.ag"
  for (unsigned int i = 0;
# 545 "kind.ag"
       i < validImages.size() - 1; // never expire latest image
# 546 "kind.ag"
       ++i)
# 547 "kind.ag"
    {
# 548 "kind.ag"
      try
# 549 "kind.ag"
        {
# 550 "kind.ag"
          string imageName = validImages[i];
# 551 "kind.ag"
          DateTime imageTime = imageDate(imageName);
# 552 "kind.ag"
          string rule;
# 553 "kind.ag"
          DateTime expireTime = expireDate(imageName, conf, rule);
# 554 "kind.ag"

# 555 "kind.ag"
          if (debug)
# 556 "kind.ag"
            {
# 557 "kind.ag"
              cout << "image: " << imageTime.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 558 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 559 "kind.ag"
            }
# 560 "kind.ag"
          if (now > expireTime)
# 561 "kind.ag"
            {
# 562 "kind.ag"
              if (!quiet)
# 563 "kind.ag"
                cout << "removing " << imageName << " rule=" << rule << endl;
# 564 "kind.ag"
              removeDir(imageName);
# 565 "kind.ag"
            }
# 566 "kind.ag"
        }
# 567 "kind.ag"
      catch (Exception ex)
# 568 "kind.ag"
        {
# 569 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 570 "kind.ag"
        }
# 571 "kind.ag"
    }
# 572 "kind.ag"
}
# 573 "kind.ag"

# 574 "kind.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - archiving backup" << endl;
  cout << "Aufruf:" << endl;
  cout << ag_programName << " [<options>] vault_or_group " << endl;
  cout << "  vault_or_group - Vault to backup" << endl;
  cout << "Optionen:" << endl;
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
# 578 "kind.ag"

# 579 "kind.ag"
  int exitCode = 0;
# 580 "kind.ag"
  string lockFile;
# 581 "kind.ag"
  try
# 582 "kind.ag"
    {
# 583 "kind.ag"
      if (debug)
# 584 "kind.ag"
        verbose = true;
# 585 "kind.ag"

# 586 "kind.ag"
      KindConfig conf;
# 587 "kind.ag"

# 588 "kind.ag"
      // default-values
# 589 "kind.ag"
      conf.add("imageName", "image");
# 590 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 591 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 592 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 593 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 594 "kind.ag"
      conf.add("remoteShell", "");
# 595 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 596 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 597 "kind.ag"
      conf.add("userExcludeCommand",
# 598 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 599 "kind.ag"
      conf.add("logSize", "");
# 600 "kind.ag"

# 601 "kind.ag"
      if (listConfig)
# 602 "kind.ag"
        {
# 603 "kind.ag"
          cout << "builtin config" << endl;
# 604 "kind.ag"
          conf.print(".   ");
# 605 "kind.ag"
        }
# 606 "kind.ag"

# 607 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 608 "kind.ag"

# 609 "kind.ag"
      banks = conf.getStrings("bank");
# 610 "kind.ag"
      if (banks.empty())
# 611 "kind.ag"
        throw Exception("read master config", "no banks defined");
# 612 "kind.ag"

# 613 "kind.ag"
      if (listConfig)
# 614 "kind.ag"
        {
# 615 "kind.ag"
          cout << "global config:" << endl;
# 616 "kind.ag"
          conf.print(".   ");
# 617 "kind.ag"
          readVaultConfig(vault, conf);
# 618 "kind.ag"
          cout << "vault config:" << endl;
# 619 "kind.ag"
          conf.print(".   ");
# 620 "kind.ag"
          exit(0);
# 621 "kind.ag"
        }
# 622 "kind.ag"

# 623 "kind.ag"
      lockFile = conf.getString("lockfile");
# 624 "kind.ag"
      createLock(lockFile);
# 625 "kind.ag"

# 626 "kind.ag"
      DateTime imageTime = DateTime::now();
# 627 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 628 "kind.ag"
      if (!logSizeFile.empty() && fileExists(logSizeFile))
# 629 "kind.ag"
        {
# 630 "kind.ag"
          vector<string> ss;
# 631 "kind.ag"
          file2Strings(logSizeFile, ss);
# 632 "kind.ag"
          for (auto s : ss)
# 633 "kind.ag"
            {
# 634 "kind.ag"
              unsigned int i = 0;
# 635 "kind.ag"
              string v = getWord(s, i);
# 636 "kind.ag"
              long int s1 = getLongInt(s, i);
# 637 "kind.ag"
              long int s2 = getLongInt(s, i);
# 638 "kind.ag"
              sizes[v] = Sizes(s1, s2);
# 639 "kind.ag"
            }
# 640 "kind.ag"
        }
# 641 "kind.ag"

# 642 "kind.ag"
      vector<string> vaults;
# 643 "kind.ag"
      string groupname = "group_" + vault;
# 644 "kind.ag"
      if (conf.hasKey(groupname))
# 645 "kind.ag"
        vaults = conf.getStrings(groupname);
# 646 "kind.ag"
      else
# 647 "kind.ag"
        vaults.push_back(vault);
# 648 "kind.ag"

# 649 "kind.ag"
      if (!expireOnly)
# 650 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 651 "kind.ag"
          {
# 652 "kind.ag"
            backupVault(vaults[i], conf, imageTime, fullImage);
# 653 "kind.ag"
            if (!logSizeFile.empty())
# 654 "kind.ag"
              {
# 655 "kind.ag"
                Strings st;
# 656 "kind.ag"
                for (auto s : sizes)
# 657 "kind.ag"
                  {
# 658 "kind.ag"
                    string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 659 "kind.ag"
                    st.push_back(h);
# 660 "kind.ag"
                  }
# 661 "kind.ag"
                strings2File(st, logSizeFile);
# 662 "kind.ag"
              }
# 663 "kind.ag"
          }
# 664 "kind.ag"

# 665 "kind.ag"
      if (!backupOnly)
# 666 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 667 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 668 "kind.ag"

# 669 "kind.ag"
      if (!quiet)
# 670 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 671 "kind.ag"

# 672 "kind.ag"
    }
# 673 "kind.ag"
  catch (const Exception& ex)
# 674 "kind.ag"
    {
# 675 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 676 "kind.ag"
      exitCode = 1;
# 677 "kind.ag"
    }
# 678 "kind.ag"
  catch (const char* msg)
# 679 "kind.ag"
    {
# 680 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 681 "kind.ag"
      exitCode = 1;
# 682 "kind.ag"
    }
# 683 "kind.ag"
  catch (const string& msg)
# 684 "kind.ag"
    {
# 685 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 686 "kind.ag"
      exitCode = 1;
# 687 "kind.ag"
    }
# 688 "kind.ag"
  removeLock(lockFile);
# 689 "kind.ag"
  return exitCode;
# 690 "kind.ag"
}
