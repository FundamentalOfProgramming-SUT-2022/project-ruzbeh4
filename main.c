
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
    printf("error: ");
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
void get_address(char real_address[]){
    char file_address[50] ;
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
        invalid_input();
        printf("invalid address\n");
        return;
    }
    strcat(real_address , file_address);
}
void get_str(char input_str[] , int flag){
    char tmp;
    int special_chars_counter = 0;
    scanf(" %c" , &tmp);

    if(tmp == '"') {
        flag = 1;
        scanf("%[^-]s", input_str);
        char *last = strrchr(input_str, '"');
        *last = '\0';
    }
    else {
        flag = 0;
        input_str[0] = tmp;
        input_str++;
        scanf("%s", input_str);
        input_str--;
    }
    for (int i = 0; i < strlen(input_str); ++i) {
        if(input_str[i] == '\\'){
            switch (input_str[i+1]) {
                case 'n':
                    input_str[i+1] = '\n';
                    memmove(&input_str[i] , &input_str[i+1], strlen(input_str) - i);
                    break;
                case 't':
                    input_str[i+1] = '\t';
                    memmove(&input_str[i] , &input_str[i+1], strlen(input_str) - i);
                    break;
                case '\'':
                    input_str[i+1] = '\'';
                    memmove(&input_str[i] , &input_str[i+1], strlen(input_str) - i);
                    break;
                case '"':
                    input_str[i+1] = '"';
                    memmove(&input_str[i] , &input_str[i+1], strlen(input_str) - i);
                    break;
                case '\\':
                    memmove(&input_str[i] , &input_str[i+1], strlen(input_str) - i);
                    i++;
                    break;
            }
        }
    }
}

void insert_str_infile(char real_address[50], char input_str[100] , int line , int pos){
    FILE * file;
    file = fopen(real_address, "r+");
    char tmp[1000];
    for (int i = 0; 1 ; ++i) {
        fscanf(file , "%c" , &tmp[i]);
        if(tmp[i] == EOF)break;
    }
    fclose(file);
    int i = 0;
    for (; tmp[i] != EOF ; ++i) {
        if(tmp[i] == '\n')
            line --;
        if(line == 1)
            break;
    }
    i+= (pos+1);
    memmove(&tmp[i+ strlen(input_str)] ,&tmp[i] , strlen(tmp) - i);
    memcpy(&tmp[i] , input_str , strlen(input_str));
    file = fopen(real_address, "w+");
    fputs(tmp, file);
    fclose(file);
}
void create_file(){
    char subcommand[10];
    char real_address[50] = "../";
    scanf("%s" , subcommand);

    if (!strcmp(subcommand, "--file")){

        get_address(real_address);
        if (access(real_address , F_OK) == 0) {
            invalid_input();
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
        invalid_input();
        printf("invalid arguments for createfile\n");
    }

}
void insertstr(){
    char subcommand[10];
    char tmp ;
    int flag = 0;
    char real_address[50] = "../";
    char input_str[100];
    int line , pos;
    for (int i = 0; i < 3; ++i) {
        if(!flag)
            scanf(" ");
        scanf("%c" , &tmp);
        if(tmp != '-'){
            invalid_input();
            printf("invalid arguments for insertstr\n");
            return;
        }
        scanf("%s", subcommand);

        if (!strcmp(subcommand, "-file")) {
            get_address(real_address);
            if (access(real_address, F_OK) != 0) {
                invalid_input();
                printf("no such file or directory\n");
                return;
            }

        } else if (!strcmp(subcommand, "-str")) {
            get_str(input_str , flag);
        } else if (!strcmp(subcommand, "-pos")) {
            scanf(" %d,%d" , &line,&pos);
        }
        else {
            invalid_input();
            printf("invalid arguments for insertstr\n");
        }
    }

    insert_str_infile(real_address , input_str , line , pos);

    printf("done\n");


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
            insertstr();
        } else if (!strcmp(command, "cat")) {
 //           cat();

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
            invalid_input();
            printf("invalid command\n");
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