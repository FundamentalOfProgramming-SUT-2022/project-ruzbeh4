//
// Created by ruzbeh on 1/14/23.
//
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"

int maint2(){
    char *real_address= "../root/1/2/af";
    real_address[14] = 'p';
    int result = mkdir(real_address, 0777);
    printf("%d" , result);
    if (access(real_address , F_OK) == 0) {
        printf("file already exists!\n");
//    system("tree");
    } else{
        printf("la");
    }
}
