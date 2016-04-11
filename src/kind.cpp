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
void writeSizes(const string logSizeFile)
# 79 "kind.ag"
{
# 80 "kind.ag"
  if (!logSizeFile.empty())
# 81 "kind.ag"
    {
# 82 "kind.ag"
      Strings st;
# 83 "kind.ag"
      for (auto s : sizes)
# 84 "kind.ag"
	{
# 85 "kind.ag"
	  string h = s.first + " " + to_string(s.second.first) + " " + to_string(s.second.second);
# 86 "kind.ag"
	  st.push_back(h);
# 87 "kind.ag"
	}
# 88 "kind.ag"
      strings2File(st, logSizeFile);
# 89 "kind.ag"
    }
# 90 "kind.ag"
}
# 91 "kind.ag"

# 92 "kind.ag"
void readMasterConfig1(const string& fn, KindConfig& conf)
# 93 "kind.ag"
{
# 94 "kind.ag"
  verbosePrint("reading master config " + fn);
# 95 "kind.ag"
  conf.addFile(fn);
# 96 "kind.ag"
}
# 97 "kind.ag"

# 98 "kind.ag"
void readMasterConfig(const string& fn, KindConfig& conf)
# 99 "kind.ag"
{
# 100 "kind.ag"
  if (!fn.empty())  // master config given by user on commandline
# 101 "kind.ag"
    readMasterConfig1(fn, conf);
# 102 "kind.ag"
  else if (fileExists("/etc/kind/master.conf"))
# 103 "kind.ag"
    readMasterConfig1("/etc/kind/master.conf", conf);
# 104 "kind.ag"
  else if (fileExists("/ffp/etc/kind/master.conf"))
# 105 "kind.ag"
    readMasterConfig1("/ffp/etc/kind/master.conf", conf);
# 106 "kind.ag"
  else
# 107 "kind.ag"
    throw Exception("MasterConfig", "no file");
# 108 "kind.ag"
}
# 109 "kind.ag"

# 110 "kind.ag"
string findVault(const string& v)
# 111 "kind.ag"
{
# 112 "kind.ag"
  bool found = false;
# 113 "kind.ag"
  FileName fn;
# 114 "kind.ag"
  fn.setName(v);
# 115 "kind.ag"
  for (unsigned int i = 0; !found && i < banks.size(); ++i)
# 116 "kind.ag"
    {
# 117 "kind.ag"
      fn.setPath(banks[i]);
# 118 "kind.ag"
      if (dirExists(fn.getFileName()))
# 119 "kind.ag"
        found = true;
# 120 "kind.ag"
    }
# 121 "kind.ag"
  if (!found)
# 122 "kind.ag"
    throw Exception("find vault", v + " not found");
# 123 "kind.ag"
  verbosePrint("using vault " + fn.getFileName());
# 124 "kind.ag"
  return fn.getFileName();
# 125 "kind.ag"
}
# 126 "kind.ag"

# 127 "kind.ag"
void readVaultConfig(const string& vault, KindConfig& conf)
# 128 "kind.ag"
{
# 129 "kind.ag"
  string vaultpath = findVault(vault);
# 130 "kind.ag"
  const string& vaultConfigName = vaultpath + '/' + conf.getString("vaultConfigName");
# 131 "kind.ag"
  verbosePrint("reading vault config:");
# 132 "kind.ag"
  verbosePrint("  " + vaultConfigName);
# 133 "kind.ag"
  conf.addFile(vaultConfigName);
# 134 "kind.ag"
}
# 135 "kind.ag"

# 136 "kind.ag"
string getImageName(const KindConfig& conf)
# 137 "kind.ag"
{
# 138 "kind.ag"
  bool nonPortable = false;
# 139 "kind.ag"
  string res = conf.getString("imageName");
# 140 "kind.ag"
  for (unsigned int i = 0; !nonPortable && i < res.size(); ++i)
# 141 "kind.ag"
    {
# 142 "kind.ag"
      char c = res[i];
# 143 "kind.ag"
      if (!isalnum(c) && c != '.' && c != '_')
# 144 "kind.ag"
        nonPortable = true;
# 145 "kind.ag"
    }
# 146 "kind.ag"
  if (nonPortable)
# 147 "kind.ag"
    throw Exception("getImageName", "Invalid character in image name " + res);
# 148 "kind.ag"
  return res;
# 149 "kind.ag"
}
# 150 "kind.ag"

# 151 "kind.ag"
bool isValidImage(const string& imageName)
# 152 "kind.ag"
{
# 153 "kind.ag"
  return dirExists(imageName) &&
# 154 "kind.ag"
         !fileExists(imageName + "/error") &&
# 155 "kind.ag"
         dirExists(imageName + "/tree");
# 156 "kind.ag"
}
# 157 "kind.ag"

# 158 "kind.ag"
Strings findValidImages(const string& vaultpath, const KindConfig& conf)
# 159 "kind.ag"
{
# 160 "kind.ag"
  Strings imageList;
# 161 "kind.ag"
  debugPrint("searching images in " + vaultpath);
# 162 "kind.ag"
  dirList(vaultpath, imageList);
# 163 "kind.ag"

# 164 "kind.ag"
  Strings validImageList;
# 165 "kind.ag"
  for (unsigned int i = 0; i < imageList.size(); ++i)
# 166 "kind.ag"
    {
# 167 "kind.ag"
      FileName fn(imageList[i]);
# 168 "kind.ag"
      string imgname = getImageName(conf);
# 169 "kind.ag"
      int len = imgname.length();
# 170 "kind.ag"
      if (fn.getName().substr(0, len) == imgname)
# 171 "kind.ag"
        {
# 172 "kind.ag"
          debugPrint("Checking " + imageList[i]);
# 173 "kind.ag"
          if (isValidImage(imageList[i]))
# 174 "kind.ag"
            validImageList.push_back(imageList[i]);
# 175 "kind.ag"
        }
# 176 "kind.ag"
    }
# 177 "kind.ag"
  if (validImageList.empty())
# 178 "kind.ag"
    throw Exception("Find reference", "No reference found");
# 179 "kind.ag"
  sort(validImageList.begin(), validImageList.end());
# 180 "kind.ag"
  return validImageList;
# 181 "kind.ag"
}
# 182 "kind.ag"

# 183 "kind.ag"
void backupVault(const string& vault,
# 184 "kind.ag"
                 KindConfig conf /*Copy!*/ ,
# 185 "kind.ag"
                 const DateTime& imageTime,
# 186 "kind.ag"
                 bool fullImage)
# 187 "kind.ag"
{
# 188 "kind.ag"
  if (!quiet)
# 189 "kind.ag"
    cout << DateTime::now().getString('h') << ": Backup of vault " << vault << endl;
# 190 "kind.ag"
  try
# 191 "kind.ag"
    {
# 192 "kind.ag"
      sizes[vault].second = 0; // nothing backed up yet
# 193 "kind.ag"

# 194 "kind.ag"
      readVaultConfig(vault, conf);
# 195 "kind.ag"

# 196 "kind.ag"
      // where to store
# 197 "kind.ag"
      string vaultpath = findVault(vault);
# 198 "kind.ag"

# 199 "kind.ag"
      // image path
# 200 "kind.ag"
      string imageName = getImageName(conf);
# 201 "kind.ag"
      if (!imageName.empty())
# 202 "kind.ag"
        imageName += '-';
# 203 "kind.ag"
      string imageFullName =  vaultpath + "/" + imageName ;
# 204 "kind.ag"

# 205 "kind.ag"
      if (conf.getBool("longImageName"))
# 206 "kind.ag"
        imageFullName += imageTime.getString('m');
# 207 "kind.ag"
      else
# 208 "kind.ag"
        imageFullName += imageTime.getString('s');
# 209 "kind.ag"

# 210 "kind.ag"
      verbosePrint("backup to \"" + imageFullName + "\"");
# 211 "kind.ag"

# 212 "kind.ag"
      // find reference image
# 213 "kind.ag"
      string referenceImage;
# 214 "kind.ag"
      if (!fullImage)
# 215 "kind.ag"
        {
# 216 "kind.ag"
          Strings validImageList = findValidImages(vaultpath, conf);
# 217 "kind.ag"
          // last image is newest image
# 218 "kind.ag"
          referenceImage = validImageList.back();
# 219 "kind.ag"
        }
# 220 "kind.ag"

# 221 "kind.ag"
      // create image path
# 222 "kind.ag"
      if (!dryRun)
# 223 "kind.ag"
        if (mkdir(imageFullName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
# 224 "kind.ag"
          throw Exception("Create image", "failed to create " + imageFullName);
# 225 "kind.ag"

# 226 "kind.ag"
      // error message
# 227 "kind.ag"
      // we write an generic error message to mark backup as unsuccessful
# 228 "kind.ag"
      // will be deleted at successful end of rsync
# 229 "kind.ag"
      string errorfile = imageFullName + "/error";
# 230 "kind.ag"
      if (!dryRun)
# 231 "kind.ag"
        {
# 232 "kind.ag"
          ofstream error(errorfile);
# 233 "kind.ag"
          error << "failed" << endl;
# 234 "kind.ag"
          error.close();
# 235 "kind.ag"
        }
# 236 "kind.ag"

# 237 "kind.ag"
      // create source descriptor
# 238 "kind.ag"
      string host;
# 239 "kind.ag"
      if (conf.hasKey("host"))
# 240 "kind.ag"
        host = conf.getString("host");
# 241 "kind.ag"

# 242 "kind.ag"
      string server;
# 243 "kind.ag"
      if (conf.hasKey("server"))
# 244 "kind.ag"
        server = conf.getString("server");
# 245 "kind.ag"

# 246 "kind.ag"
      if (!host.empty() && !server.empty())
# 247 "kind.ag"
        throw Exception("backupVault", "Cannot have host and server");
# 248 "kind.ag"

# 249 "kind.ag"
      string path = conf.getString("path");
# 250 "kind.ag"
      if (path.empty())
# 251 "kind.ag"
        throw Exception("rsync", "empty source path");
# 252 "kind.ag"
      if (path.back() != '/')
# 253 "kind.ag"
        path += '/';
# 254 "kind.ag"

# 255 "kind.ag"
      string rsyncCmd = "rsync -vrltH --delete --stats -D --numeric-ids ";
# 256 "kind.ag"
      if (!conf.getBool("ignorePermission"))
# 257 "kind.ag"
        rsyncCmd += "-pgo";
# 258 "kind.ag"
      vector<string> rso = conf.getStrings("rsyncOption");
# 259 "kind.ag"
      for (const string& opt : rso)
# 260 "kind.ag"
        rsyncCmd += opt + " ";
# 261 "kind.ag"

# 262 "kind.ag"
      // excludes
# 263 "kind.ag"
      Strings excluded;
# 264 "kind.ag"

# 265 "kind.ag"
      if (conf.hasKey("exclude"))
# 266 "kind.ag"
        excluded += conf.getStrings("exclude");
# 267 "kind.ag"

# 268 "kind.ag"
      if (!host.empty())  // shell mode
# 269 "kind.ag"
        {
# 270 "kind.ag"
          // cout << "USING SHELLMODE '" << host << "'" << endl;
# 271 "kind.ag"
          string remoteShell = conf.getString("remoteShell");
# 272 "kind.ag"
          string userAtHost = conf.getString("user") + "@" +
# 273 "kind.ag"
                              conf.getString("host");
# 274 "kind.ag"
          string rshCommand = remoteShell;
# 275 "kind.ag"
          if (remoteShell.empty())
# 276 "kind.ag"
            rshCommand = "ssh";
# 277 "kind.ag"

# 278 "kind.ag"
          rshCommand += " " + userAtHost;
# 279 "kind.ag"

# 280 "kind.ag"
          string userExcludeCommand = conf.getString("userExcludeCommand");
# 281 "kind.ag"

# 282 "kind.ag"
          if (!userExcludeCommand.empty())
# 283 "kind.ag"
            {
# 284 "kind.ag"
              replacePlaceHolder(userExcludeCommand, "%path", path);
# 285 "kind.ag"
              string excludeCommand = rshCommand + " " + userExcludeCommand;
# 286 "kind.ag"

# 287 "kind.ag"
              verbosePrint("searching for exclusions (" + excludeCommand + ")");
# 288 "kind.ag"

# 289 "kind.ag"
              int rc;
# 290 "kind.ag"
              Strings excludedFiles = myPopen(excludeCommand, rc, debug);
# 291 "kind.ag"
              if (rc > 0)
# 292 "kind.ag"
                throw Exception("Find exludes", "Search for excludes failed");
# 293 "kind.ag"

# 294 "kind.ag"
              for (unsigned int i = 0; i < excludedFiles.size(); ++i)
# 295 "kind.ag"
                {
# 296 "kind.ag"
                  FileName fn(excludedFiles[i]);
# 297 "kind.ag"
                  excluded.push_back('/' + fn.getPath());
# 298 "kind.ag"
                  debugPrint("Excluding: " + excluded.back());
# 299 "kind.ag"
                }
# 300 "kind.ag"
            }
# 301 "kind.ag"

# 302 "kind.ag"
          string userExcludeFile = conf.getString("userExcludeFile");
# 303 "kind.ag"
          if (!userExcludeFile.empty())
# 304 "kind.ag"
            {
# 305 "kind.ag"
              userExcludeFile = path + userExcludeFile;
# 306 "kind.ag"
              string getExcludeFileCommand = rshCommand;
# 307 "kind.ag"
              getExcludeFileCommand += " \" if [ -f '" + userExcludeFile + "' ]; then ";
# 308 "kind.ag"
              getExcludeFileCommand += " cat '" + userExcludeFile + "' ; fi \"";
# 309 "kind.ag"
              // cout << getExcludeFileCommand << endl;
# 310 "kind.ag"
              int rc;
# 311 "kind.ag"
              Strings excludes2 = myPopen(getExcludeFileCommand, rc, debug);
# 312 "kind.ag"
              if (rc == 0)
# 313 "kind.ag"
                excluded += excludes2;
# 314 "kind.ag"
            }
# 315 "kind.ag"

# 316 "kind.ag"
          if (!dryRun)
# 317 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 318 "kind.ag"

# 319 "kind.ag"
          // rsync image
# 320 "kind.ag"

# 321 "kind.ag"
          if (!remoteShell.empty())
# 322 "kind.ag"
            rsyncCmd += " -e \'" + remoteShell + "\' ";
# 323 "kind.ag"

# 324 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 325 "kind.ag"
          if (!referenceImage.empty())
# 326 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 327 "kind.ag"
          rsyncCmd += userAtHost + ":" + path + " ";
# 328 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 329 "kind.ag"
        } // shell mode
# 330 "kind.ag"
      else
# 331 "kind.ag"
        {
# 332 "kind.ag"
          // cout << "USING SERVERMODE" << endl;
# 333 "kind.ag"

# 334 "kind.ag"
          if (!dryRun)
# 335 "kind.ag"
            strings2File(excluded, imageFullName + "/exclude");
# 336 "kind.ag"

# 337 "kind.ag"
          rsyncCmd += "--exclude-from=" + imageFullName + "/exclude ";
# 338 "kind.ag"
          if (!referenceImage.empty())
# 339 "kind.ag"
            rsyncCmd += "--link-dest=" + referenceImage + "/tree ";
# 340 "kind.ag"
          rsyncCmd += conf.getString("server") + "::" + path + " ";
# 341 "kind.ag"
          rsyncCmd += imageFullName + "/tree";
# 342 "kind.ag"
        }
# 343 "kind.ag"

# 344 "kind.ag"
      debugPrint("Action: " + rsyncCmd);
# 345 "kind.ag"

# 346 "kind.ag"
      vector<string> backupResult;
# 347 "kind.ag"
      if (!dryRun)
# 348 "kind.ag"
        {
# 349 "kind.ag"
          verbosePrint("syncing (" + rsyncCmd + ")");
# 350 "kind.ag"
          int rc;
# 351 "kind.ag"
          backupResult = myPopen(rsyncCmd, rc, debug, imageFullName + "/rsync-log");
# 352 "kind.ag"
          if (rc == 0 ||
# 353 "kind.ag"
              rc == 24 || // "no error" or "vanished source files" (ignored)
# 354 "kind.ag"
              rc == 6144) // workaround for wrong exit code ??!!
# 355 "kind.ag"
            {
# 356 "kind.ag"
              unlink(errorfile.c_str());
# 357 "kind.ag"
              string lastLink = vaultpath + "/last";
# 358 "kind.ag"
              unlink(lastLink.c_str());
# 359 "kind.ag"
              symlink(imageFullName.c_str(), lastLink.c_str());
# 360 "kind.ag"
              long int st = 0;
# 361 "kind.ag"
              long int sc = 0;
# 362 "kind.ag"
              for (auto bl : backupResult)
# 363 "kind.ag"
                {
# 364 "kind.ag"
                  if (bl.substr(0, 15) == "Total file size")
# 365 "kind.ag"
                    st = getNumber(bl);
# 366 "kind.ag"
                  else if (bl.substr(0, 27) == "Total transferred file size")
# 367 "kind.ag"
                    sc = getNumber(bl);
# 368 "kind.ag"
                }
# 369 "kind.ag"
              // sizes[vault] = pair<long int, long int>(st, sc);
# 370 "kind.ag"
              sizes[vault] = Sizes(st, sc);
# 371 "kind.ag"
              //  cout << vault << " " << st << " || " << sc << endl;
# 372 "kind.ag"
            }
# 373 "kind.ag"
          else
# 374 "kind.ag"
            throw Exception("Backup", "Failed to execute rsync (result: " + to_string(rc) + ")");
# 375 "kind.ag"
        }
# 376 "kind.ag"
      else
# 377 "kind.ag"
        cout << "Not executing " << rsyncCmd << endl;
# 378 "kind.ag"

# 379 "kind.ag"
      string rule;
# 380 "kind.ag"
      createExpireFile(imageFullName, conf, rule);
# 381 "kind.ag"
    }
# 382 "kind.ag"
  catch (Exception ex)
# 383 "kind.ag"
    {
# 384 "kind.ag"
      cerr << "Exception in vault " << vault << ": " << ex.what() << endl;
# 385 "kind.ag"
    }
# 386 "kind.ag"
}
# 387 "kind.ag"

# 388 "kind.ag"
void expireVault(const string& vault, KindConfig conf, DateTime now)
# 389 "kind.ag"
{
# 390 "kind.ag"
  if (!quiet)
# 391 "kind.ag"
    cout << DateTime::now().getString('h') << ": Expiring images in vault " << vault << endl;
# 392 "kind.ag"

# 393 "kind.ag"
  readVaultConfig(vault, conf);
# 394 "kind.ag"

# 395 "kind.ag"
  string vaultpath = findVault(vault);
# 396 "kind.ag"

# 397 "kind.ag"
  Strings dirlist; // list of subdirectories
# 398 "kind.ag"
  dirList(vaultpath, dirlist);
# 399 "kind.ag"

# 400 "kind.ag"
  Strings validImages;
# 401 "kind.ag"
  Strings invalidImages;
# 402 "kind.ag"
  string imgname = getImageName(conf);
# 403 "kind.ag"

# 404 "kind.ag"
  for (unsigned int i = 0; i < dirlist.size(); ++i)
# 405 "kind.ag"
    {
# 406 "kind.ag"
      FileName fn(dirlist[i]);
# 407 "kind.ag"
      if (startsWith(fn.getName(), imgname)) // dir is image ?
# 408 "kind.ag"
        {
# 409 "kind.ag"
          debugPrint(dirlist[i]);
# 410 "kind.ag"

# 411 "kind.ag"
          DateTime t = imageDate(dirlist[i]);
# 412 "kind.ag"

# 413 "kind.ag"
          if (t != now) // ignore just created image
# 414 "kind.ag"
            {
# 415 "kind.ag"
              if (!isValidImage(dirlist[i])) // invalid image?
# 416 "kind.ag"
                {
# 417 "kind.ag"
                  invalidImages.push_back(dirlist[i]);
# 418 "kind.ag"
                  debugPrint("- invalid image");
# 419 "kind.ag"
                }
# 420 "kind.ag"
              else
# 421 "kind.ag"
                {
# 422 "kind.ag"
                  validImages.push_back(dirlist[i]);
# 423 "kind.ag"
                  debugPrint("- valid image");
# 424 "kind.ag"
                }
# 425 "kind.ag"
            }
# 426 "kind.ag"
          else
# 427 "kind.ag"
            debugPrint("- current image - ignored");
# 428 "kind.ag"
        }
# 429 "kind.ag"
    }
# 430 "kind.ag"

# 431 "kind.ag"
  for (unsigned int i = 0; i < invalidImages.size(); ++i)
# 432 "kind.ag"
    {
# 433 "kind.ag"
      try
# 434 "kind.ag"
        {
# 435 "kind.ag"
          DateTime t = imageDate(invalidImages[i]);
# 436 "kind.ag"
          DateTime expireTime = t + stot(conf.getString("expireFailedImage"));
# 437 "kind.ag"
          if (debug)
# 438 "kind.ag"
            {
# 439 "kind.ag"
              cout << "image: " << t.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 440 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 441 "kind.ag"
            }
# 442 "kind.ag"
          if (expireTime < now)
# 443 "kind.ag"
            {
# 444 "kind.ag"
              if (!quiet)
# 445 "kind.ag"
                cout << "  removing invalid image " << invalidImages[i] << endl;
# 446 "kind.ag"
              if (removeDir(invalidImages[i]) != 0)
# 447 "kind.ag"
                cout << "Error removing " <<  invalidImages[i] << endl;
# 448 "kind.ag"
            }
# 449 "kind.ag"
        }
# 450 "kind.ag"
      catch (Exception ex)
# 451 "kind.ag"
        {
# 452 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 453 "kind.ag"
        }
# 454 "kind.ag"
    }
# 455 "kind.ag"

# 456 "kind.ag"
  sort(validImages.begin(), validImages.end()); // lexicographical order == temporal order
# 457 "kind.ag"
  for (unsigned int i = 0;
# 458 "kind.ag"
       i < validImages.size() - 1; // never expire latest image
# 459 "kind.ag"
       ++i)
# 460 "kind.ag"
    {
# 461 "kind.ag"
      try
# 462 "kind.ag"
        {
# 463 "kind.ag"
          string imageName = validImages[i];
# 464 "kind.ag"
          DateTime imageTime = imageDate(imageName);
# 465 "kind.ag"
          string rule;
# 466 "kind.ag"
          DateTime expireTime = expireDate(imageName, conf, rule);
# 467 "kind.ag"

# 468 "kind.ag"
          if (debug)
# 469 "kind.ag"
            {
# 470 "kind.ag"
              cout << "image: " << imageTime.getString('h') << "  expire: " << expireTime.getString('h') << endl;
# 471 "kind.ag"
              cout << " now: " << now.getString('h') << endl;
# 472 "kind.ag"
            }
# 473 "kind.ag"
          if (now > expireTime)
# 474 "kind.ag"
            {
# 475 "kind.ag"
              if (!quiet)
# 476 "kind.ag"
                cout << "removing " << imageName << " rule=" << rule << endl;
# 477 "kind.ag"
              removeDir(imageName);
# 478 "kind.ag"
            }
# 479 "kind.ag"
        }
# 480 "kind.ag"
      catch (Exception ex)
# 481 "kind.ag"
        {
# 482 "kind.ag"
          cerr << "Exception: " << ex.what() << endl;
# 483 "kind.ag"
        }
# 484 "kind.ag"
    }
# 485 "kind.ag"
}
# 486 "kind.ag"

# 487 "kind.ag"
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
# 491 "kind.ag"

# 492 "kind.ag"
  int exitCode = 0;
# 493 "kind.ag"
  string lockFile;
# 494 "kind.ag"
  try
# 495 "kind.ag"
    {
# 496 "kind.ag"
      // handling of parameters and switches
# 497 "kind.ag"
      if (debug)        // debug implies verbose
# 498 "kind.ag"
        verbose = true;
# 499 "kind.ag"

# 500 "kind.ag"
      if (!doBackup && !doExpire && !listConfig)
# 501 "kind.ag"
        {
# 502 "kind.ag"
          doBackup = true;
# 503 "kind.ag"
          doExpire = true;
# 504 "kind.ag"
        }
# 505 "kind.ag"

# 506 "kind.ag"
      KindConfig conf;
# 507 "kind.ag"

# 508 "kind.ag"
      // default-values
# 509 "kind.ag"
      conf.add("imageName", "image");
# 510 "kind.ag"
      conf.add("vaultConfigName", "kind/vault.conf");
# 511 "kind.ag"
      conf.add("expireFailedImage", "3 days");
# 512 "kind.ag"
      conf.add("expireRule", "* * * * 1 month");
# 513 "kind.ag"
      conf.add("rsyncOption", ""); // no additional rsync option
# 514 "kind.ag"
      conf.add("remoteShell", "");
# 515 "kind.ag"
      conf.add("lockfile", "/var/lock/kind");
# 516 "kind.ag"
      conf.add("userExcludeFile", "nobackup.list");
# 517 "kind.ag"
      conf.add("userExcludeCommand",
# 518 "kind.ag"
               "find %path -type f -iname '*nobackup' -printf '%P\\\\n'");
# 519 "kind.ag"
      conf.add("logSize", "");
# 520 "kind.ag"

# 521 "kind.ag"
      if (listConfig)
# 522 "kind.ag"
        {
# 523 "kind.ag"
          cout << "builtin config" << endl;
# 524 "kind.ag"
          conf.print(".   ");
# 525 "kind.ag"
        }
# 526 "kind.ag"

# 527 "kind.ag"
      readMasterConfig(masterConfig, conf);
# 528 "kind.ag"

# 529 "kind.ag"
      banks = conf.getStrings("bank");
# 530 "kind.ag"
      if (banks.empty())
# 531 "kind.ag"
        throw Exception("read master configuration", "no banks defined");
# 532 "kind.ag"

# 533 "kind.ag"
      if (listConfig)
# 534 "kind.ag"
        {
# 535 "kind.ag"
          cout << "global config:" << endl;
# 536 "kind.ag"
          conf.print(".   ");
# 537 "kind.ag"
          readVaultConfig(vault, conf);
# 538 "kind.ag"
          cout << "vault config:" << endl;
# 539 "kind.ag"
          conf.print(".   ");
# 540 "kind.ag"
          exit(0);
# 541 "kind.ag"
        }
# 542 "kind.ag"

# 543 "kind.ag"
      lockFile = conf.getString("lockfile");
# 544 "kind.ag"
      createLock(lockFile);
# 545 "kind.ag"

# 546 "kind.ag"
      DateTime imageTime = DateTime::now();
# 547 "kind.ag"
      string logSizeFile = conf.getString("logSize");
# 548 "kind.ag"
      if (!logSizeFile.empty() && fileExists(logSizeFile))
# 549 "kind.ag"
        {
# 550 "kind.ag"
          vector<string> ss;
# 551 "kind.ag"
          file2Strings(logSizeFile, ss);
# 552 "kind.ag"
          for (auto s : ss)
# 553 "kind.ag"
            {
# 554 "kind.ag"
              unsigned int i = 0;
# 555 "kind.ag"
              string v = getWord(s, i);
# 556 "kind.ag"
              long int s1 = getLongInt(s, i);
# 557 "kind.ag"
              long int s2 = getLongInt(s, i);
# 558 "kind.ag"
              sizes[v] = Sizes(s1, s2);
# 559 "kind.ag"
            }
# 560 "kind.ag"
        }
# 561 "kind.ag"

# 562 "kind.ag"
      vector<string> vaults;
# 563 "kind.ag"
      string groupname = "group_" + vault;
# 564 "kind.ag"
      if (conf.hasKey(groupname))
# 565 "kind.ag"
        vaults = conf.getStrings(groupname);
# 566 "kind.ag"
      else
# 567 "kind.ag"
        vaults.push_back(vault);
# 568 "kind.ag"

# 569 "kind.ag"
      if (doBackup)
# 570 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 571 "kind.ag"
          {
# 572 "kind.ag"
            backupVault(vaults[i], conf, imageTime, fullImage);
# 573 "kind.ag"
	    writeSizes(logSizeFile);
# 574 "kind.ag"
          }
# 575 "kind.ag"

# 576 "kind.ag"
      if (doExpire)
# 577 "kind.ag"
        for (unsigned int i = 0; i < vaults.size(); ++i)
# 578 "kind.ag"
          expireVault(vaults[i], conf, imageTime);
# 579 "kind.ag"

# 580 "kind.ag"
      if (!quiet)
# 581 "kind.ag"
        cout << DateTime::now().getString('h') << ": finished" << endl;
# 582 "kind.ag"

# 583 "kind.ag"
    }
# 584 "kind.ag"
  catch (const Exception& ex)
# 585 "kind.ag"
    {
# 586 "kind.ag"
      cerr << "Exception: " << ex.what() << endl;
# 587 "kind.ag"
      exitCode = 1;
# 588 "kind.ag"
    }
# 589 "kind.ag"
  catch (const char* msg)
# 590 "kind.ag"
    {
# 591 "kind.ag"
      cerr << "Exception(char*): " << msg << endl;
# 592 "kind.ag"
      exitCode = 1;
# 593 "kind.ag"
    }
# 594 "kind.ag"
  catch (const string& msg)
# 595 "kind.ag"
    {
# 596 "kind.ag"
      cerr << "Exception(string): " << msg << endl;
# 597 "kind.ag"
      exitCode = 1;
# 598 "kind.ag"
    }
# 599 "kind.ag"
  removeLock(lockFile);
# 600 "kind.ag"
  return exitCode;
# 601 "kind.ag"
}
