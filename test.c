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
    int size = 0;
    char* dirname = "../root/2.txt";
    char *tmp;
    FILE * ptr;
    ptr = fopen(dirname , "a+");
//    if (ptr == NULL)
//        printf("no");
    rewind(ptr);
    getline(&tmp,&size,ptr);
    getline(&tmp,&size,ptr);
   fseek(ptr, 20 , 2);
    fprintf(ptr , "$$$");
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