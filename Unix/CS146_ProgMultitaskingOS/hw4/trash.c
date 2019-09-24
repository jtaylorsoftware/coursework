/** 
 * trash: deletes all files that currently exist in the TRASH directory. The 
 * TRASH environment variable must be set to an existing directory in order
 * to run trash. trash takes no arguments.
 * 
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
const char* ERROR_NO_TRASH_ENV = "trash: error: TRASH environment variable must be set to path of a directory\n";
const char* WARN_NO_FILE = "trash: unable to delete \"%s\" from TRASH: No such file or directory \n";

/** 
 * Concatenates two strings to form a full path. The returned string must be freed.
 */
char* createPath(char* basePath, char* filename);

/**
 * Gets the mode of a file (which includes things such as write/read/exec permissions)
 */
mode_t getFileMode(char* filePath);

/**
 * Deletes a directory and all nested directories or files. If deleteTop == 1, 
 * then the topmost directory (the one passed as "path") is not deleted, only
 * its contents are deleted.
 */
void deleteDir(char* path, int deleteTop);

char* trashDirPath = NULL;
DIR* trashDir = NULL;
int main(int argc, char* argv[]){
    // check existence of TRASH environment variable
    trashDirPath = getenv("TRASH");
    if (trashDirPath == NULL){
        fprintf(stderr, ERROR_NO_TRASH_ENV);
        return EXIT_FAILURE;
    }else{
        if ((trashDir = opendir(trashDirPath)) == NULL){
            fprintf(stderr, ERROR_NO_TRASH_ENV);
            return EXIT_FAILURE;
        }
    }
    
    // delete the files in the trash directory
    deleteDir(trashDirPath, 0);
    // struct dirent* ent;
    // while ((ent = readdir(trashDir)) != NULL){
    //     char* entName = ent->d_name;
    //     if (strcmp(entName, ".") == 0 || strcmp(entName, "..") == 0){
    //         // readdir may return ".." or "." as entries, which we ignore
    //         continue;
    //     }

    //     // create the full name of the file
    //     char* path = createPath(trashDirPath, entName);

    //     // delete the file or directory
    //     if (S_ISDIR(getFileMode(path))){
    //         // use deleteDir to remove directories
    //         deleteDir(path);
    //     }else{
    //         // remove file with unlink
    //         unlink(path);
    //     }

    //     free(path);
    // }

    closedir(trashDir);
    return EXIT_SUCCESS;
}

char* createPath(char* basePath, char* filename){
    // concat basePath to filename, with a "/" between
    int len = strlen(basePath) + strlen(filename) + 2;
    char* dstPath = (char*) malloc(sizeof(char) * len);
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

void deleteDir(char* path, int deleteTop){
    // try to open the directory
    DIR* dir = opendir(path);
    if (dir == NULL){
        fprintf(stderr, WARN_NO_FILE, path);
        return;
    }

    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL){
        char* entName = ent->d_name;
        if (strcmp(entName, ".") == 0 || strcmp(entName, "..") == 0){
            // readdir may return ".." or "." as entries, which we ignore
            continue;
        }
        // create the paths for moving this entry
        char* entPath = createPath(path,entName);

        // recursively call moveDir if the file is a directory, otherwise call moveFile
        if (S_ISDIR(getFileMode(entPath))){
            deleteDir(entPath, 1);
        }
        else{
            unlink(entPath);
        }
        free(entPath);
    }

    // close both directories, removing the src directory (it should be empty)
    closedir(dir);
    if (deleteTop == 1){
        rmdir(path);
    }
}