//
// Created by ruzbeh on 1/21/23.
//
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "dirent.h"
enum { doSkip, isFile, isDir }
testDir(char *path, char *name)
{
    struct stat st_buf;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        return doSkip;
    }
    stat(path, &st_buf);
    if (S_ISDIR(st_buf.st_mode))
        return isDir;
    if (S_ISREG(st_buf.st_mode))
        return isFile;
}

void list(const char *path, int indentlevel , int depth){
    if(depth == 0)
        return;
    DIR *dirp = opendir(path);
    struct dirent *dentry;
    char buf[1024];
    if (!dirp) {
        printf("%*sNo access\n",indentlevel,"");
        return;
    }

    while ((dentry = readdir(dirp)) != NULL) {

        sprintf(buf,"%s/%s", path, dentry->d_name);
        switch (testDir(buf,dentry->d_name)) {
            case doSkip:
                /* do nothing */
                break;
            case isDir:
                printf("%*s%s:\n",indentlevel,"",dentry->d_name);
                list(buf,indentlevel+4 , depth-1);
                break;
            case isFile:
                if(dentry->d_name[0] !='.')
                    printf("%*s%s\n",indentlevel,"",dentry->d_name);
                break;
        }
    }
    closedir(dirp);
}

//int main()
//{
//    list("../root", 0 , 4);
//    return 0;
//}