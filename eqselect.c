/*
 * File:    eqselect.c
 *
 * Author:  fossette
 *
 * Date:    2015/09/13
 *
 * Version: 0.91
 *
 * Descr:   Select a random file in a directory tree and execute it
 *          according to its file type.  If used numerous times, all files
 *          will be executed because eqselect won't pick the same file in
 *          a cycle.  Tested under FreeBSD 10.1.  Should be portable
 *          because only standard libraries are used.
 *
 *          Option:
 *             -c    Continuous selection instead of the default
 *                   random selection.
 *             -l    Consider the current directory as a leaf,
 *                   i.e. sub-directories are ignored.
 *             -r    Repeat the last executed file.
 *             -z    Reset the executed files list
 *
 *          Optional parameter: the directory to use.
 *          If not specified, the current directory is used.
 *
 * Web:     https://github.com/fossette/eqselect/wiki
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>


/*
 *  Constants
 */

#define LNSZ                  500
#define DEFAULTDIRMODE        S_IRWXU|S_IRGRP
#define DEFAULTPLAYER         "vlc"
#define DIRSEP                "/"
#define EQSELECTDIR           ".eqselect"
#define EXECUTEDFILENAMES     "exec.txt"
#define FOPENAPPEND           "a"
#define FOPENREAD             "r"
#define FOPENWRITE            "w"
#define HOMEDIRPATHENV        "HOME"
#define NL                    "\n"
#define VALIDFILETYPES        ".avi .flv .mkv .mov .mp3 .mp4 .mpeg .mpg .ogg .ts .wav .wmv"

#define ERROR_EQSELECT_DIR    1
#define ERROR_EQSELECT_EMPTY  2
#define ERROR_EQSELECT_MEM    3
#define ERROR_EQSELECT_ERRNO  4
#define ERROR_EQSELECT_QUIT   99


/*
 *  Types
 */

typedef char FILENAME[LNSZ];

typedef  struct
         {
            int      max,
                     n;
            FILENAME *array;
         } FILELIST;


/*
 *  Global variable
 */

static int        giOptionContinu = 0,
                  giOptionLeaf = 0,
                  giOptionRepeatLast = 0,
                  giOptionReset = 0;
static FILELIST   gaAvailableFilenames,
                  gaExecutedFilenames;
static FILENAME   gszExecutedFileName;


/*
 *  Prototypes
 */

int   DirnameExist(const char *szDirname);
int   fgetsz(FILE *hFile,     char *szLine);
int   FilelistAdd(const char *szFilename, FILELIST *pFilelist);
int   FilelistExist(const char *szFilename, FILELIST *pFilelist);
void  FilenameSplit(const char *szFilename,     char *szBase, char *szExt);
int   FilenameValid(const char *szFilename);
int   ParseCurrentDirectory(const char *pDirectoryName);
void  ResetExecutedFilenames(void);
void  StrnCopy(char *dst, const char *src, const int l);


/*
 *  DirnameExist
 */

int
DirnameExist(const char *szDirname)
{
   int i;
   struct stat sStat;


   i = !stat(szDirname,     &sStat);
   if (i)
      i = (sStat.st_mode & S_IFDIR);

   return(i);
}


/*
 *  fgetsz
 */

int
fgetsz(FILE *hFile,     char *szLine)
{
   int   i;
   char  *pChar;


   pChar = fgets(szLine, LNSZ-1, hFile);
   if (pChar)
   {
      szLine[LNSZ-1] = 0;
      i = (int)strlen(szLine);
      while (i)
      {
         i--;
         if (szLine[i]=='\n' || szLine[i]=='\r')
            szLine[i] = 0;
         else
            i = 0;
      }
   }
   else
      *szLine = 0;

   return(pChar != NULL);
}


/*
 *  FilelistAdd
 */

int
FilelistAdd(const char *szFilename, FILELIST *pFilelist)
{
   int iErr = 0;


   // Increase the array if needed
   if (pFilelist->n >= pFilelist->max)
   {
      if (pFilelist->max)
      {
         pFilelist->max *= 2;
         pFilelist->array = (FILENAME *)
            realloc(pFilelist->array, (size_t)(pFilelist->max) * sizeof(FILENAME));
      }
      else
      {
         pFilelist->max = 16;
         pFilelist->array = (FILENAME *)
            malloc((size_t)(pFilelist->max) * sizeof(FILENAME));
      }

      if (!(pFilelist->array))
      {
         pFilelist->max = 0;
         iErr = ERROR_EQSELECT_MEM;
      }
   }

   if (!iErr)
   {
      // Append the filename into the array
      strcpy(pFilelist->array[pFilelist->n], szFilename);
      (pFilelist->n)++;
   }

   return(iErr);
}


/*
 *  FilelistExist
 */

int
FilelistExist(const char *szFilename, FILELIST *pFilelist)
{
   int   i,
         iNotFound = 1;


   i = pFilelist->n;
   while (i && iNotFound)
   {
      i--;
      iNotFound = strcmp(szFilename, pFilelist->array[i]);
   }

   return(!iNotFound);
}


/*
 *  FilenameSplit
 */

void
FilenameSplit(const char *szFilename,     char *szBase, char *szExt)
{
   int l;


   *szBase = (char)0;
   *szExt = (char)0;
   l = (int)strlen(szFilename);
   if (l > 2)
   {
      while (l && szFilename[l-1] != '.')
         l--;

      if (l)
      {
         StrnCopy(szBase, szFilename, l);
         StrnCopy(szExt, szFilename+l, LNSZ);
      }
   }
}


/*
 *  FilenameValid
 */

int
FilenameValid(const char *szFilename)
{
   char  szBase[LNSZ],
         szExt[LNSZ];


   FilenameSplit(szFilename,     szBase, szExt+1);
   if (szExt[1])
      *szExt = '.';
   else
      strcpy(szExt, "...");

   return(strcasestr(VALIDFILETYPES, szExt) != NULL);
}


/*
 *  ParseCurrentDirectory
 */

int
ParseCurrentDirectory(const char *pDirectoryName)
{
   int            iErr = 0,
                  iDirectoryNameLen;
   char           sz[LNSZ];
   DIR            *pDir;
   struct dirent  *pEntry;


   // Make sure pathnames from the current directory won't exceed LNSZ
   iDirectoryNameLen = (int)strlen(pDirectoryName);
   if (iDirectoryNameLen)
      iDirectoryNameLen++; // Consider the separating slash too

   // Process filenames in the current directory
   pDir = opendir(".");
   if (pDir)
   {
      do
      {
         pEntry = readdir(pDir);
         if (pEntry)
         {
            if (((int)strlen(pEntry->d_name) + iDirectoryNameLen) < LNSZ)
            {
               if (iDirectoryNameLen)
                  sprintf(sz, "%s%s%s", pDirectoryName, DIRSEP,
                          pEntry->d_name);
               else
                  strcpy(sz, pEntry->d_name);

               if ((pEntry->d_type & DT_REG) && FilenameValid(sz))
               {
                  if (!FilelistExist(sz, &gaExecutedFilenames))
                  {
                     FilelistAdd(sz, &gaAvailableFilenames);
                     if (giOptionContinu)
                        iErr = ERROR_EQSELECT_QUIT;
                  }
               }
               else if (!giOptionLeaf && (pEntry->d_type & DT_DIR)
                        && strcmp(pEntry->d_name, ".")
                        && strcmp(pEntry->d_name, ".."))
               {
                  if (chdir(pEntry->d_name))
                     printf("Warning: Can't enter %s, error %d" NL NL,
                            sz, errno);
                  else
                  {
                     iErr = ParseCurrentDirectory(sz);
                     chdir("..");
                  }
               }
            }
         }
      }
      while (pEntry && !iErr);
      closedir(pDir);
   }
   else
      iErr = ERROR_EQSELECT_DIR;

   if (iErr == ERROR_EQSELECT_QUIT)
      iErr = 0;

   return(iErr);
}


/*
 *  ResetExecutedFilenames
 */

void
ResetExecutedFilenames(void)
{
   FILE *hFile;


   hFile = fopen(gszExecutedFileName, FOPENWRITE);
   if (hFile)
      fclose(hFile);
}


/*
 *  StrnCopy
 */

void
StrnCopy(char *dst, const char *src, const int l)
{
   if (l > 1)
   {
      strncpy(dst, src, (size_t)(l-1));
      dst[l-1] = 0;
   }
   else
      *dst = 0;
}


/*
 *  Main
 */

int
main(int argc, char** argv)
{
   int      i,
            iErr = 0;
   char     sz[LNSZ + 20];
   FILE     *hFile;
   FILENAME szPath;


   // Initialisation
   memset(&gaAvailableFilenames, 0, sizeof(FILELIST));
   memset(&gaExecutedFilenames, 0, sizeof(FILELIST));

   StrnCopy(gszExecutedFileName, getenv(HOMEDIRPATHENV), LNSZ - 20);
   i = (int)strlen(gszExecutedFileName);
   if (i)
   {
      if (strcmp(gszExecutedFileName + i - 1, DIRSEP))
         strcat(gszExecutedFileName, DIRSEP);
   }
   strcat(gszExecutedFileName, EQSELECTDIR);
   if (!DirnameExist(gszExecutedFileName))
   {
      if (mkdir(gszExecutedFileName, DEFAULTDIRMODE))
         iErr = ERROR_EQSELECT_ERRNO;
   }
   strcat(gszExecutedFileName, DIRSEP);
   strcat(gszExecutedFileName, EXECUTEDFILENAMES);

   printf(NL "eqselect v0.91" NL
             "--------------" NL
             "https://github.com/fossette/eqselect/wiki" NL NL);

   if (!iErr)
   {
      // Options parsing
      for (i = 1 ; i < argc ; i++)
         if (argv[i][0])
         {
            if (strcmp(argv[i], "-c"))
            {
               if (strcmp(argv[i], "-l"))
               {
                  if (strcmp(argv[i], "-r"))
                  {
                     if (strcmp(argv[i], "-z"))
                        chdir(argv[i]);
                     else
                        giOptionReset = 1;

                  }
                  else
                     giOptionRepeatLast = 1;
               }
               else
                  giOptionLeaf = 1;
            }
            else
               giOptionContinu = 1;
         }

      getcwd(szPath, LNSZ-1);
      szPath[LNSZ-1] = 0;
      printf("Working directory: %s" NL, szPath);

      if (giOptionReset)
         ResetExecutedFilenames();
      else
      {
         // Read the executed files list
         hFile = fopen(gszExecutedFileName, FOPENREAD);
         if (hFile)
         {
            while (fgetsz(hFile,     sz))
               if (FilenameValid(sz))
                  FilelistAdd(sz, &gaExecutedFilenames);

            fclose(hFile);
         }
      }

      if (giOptionRepeatLast && gaExecutedFilenames.n)
      {
         sprintf(sz, "%s \"%s\" &", DEFAULTPLAYER, gaExecutedFilenames.array[
                                                      gaExecutedFilenames.n - 1]);
         system(sz);
         iErr = ERROR_EQSELECT_QUIT;
      }

      if (!iErr)
         // Find files that can be executed
         iErr = ParseCurrentDirectory("");
   }

   if (!(iErr || gaAvailableFilenames.n))
   {
      // All files have been executed once, start over!
      gaExecutedFilenames.n = 0;
      ResetExecutedFilenames();

      iErr = ParseCurrentDirectory("");
   }

   if (!iErr)
   {
      // Execute a filename
      if (gaAvailableFilenames.n)
      {
         if (giOptionContinu || (gaAvailableFilenames.n == 1))
            i = 0;
         else
         {
            srandomdev();
            i = random() % gaAvailableFilenames.n;
         }

         // Take note of the next executed file
         hFile = fopen(gszExecutedFileName, FOPENAPPEND);
         if (hFile)
         {
            fprintf(hFile, "%s" NL, gaAvailableFilenames.array[i]);
            fclose(hFile);
         }
         else
            iErr = ERROR_EQSELECT_ERRNO;

         sprintf(sz, "%s \"%s\" &", DEFAULTPLAYER, gaAvailableFilenames.array[i]);
         system(sz);
      }
      else
         iErr = ERROR_EQSELECT_EMPTY;
   }

   // Closing
   if (gaAvailableFilenames.array)
      free(gaAvailableFilenames.array);
   if (gaExecutedFilenames.array)
      free(gaExecutedFilenames.array);

   switch (iErr)
   {
      case ERROR_EQSELECT_DIR:
         printf(NL "ERROR: Specified path can't be accessed." NL NL);
         break;

      case ERROR_EQSELECT_EMPTY:
         printf(NL "ERROR: No file to execute in the current directory."
                NL NL);
         break;

      case ERROR_EQSELECT_ERRNO:
         printf(NL "ERROR: System Call Error#%d (ref. errno)" NL NL, errno);
         break;

      case ERROR_EQSELECT_MEM:
         printf(NL "ERROR: Out of memory." NL NL);
         break;

      case ERROR_EQSELECT_QUIT:
         iErr = 0;
         break;
   }

   return(iErr ? EXIT_FAILURE : EXIT_SUCCESS);
}

