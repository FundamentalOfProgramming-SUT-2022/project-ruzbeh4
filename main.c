
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
char * create_tmp_file(char real_address[]){
    FILE * file;
    file = fopen(real_address, "r+");
    char *tmp =(char *) calloc(10000 , sizeof(char ));
    int j = 0;
    for (; 1 ; ++j) {
        tmp[j]= fgetc(file);
        if(tmp[j] == EOF)break;
    }
    tmp[j] = '\0';
    fclose(file);
    return tmp;
}
int access_to_position(char *tmp , int line , int pos){
    if(line == 1)
        pos--;
    int i = 0;
    for (; tmp[i] != EOF ; ++i) {
        if(tmp[i] == '\n')
            line --;
        if(line == 1)
            break;
    }
    i+= (pos+1);
    return i;
}

void insert_str_infile(char real_address[50], char input_str[100] , int line , int pos){

//    FILE * file;
//    file = fopen(real_address, "r+");
//    char tmp[1000] ={'\0'};
//    int j = 0;
//    for (; 1 ; ++j) {
//        tmp[j]= fgetc(file);
//        if(tmp[j] == EOF)break;
//    }
//    tmp[j] = '\0';
//    fclose(file);
    char *tmp = create_tmp_file(real_address);
    int i = access_to_position( tmp , line , pos);

//    if(line == 1)
//        pos--;
//    int i = 0;
//    for (; tmp[i] != EOF ; ++i) {
//        if(tmp[i] == '\n')
//            line --;
//        if(line == 1)
//            break;
//    }
//    i+= (pos+1);
//    printf("$ %d $" , i);
    memmove(&tmp[i+ strlen(input_str)] ,&tmp[i] , strlen(tmp) - i);
    memcpy(&tmp[i] , input_str , strlen(input_str));
    FILE * file = fopen(real_address, "w+");
    fputs(tmp, file);
    fclose(file);
}
void remove_str_fromfile(char real_address[50], int size, int line , int pos , char direction){
    char *tmp = create_tmp_file(real_address);
    int i = access_to_position( tmp , line , pos);

    if(direction == 'f'){
        memmove(&tmp[i] ,&tmp[i+size] , strlen(tmp) - i -size);
    } else if(direction == 'b'){
        memmove(&tmp[i-size] ,&tmp[i] , strlen(tmp) - i);
    } else{
        invalid_input();
        printf("no such option for this command\n");
    }

    FILE * file = fopen(real_address, "w");
    fputs(tmp, file);
    fclose(file);
//    printf("done\n");
}
void copy_str_fromfile(char real_address[50], int size, int line , int pos , char direction){
    char *saved_data = (char *)calloc(10000 , sizeof (char ));
    char *tmp = create_tmp_file(real_address);
    int i = access_to_position( tmp , line , pos);

    if(direction == 'f'){
        memcpy(saved_data ,&tmp[i] , size);
    } else if(direction == 'b'){
        memcpy(saved_data,&tmp[i - size] , size);
    } else{
        invalid_input();
    }
    FILE * clipboard = fopen("./clipboard.txt" , "w");
    fputs(saved_data , clipboard);
    fclose(clipboard);
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
void cat(){
    char subcommand[10];
    char real_address[50] = "../";
    scanf("%s" , subcommand);

    if (!strcmp(subcommand, "--file")){

        get_address(real_address);
        if (access(real_address, F_OK) != 0) {
            invalid_input();
            printf("no such file or directory\n");
            return;
        }


        FILE * file;
        file = fopen(real_address, "r+");
        char tmp ='\0';
        int j = 0;
        for (; 1 ; ++j) {
            tmp= fgetc(file);
            if(tmp == EOF)break;
            printf("%c" , tmp);
        }
        fclose(file);

    } else {
        invalid_input();
        printf("invalid arguments for createfile\n");
    }
}

void remove_or_copy_or_cutstr(int specifier){  // 0 for remove 1 for copy 2 for cut
    char subcommand[10];
    char first , direction ;
    char real_address[50] = "../";
    int size ,dir;
    int line , pos;
    for (int i = 0; i < 3; ++i) {
        scanf(" %c" , &first);
        if(first != '-'){
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

        } else if (!strcmp(subcommand, "-size")) {
            scanf("%d" ,&size);
        } else if (!strcmp(subcommand, "-pos")) {
            scanf(" %d,%d" , &line,&pos);
        }else {
            invalid_input();
            printf("invalid arguments for insertstr\n");
        }

    }
    scanf(" -%c" , &direction);
    if(specifier == 0)
        remove_str_fromfile(real_address, size, line , pos , direction);
    else if(specifier == 1)
        copy_str_fromfile(real_address, size, line , pos , direction);
    else{
        copy_str_fromfile(real_address, size, line , pos , direction);
        remove_str_fromfile(real_address, size, line , pos , direction);
    }

    printf("Done\n");

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
            cat();

        } else if (!strcmp(command, "removestr")) {
            remove_or_copy_or_cutstr(0);
        } else if (!strcmp(command, "copystr")) {
            remove_or_copy_or_cutstr(1);
        } else if (!strcmp(command, "cutstr")) {
            remove_or_copy_or_cutstr(2);
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

    return(0);

}