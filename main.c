
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "dirent.h"
#include "tree.h"
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
void get_str(char input_str[] ){
    char tmp;

    scanf(" %c" , &tmp);

    if(tmp == '"') {
        int j = 0;
        for (; 1 ; ++j) {
            input_str[j]= fgetc(stdin);
            if(input_str[j] == '\"' && input_str[j-1] != '\\')break;
        }
        input_str[j] = '\0';
    }
    else {
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
char * last_version_address(char real_address[]){
    char * version_address = calloc(50 ,sizeof(char ));
    strcpy(version_address , real_address);
    char *filename = strrchr( version_address, '/');
    filename++;
    memmove(&filename[1],&filename[0], strlen(filename));
    filename[0] = '.';
    return version_address;
}
char * create_tmp_file(char real_address[]){
    char *version_address = last_version_address(real_address);
    FILE * file;
    FILE * last_version;
    last_version = fopen(version_address, "w");
    file = fopen(real_address, "r+");
    char *tmp =(char *) calloc(10000 , sizeof(char ));
    int j = 0;
    for (; 1 ; ++j) {
        tmp[j]= fgetc(file);
        if(tmp[j] == EOF)break;
        fputc(tmp[j] , last_version);
    }
 //   tmp[j] = '\0';
    fclose(file);
    fclose(last_version);
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

    char *tmp = create_tmp_file(real_address);
  //  last_version_address(real_address);
    int i = access_to_position( tmp , line , pos);

    memmove(&tmp[i+ strlen(input_str)] ,&tmp[i] , strlen(tmp) - i);
    memcpy(&tmp[i] , input_str , strlen(input_str));
    tmp[strlen(tmp) - 1] = '\0';
    FILE * file = fopen(real_address, "w+");
    fputs(tmp, file);
    fclose(file);
}
void remove_str_fromfile(char real_address[50], int size, int line , int pos , char direction){
    char *tmp = create_tmp_file(real_address);
    int i = access_to_position( tmp , line , pos);

    if(direction == 'f'){
        memmove(&tmp[i] ,&tmp[i+size] , strlen(tmp) - i -size + 1 );
    } else if(direction == 'b'){
        memmove(&tmp[i-size] ,&tmp[i] , strlen(tmp) - i + 1 );
    } else{
        invalid_input();
        printf("no such option for this command\n");
    }
    tmp[strlen(tmp) - 1] = '\0';
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
    tmp[strlen(tmp) ] = '\0';
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
    char real_address[50] = "../";
    char input_str[100];
    int line , pos;
    for (int i = 0; i < 3; ++i) {

        scanf(" %c" , &tmp);
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
            get_str(input_str );
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

void pastestr(){
    char subcommand[10];
    char tmp ;
    char real_address[50] = "../";
    int line , pos;
    for (int i = 0; i < 2 ; ++i) {

        scanf(" %c" , &tmp);
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

        }else if (!strcmp(subcommand, "-pos")) {
            scanf(" %d,%d" , &line,&pos);
        }
        else {
            invalid_input();
            printf("invalid arguments for insertstr\n");
        }
    }
    char *saved_data = create_tmp_file("./clipboard.txt");
    saved_data[strlen(saved_data) -1] = '\0';
    insert_str_infile(real_address , saved_data , line , pos);

    printf("Done\n");

}
void find_by_words(int specifier , int size){
    printf("w: %d ,%d" , specifier , size);
}
void find_by_chars(int specifier , int size){
    printf("c: %d ,%d" , specifier , size);
}
void find(){
    char subcommand[10];
    char options[30];
    char tmp ;
    int word_flag = 0, options_specifier = 0 , at_size = 0;
    char real_address[50] = "../";
    char input_str[100];
    for (int i = 0; i < 2; ++i) {

        scanf(" %c" , &tmp);
        if(tmp != '-'){
            invalid_input();
            printf("invalid arguments for find\n");
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
            get_str(input_str );
        }else {
            invalid_input();
            printf("invalid arguments for find\n");
        }
    }
    scanf("%[^'\n']s" , options);

    const char s[2] = "-";
    char *token;
    token = strtok(options, s);
    while( token != NULL ) {
        if(token[strlen(token) -  1] == ' ')
            token[strlen(token) -  1] = '\0';
        if (!strcmp(token, "count")){
            if(options_specifier == 0)
                options_specifier = 1;
            else {
                invalid_input();
                printf("logical error\n");
                return;
            }
        } else if(token[0]=='a' && token[1] == 't'){
            if(options_specifier == 0)
                options_specifier = 2;
            else {
                invalid_input();
                printf("logical error\n");
                return;
            }
            sscanf(token, "at %d" , &at_size);
        }else if(!strcmp(token, "all")) {
            if(options_specifier == 0)
                options_specifier = 3;
            else {
                invalid_input();
                printf("logical error\n");
                return;
            }
        }else if(!strcmp(token, "byword"))
            word_flag =1;

        token = strtok(NULL, s);
    }
    if(word_flag)
        find_by_words(options_specifier , at_size);
    else
        find_by_chars(options_specifier , at_size);

}
void undo(){
    char subcommand[10];
    char first ;
    char real_address[50] = "../";

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
        }else {
            invalid_input();
            printf("invalid arguments for insertstr\n");
        }
    char * version_address = last_version_address(real_address);
    char * last_data = create_tmp_file(version_address);
    create_tmp_file(real_address);
    last_data[strlen(last_data) - 1] = '\0';
    FILE *file = fopen(real_address , "w");
    fputs(last_data , file);
    fclose(file);
    char * wrong_file = last_version_address(version_address);
    remove(wrong_file);
    printf("Done!\n");
}
void compare_line_by_line(int specifier , char address1[] , char  address2[]){
    char * file1 = create_tmp_file(address1);
    file1[strlen(file1) -1] = '\0';
    char * file2 = create_tmp_file(address2);
    file2[strlen(file2) -1] = '\0';
    const char s[2] = "\n\0";
    char * file1_token;
    char * file2_token;
    file1_token = strsep(&file1, s);
    file2_token = strsep(&file2 , s);
    for (int i = 1; file1_token != NULL && file2_token != NULL ; i++) {
        if (strcmp(file1_token, file2_token)){
            printf("######### line %d #########\n%s\n%s\n" , i , file1_token, file2_token );
        }
        file1_token = strsep(&file1, s);
        file2_token = strsep(&file2, s);
    }
    if(file1_token != NULL){
        printf("######## first file is longer ########\n");
        while (file1_token != NULL){
            printf("%s\n" , file1_token);
            file1_token = strsep(&file1 , s);
        }
    }
    if(file2_token != NULL){
        printf("!!!!!!!!! second file is longer !!!!!!!!!\n");
        while (file2_token != NULL){
            printf("%s\n" , file2_token);
            file2_token = strsep(&file2 , s);
        }
    }
}
void compare(){
    char subcommand[10];
    char options[30];
    int options_specifier = 0;
    char real_address1[50] = "../";
    char real_address2[50] = "../";

    scanf("%s", subcommand);

    if (!strcmp(subcommand, "--files")) {
        get_address(real_address1);
        get_address(real_address2);
        if (access(real_address1, F_OK) != 0 || access(real_address2, F_OK) != 0 ) {
            invalid_input();
            printf("no such file or directory\n");
            return;
        }
    }else {
        invalid_input();
        printf("invalid arguments for compare\n");
    }
    scanf("%[^'\n']s" , options);

    const char s[2] = "-";
    char *token;
    token = strtok(options, s);
    while( token != NULL ) {
        if(token[strlen(token) -  1] == ' ')
            token[strlen(token) -  1] = '\0';
        if (!strcmp(token, "c")){
            if(options_specifier == 0)
                options_specifier = 1;
            else {
                invalid_input();
                printf("logical error\n");
                return;
            }
        }else if (!strcmp(token, "I")) {
            if (options_specifier == 0)
                options_specifier = 2;
            else {
                invalid_input();
                printf("logical error\n");
                return;
            }
        }

        token = strtok(NULL, s);
    }
    compare_line_by_line(options_specifier , real_address1, real_address2);
}
void tree(){
    char subcommand[10];
    int depth;
    char real_address[50] = "../";
    scanf("%d" , &depth);
    if(depth<-1){
        invalid_input();
        printf("invalid depth\n");
    }
    scanf("%s", subcommand);

    if (!strcmp(subcommand, "--dir")) {
        get_address(real_address);
        if (access(real_address, F_OK) != 0) {
            invalid_input();
            printf("no such file or directory\n");
            return;
        }
    }else {
        invalid_input();
        printf("invalid arguments for depth\n");
    }
    list(real_address , 0 , depth);

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
            pastestr();
        } else if (!strcmp(command, "find")) {
            find();
        } else if (!strcmp(command, "replace")) {

        } else if (!strcmp(command, "greb")) {

        } else if (!strcmp(command, "undo")) {
            undo();
        } else if (!strcmp(command, "autoindent")) {

        } else if (!strcmp(command, "compare")) {
            compare();
        } else if (!strcmp(command, "tree")) {
            tree();
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