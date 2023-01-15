
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"

void invalid_input(){
    char tmp;
    char tmp2[100];
    scanf("%c", &tmp);
    if (tmp == ' ') {
        scanf("%[^'\n']s", tmp2);
    }
    printf("Invalid input\n");
}
void make_dir(char * path){
    char *seprator = strrchr(path , '/');
    if( seprator != NULL){
        *seprator = 0;
        make_dir(path);
        *seprator = '/';
    }
    mkdir(path , 0777);
}
void create_file(){
    char subcommand[10];
    char file_address[20] ;
    scanf("%s" , subcommand);

    if (!strcmp(subcommand, "--file")){
        char tmp;
        scanf(" %c" , &tmp);

        if((int)tmp == 47 ) {

            scanf("%s", file_address);
        }
        else if(tmp == '"') {
            scanf("/");
            scanf("%[^'\"']s", file_address);
            scanf("\"");
        }
        else {
            printf("2");
            invalid_input();
            return;
        }
        char real_address[50] = "../";
        strcat(real_address , file_address);
        if (access(real_address , F_OK) == 0) {
            printf("file already exists!\n");
        } else {
            char dir_address[50];
            strcpy(dir_address , real_address);
            char *filename = strrchr(dir_address, '/');
            for (int i = 0;*(filename+i+1) != '\0'; ++i) {
                *(filename+i) ='\0';
            }
            make_dir(dir_address);

            FILE * file;
            file = fopen(real_address, "w");
            fclose(file);
            printf("done\n");
        }
    } else {
        printf("1");
        invalid_input();
    }


}
void get_command() {
    char command[30];
    while (1) {
        scanf("%s", command);
        if (!strcmp(command, "exit"))
            break;
        else if (!strcmp(command, "createfile")) {
            create_file();
        } else if (!strcmp(command, "insertstr")) {

        } else if (!strcmp(command, "cat")) {

        } else if (!strcmp(command, "removestr")) {

        } else if (!strcmp(command, "copystr")) {

        } else if (!strcmp(command, "cutstr")) {

        } else if (!strcmp(command, "pastestr")) {

        } else if (!strcmp(command, "find")) {

        } else if (!strcmp(command, "replace")) {

        } else if (!strcmp(command, "greb")) {

        } else if (!strcmp(command, "undo")) {

        } else if (!strcmp(command, "autoindent")) {

        } else if (!strcmp(command, "compare")) {

        } else if (!strcmp(command, "tree")) {

        } else {
            printf("3");
            invalid_input();
        }
    }
    printf("bye");
}

int main(){
    get_command();
    char a[30];


//        FILE * fp;
//
//        fp = fopen ("../root/file.txt", "r+");
//        fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);
//
//        fclose(fp);
//        fp = fopen("../root/file.txt","r+");
//        fscanf(fp , "%s" , a);
//        fclose(fp);
//
//        printf("%s", a);
//
//

    return(0);

}