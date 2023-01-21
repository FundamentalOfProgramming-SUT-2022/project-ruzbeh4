//
// Created by ruzbeh on 1/16/23.
//
#include <string.h>
#include <stdio.h>

int mains () {
    char str[80] = "/root/3/4.txt";
    char * name = strrchr(str , '/');
    printf("%d" , strlen(name));

    return(0);
}