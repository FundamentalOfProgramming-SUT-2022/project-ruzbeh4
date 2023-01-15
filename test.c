//
// Created by ruzbe on 1/2/2023.
//

//#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void maint()
{
    int check;
    char* dirname = "./root/2.txt";
    FILE * ptr;
    ptr = fopen(dirname , "w");
    if (ptr == NULL)
        printf("no");
    fputs("hello" , ptr);
    fclose(ptr);
//    check = mkdir(dirname ,);
//
//    // check if directory is created or not
//    if (!check)
//        printf("Directory created\n");
//    else {
//        printf("Unable to create directory\n");
//        exit(1);
//    }



  //  system("dir");

}