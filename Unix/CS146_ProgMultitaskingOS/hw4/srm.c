/** 
 * srm: safely removes files by moving them to a TRASH folder
 * usage: $ ./srm [files or directories]
 * 
 * notes: the TRASH environment variable must be set to the path of an existing directory
 * in order to run srm; otherwise, an error is printed and srm will quit. srm will 
 * overwrite previously srm'd files that share a name. 
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Useful error messages
const char* ERROR_NO_TRASH_ENV = "srm: error: TRASH environment variable must be set to path of a directory\n";
const char* WARN_NO_FILE = "srm: unable to remove \"%s\": No such file or directory \n";
const char* WARN_SRM_ROOT = "srm: cannot remove root, \"/\"\n";
const char* ERROR_CANT_CREATE = "srm: error: unable to safely remove \"%s\" (could not create a destination file)\n";

/** 
 * Gets the basename of a file or directory, defined as the substr after the
 * last meaningful "/" character. The returned string must be freed.
 */
char* getBasename(char* path);

/** 
 * Concatenates two strings to form a full path. The returned string must be freed.
 */
char* createPath(char* basePath, char* filename);

/**
 * Gets the mode of a file (which includes things such as write/read/exec permissions)
 */
mode_t getFileMode(char* filePath);

/** 
 * Moves file contents from src to dst, returning a positive integer if something is moved, 0 
 * otherwise. A positive return value may be the number of bytes read from a file, or
 * the number of files and directories moved. Introspection on what is moved is needed
 * to determine the meaning of the value.
 */
int moveContents(char* dst, char* src);

/**
 * Moves a regular file from src to dst, returning the number of bytes read.
 */
int moveFile(char* dst, char* src);

/**
 * Moves a directory from src to dst, returning the number of files + directories moved.
 */
int moveDir(char* dst, char* src);

char* trashDirPath = NULL;
int main(int argc, char* argv[]){
    // check existence of TRASH environment variable
    trashDirPath = getenv("TRASH");
    if (trashDirPath == NULL){
        fprintf(stderr, ERROR_NO_TRASH_ENV);
        return EXIT_FAILURE;
    }else{
        DIR* d = NULL;
        if ((d = opendir(trashDirPath)) == NULL){
            fprintf(stderr, ERROR_NO_TRASH_ENV);
            return EXIT_FAILURE;
        }
        closedir(d);
    }
    
    // move all files given to trash
    int i;
    for (i = 1; i < argc; ++i){
        char* srcPath = argv[i];
        moveContents(trashDirPath,srcPath);
    }

    return EXIT_SUCCESS;
}

char* getBasename(char* filePath){
    // ensure filePath is a string with len at least 1
    if (filePath == NULL){
        return NULL;
    }
    int len = strlen(filePath);
    if (len < 1){
        return NULL;
    }
    // if "/" is given, return "/"
    if (len == 1 && filePath[0] == '/'){
        return "/";
    }

    char* basename = (char*)malloc(sizeof(char)*len+1); 
    strcpy(basename, filePath);
    char* pos = basename;

    // look for the last appearance of '/' that is not the
    // end of the string
    while(1){
        pos = strrchr(basename, '/');
        if (pos != NULL){
            if ((int)(pos - basename) == len - 1){
                // file path was given with a trailing '/'
                basename[len-1] = '\0';
                continue;
            }else{
                // found the basename, extract it by copying
                // the end to the beginning of the string
                int n = (int)(pos-basename);
                strncpy(basename, basename+n+1,len-n);
            }
        }else{
            break; // file path contains no '/'
        }
    }
    return basename;
}

char* createPath(char* basePath, char* filename){
    // concat basePath to filename, with a "/" between
    int len = strlen(basePath) + strlen(filename) + 2;
    char* dstPath = (char*) malloc(sizeof(char) * len + 1);
    strcpy(dstPath, basePath);
    strcat(dstPath, "/");
    strcat(dstPath, filename);
    return dstPath;
}

mode_t getFileMode(char* filePath){
    // use stat to get the file mode
    struct stat sb;
    if (stat(filePath, &sb) == -1){
        return 0;
    }
    return sb.st_mode;
}

int moveContents(char* dst, char* src){
    // get the name of what we're trying to move and ensure we can work with it
    char* srcBasename = getBasename(src);
    if (srcBasename == NULL){
        fprintf(stderr, WARN_NO_FILE, src);
        free(srcBasename);
        return 0;
    }
    if(strcmp(srcBasename, "/") == 0){
        fprintf(stderr, WARN_SRM_ROOT);
        return 0;
    }

    // create a path for the file in TRASH
    char* dstPath = createPath(dst,srcBasename);
    free(srcBasename);

    // get the file mode and use it to determine if it's a directory or other file type
    mode_t srcMode = getFileMode(src);
    if (srcMode == 0){
        fprintf(stderr, WARN_NO_FILE, src);
        free(dstPath);
        return 0;
    }

    int n = 0;
    if (S_ISDIR(srcMode)){
        n = moveDir(dstPath, src);
    }
    else{
        n = moveFile(dstPath, src);
    }

    free(dstPath);
    
    return n;
}

int moveFile(char* dst, char* src){
    // try to open the src file
    int srcFd = open(src, O_RDONLY);
    if (srcFd == -1){
        fprintf(stderr, WARN_NO_FILE, src);
        return 0;
    }
    // copy the permissions from the src file and try to make the dst file in TRASH
    mode_t mode = 0;
    if ( (mode = getFileMode(src) & 0777) == 0){
        mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
    }
    int dstFd = open(dst, O_CREAT|O_WRONLY|O_TRUNC, mode);
    if (dstFd == -1){
        fprintf(stderr, ERROR_CANT_CREATE, dst);
        return 0;
    }

    // copy file contents to dst from src
    int r = 0;
    int wrote = 0, w = 0;
    char buf[1024];
    while( (r = read(srcFd, buf, 1024)) > 0 ){
        w = write(dstFd, buf, r); // write only the amount read
        wrote += w; // keep track of bytes written
    }

    // close our files and remove the src file
    close(dstFd);
    close(srcFd);
    unlink(src);

    return wrote;
}

int moveDir(char* dst, char* src){
    // try to open the src directory
    DIR* srcDir = opendir(src);
    if (srcDir == NULL){
        fprintf(stderr, WARN_NO_FILE, src);
        return 0;
    }

    // copy the permissions from src and try to make or open the dst directory
    mode_t mode = 0;
    if ( (mode = getFileMode(src) & 0777) == 0){
        mode = S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IROTH;
    }
    if (mkdir(dst, mode) == -1 && errno != EEXIST){
        fprintf(stderr, ERROR_CANT_CREATE, dst);
        return 0;
    }
    DIR* dstDir = opendir(dst);
    if (dstDir == NULL){
        fprintf(stderr, ERROR_CANT_CREATE, dst);
        return 0;
    }

    // read the directory contents and recursively move nested directories and files
    int n = 1; // count of things moved
    struct dirent* ent;
    while ((ent = readdir(srcDir)) != NULL){
        char* entName = ent->d_name;
        if (strcmp(entName, ".") == 0 || strcmp(entName, "..") == 0){
            // readdir may return ".." or "." as entries, which we ignore
            continue;
        }
        // create the paths for moving this entry
        char* srcPath = createPath(src,entName);
        char* dstPath = createPath(dst,entName);

        // recursively call moveDir if the file is a directory, otherwise call moveFile
        if (S_ISDIR(getFileMode(srcPath))){
            int d = 0;
            if ((d = moveDir(dstPath, srcPath)) > 0){
                n += d;
            }
        }else{
            if (moveFile(dstPath, srcPath) > 0){
                n += 1;
            }
        }
        free(srcPath);
        free(dstPath);
    }

    // close both directories, removing the src directory (it should be empty)
    closedir(dstDir);
    closedir(srcDir);
    rmdir(src);

    return n;
}