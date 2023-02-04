
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "unistd.h"
#include "ncurses.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "dirent.h"
#include "tree.h"
#include "curses.h"

char  command_line[100] ;
WINDOW * text_part;
WINDOW * last_text_part;
WINDOW * command_part;
WINDOW * last_new_line_position;
WINDOW * status;
FILE * command_file;
WINDOW * new_line_position;
WINDOW * num;
void invalid_input(){
    char tmp;
    char tmp2[100];
    fscanf(command_file , "%c", &tmp);
    if (tmp == ' ') {
        fscanf(command_file , "%[^'\n']s", tmp2);
    }
    wprintw( command_part , "error: ");
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
int get_address(char real_address[]){
    char file_address[50] ;
    char tmp;
    tmp = fgetc(command_file);
    if(tmp == '\n')
        return 0;
    fscanf(command_file , "%c" , &tmp);

    if(tmp == 47 ) {
        fscanf(command_file , "%s", file_address);
    }
    else if(tmp == (int)'"') {
        wscanw(command_part , "/");
        wscanw(command_part , "%[^'\"']s", file_address);
        wscanw(command_part , "\"");
    }
    else {
        invalid_input();
        wprintw(command_part , "invalid address\n");
        return 0;
    }
    strcat(real_address , file_address);
    return 1;
}
void get_str(char input_str[] ){
    char tmp;

    fscanf(command_file , " %c" , &tmp);

    if(tmp == '"') {
        int j = 0;
        for (; 1 ; ++j) {
            input_str[j]= fgetc(command_file);
            if(input_str[j] == '\"' && input_str[j-1] != '\\')break;
        }
        input_str[j] = '\0';
    }
    else {
        input_str[0] = tmp;
        input_str++;
        fscanf(command_file , "%s", input_str);
        input_str--;
    }
    for (int i = 0; i < strlen(input_str); ++i) {
        if(input_str[i] == '*' && (*(input_str+i-1) != '\\' || *(input_str+i-1) == EOF))
            input_str[i] = '\a';
        if(input_str[i] == '\\'){
            switch (input_str[i+1]) {
                case '*':
                    input_str[i+1] = '*';
                    memmove(&input_str[i] , &input_str[i+1], strlen(input_str) - i);
                    break;
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
void create_last_version(char real_address[]){
    char *version_address = last_version_address(real_address);
    FILE * file;
    FILE * last_version;
    last_version = fopen(version_address, "w");
    file = fopen(real_address, "r+");
    char *tmp =(char *) calloc(10000000 , sizeof(char ));
    int j = 0;
    for (; 1 ; ++j) {
        tmp[j]= fgetc(file);
        if(tmp[j] == EOF)break;
        fputc(tmp[j] , last_version);
    }
    //   tmp[j] = '\0';
    fclose(file);
    fclose(last_version);
}
char * create_tmp_file(char real_address[]){
    FILE * file;
    file = fopen(real_address, "r+");
    char *tmp =(char *) calloc(10000000 , sizeof(char ));
    int j = 0;
    for (; 1 ; ++j) {
        tmp[j]= fgetc(file);
        if(tmp[j] == EOF)break;
    }
    //   tmp[j] = '\0';
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

    char *tmp = create_tmp_file(real_address);
  //  create_last_version(real_address);
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
    //create_last_version(real_address);
    int i = access_to_position( tmp , line , pos);

    if(direction == 'f'){
        memmove(&tmp[i] ,&tmp[i+size] , strlen(tmp) - i -size + 1 );
    } else if(direction == 'b'){
        memmove(&tmp[i-size] ,&tmp[i] , strlen(tmp) - i + 1 );
    } else{
        invalid_input();
        wprintw(command_part , "no such option for this command\n");
    }
    tmp[strlen(tmp) - 1] = '\0';
    FILE * file = fopen(real_address, "w");
    fputs(tmp, file);
    fclose(file);
//    wprintw(command_part , "done\n");
}
void copy_str_fromfile(char real_address[50], int size, int line , int pos , char direction){
    char *saved_data = (char *)calloc(10000 , sizeof (char ));
    char *tmp = create_tmp_file(real_address);
    create_last_version(real_address);
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
//    getch();

    fscanf(command_file , "%s" , subcommand);
    if (!strcmp(subcommand, "--file")){

        get_address(real_address);
        if (access(real_address , F_OK) == 0) {
            invalid_input();
            wprintw(command_part , "file already exists!\n");
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
            wprintw(command_part , "Done\n");
        }
    } else {
        invalid_input();
        wprintw(command_part , "invalid arguments for createfile\n");
    }

}
void insertstr(){
    char subcommand[10];
    char tmp ;
    char real_address[50] = "../";
    char input_str[100];
    int line , pos;
    for (int i = 0; i < 3; ++i) {

        fscanf(command_file , " %c" , &tmp);
        if(tmp != '-'){
            invalid_input();
            wprintw(command_part , "invalid arguments for insertstr\n");
            return;
        }
        fscanf(command_file , "%s", subcommand);

        if (!strcmp(subcommand, "-file")) {
            get_address(real_address);
            if (access(real_address, F_OK) != 0) {
                invalid_input();
                wprintw(command_part , "no such file or directory\n");
                return;
            }

        } else if (!strcmp(subcommand, "-str")) {
            get_str(input_str );
        } else if (!strcmp(subcommand, "-pos")) {
            fscanf(command_file , " %d,%d" , &line,&pos);
        }
        else {
            invalid_input();
            wprintw(command_part , "invalid arguments for insertstr\n");
        }
    }
    create_last_version(real_address);
    insert_str_infile(real_address , input_str , line , pos);

    wprintw(command_part , "done\n");

}
void cat(){
    char subcommand[10];
    char real_address[50] = "../";
    fscanf(command_file , "%s" , subcommand);

    if (!strcmp(subcommand, "--file")){

        get_address(real_address);
        if (access(real_address, F_OK) != 0) {
            invalid_input();
            wprintw(command_part , "no such file or directory\n");
            return;
        }


        FILE * file;
        file = fopen(real_address, "r+");
        char tmp ='\0';
        int j = 0;
        for (; 1 ; ++j) {
            tmp= fgetc(file);
            if(tmp == EOF)break;
            wprintw(command_part , "%c" , tmp);
        }
        fclose(file);

    } else {
        invalid_input();
        wprintw(command_part , "invalid arguments for createfile\n");
    }
}

void remove_or_copy_or_cutstr(int specifier){  // 0 for remove 1 for copy 2 for cut
    char subcommand[10];
    char first , direction ;
    char real_address[50] = "../";
    int size ,dir;
    int line , pos;
    for (int i = 0; i < 3; ++i) {
        fscanf(command_file , " %c" , &first);
        if(first != '-'){
            invalid_input();
            wprintw(command_part , "invalid arguments for insertstr\n");
            return;
        }
        fscanf(command_file , "%s", subcommand);

        if (!strcmp(subcommand, "-file")) {
            get_address(real_address);
            if (access(real_address, F_OK) != 0) {
                invalid_input();
                wprintw(command_part , "no such file or directory\n");
                return;
            }

        } else if (!strcmp(subcommand, "-size")) {
            fscanf(command_file , "%d" ,&size);
        } else if (!strcmp(subcommand, "-pos")) {
            fscanf(command_file , " %d,%d" , &line,&pos);
        }else {
            invalid_input();
            wprintw(command_part , "invalid arguments for insertstr\n");
        }

    }
    fscanf(command_file , " -%c" , &direction);
    create_last_version(real_address);
    if(specifier == 0)
        remove_str_fromfile(real_address, size, line , pos , direction);
    else if(specifier == 1)
        copy_str_fromfile(real_address, size, line , pos , direction);
    else{
        copy_str_fromfile(real_address, size, line , pos , direction);
        remove_str_fromfile(real_address, size, line , pos , direction);
    }

    wprintw(command_part , "Done\n");

}

void pastestr(){
    char subcommand[10];
    char tmp ;
    char real_address[50] = "../";
    int line , pos;
    for (int i = 0; i < 2 ; ++i) {

        fscanf(command_file , " %c" , &tmp);
        if(tmp != '-'){
            invalid_input();
            wprintw(command_part , "invalid arguments for insertstr\n");
            return;
        }
        fscanf(command_file , "%s", subcommand);

        if (!strcmp(subcommand, "-file")) {
            get_address(real_address);
            if (access(real_address, F_OK) != 0) {
                invalid_input();
                wprintw(command_part , "no such file or directory\n");
                return;
            }

        }else if (!strcmp(subcommand, "-pos")) {
            fscanf(command_file , " %d,%d" , &line,&pos);
        }
        else {
            invalid_input();
            wprintw(command_part , "invalid arguments for insertstr\n");
        }
    }
    char *saved_data = create_tmp_file("./clipboard.txt");
    saved_data[strlen(saved_data) -1] = '\0';
    create_last_version(real_address);
    insert_str_infile(real_address , saved_data , line , pos);

    wprintw(command_part , "Done\n");

}
char * strrstr(char *string, char *find, ssize_t len , int size)
{
    //I see the find in string when i print it
    //wprintw(command_part , "%s", string);
    char *cp;
    for (cp = string + len - size; cp >= string; cp--)
    {
        if (strncmp(cp, find,size) == 0)
            return cp;
    }
    return NULL;
}
int word_counter(char * tmp , int index){
    int counter = 0;
    for (int i = 0; i <= index ; ++i) {
        if(tmp[i] == ' ')
            counter ++;
    }
    return counter+1;
}
void find_by_chars(char input_data[] , char real_address[] , int specifier ,int byword_flag ,  int size , int is_replace , char to_be_replaced[]){
    char * tmp = create_tmp_file(real_address);
   // create_last_version(real_address);
    char * star_pos;
    int counter = 0 , actual_pos = 0;
    int ans[50] = {0} , eo_ans[50] = {0} ;
   if( ( star_pos = strstr(input_data , "\a") )!= NULL){
       if(*(star_pos - 1) != ' ' && *(star_pos - 1) != NULL){
           char * first_part = strsep(&input_data , "\a");
           char * second_part = strsep(&input_data , "\a");
           char *find ;
           int i = 0;
           do{
               if((find = strstr(tmp+i, first_part)) == NULL)
                   break;
               char *last_char = find;
               last_char += strlen(first_part);
               for (; *(last_char) != ' ' &&  *(last_char) != EOF; last_char++) { }

               if (!strncmp(last_char, second_part, strlen(second_part))) {
                   counter ++;
                   ans[counter] = strlen(tmp) - strlen(find);
                   i =  strlen(tmp) - strlen(last_char) - 1;
                   if(byword_flag == 1){
                       ans[counter] = word_counter(tmp , ans[counter]);
                   }
                   eo_ans[counter] = i;
                   if(specifier == 0 || specifier == 3) {
                       if(!is_replace)
                           wprintw(command_part , "%d->%d\n", ans[counter] , i);
                       else{
                           if(counter == 1){
                               actual_pos = i - ans[counter] + 1 - strlen(to_be_replaced);
                               remove_str_fromfile(real_address, i - ans[counter]+1, 1, ans[counter], 'f');
                               insert_str_infile(real_address, to_be_replaced, 1, ans[counter]);
                           } else {
                               remove_str_fromfile(real_address, i - ans[counter]+1 , 1, ans[counter] - actual_pos, 'f');
                               insert_str_infile(real_address, to_be_replaced, 1, ans[counter] - actual_pos);
                               actual_pos += (i - ans[counter] + 1 - strlen(to_be_replaced));
                           }
                       }
                       if(specifier == 0){ return; }
                   }
               } else {
                   if(*(last_char + 1) == EOF){
                       break;
                   }
               }
               i++;
           } while (1);

       } else if(*(star_pos + 1) != ' ' && *(star_pos + 1) != NULL){
           char * first_part = strsep(&input_data , "\a");
           char * second_part = strsep(&input_data , "\a");
           char *find  , *first_appearance ;
           int i = 0;
           do{
               if((find = strstr(tmp+i, second_part)) == NULL)
                   break;
               int j = 0;
               for (; find[j] != ' ' ; ++j) {}
               if((first_appearance = strrstr(find, second_part , j+1 , strlen(second_part))) != NULL) {
                   find = first_appearance;
               }

               char *last_char = find;

               for (; *(last_char) != ' ' &&  *(last_char) != EOF; last_char--) {}
               last_char -= strlen(first_part)-1;
               if (!strncmp(last_char, first_part, strlen(first_part))) {
                   if(*(last_char - 1) == EOF)
                       last_char = tmp;
                   counter ++;
                   ans[counter] = strlen(tmp) - strlen(last_char);

                   i =  strlen(tmp) - strlen(find) + strlen(second_part) -1 ;
                   if(byword_flag == 1){
                       ans[counter] = word_counter(tmp , ans[counter]);
                   }
                   eo_ans[counter] = i;
                   if(specifier == 0 || specifier == 3){
                       if(!is_replace)
                           wprintw(command_part , "%d->%d\n", ans[counter] , i);
                       else{
                           if(counter == 1){
                               actual_pos = i - ans[counter] + 1 - strlen(to_be_replaced);
                               remove_str_fromfile(real_address, i - ans[counter]+1, 1, ans[counter], 'f');
                               insert_str_infile(real_address, to_be_replaced, 1, ans[counter]);
                           } else {
                               remove_str_fromfile(real_address, i - ans[counter]+1 , 1, ans[counter] - actual_pos, 'f');
                               insert_str_infile(real_address, to_be_replaced, 1, ans[counter] - actual_pos);
                               actual_pos += (i - ans[counter] + 1 - strlen(to_be_replaced));
                           }
                       }
                       if(specifier == 0){ return; }
                   }
                   //return;
               } else {
                   if(*(last_char + 1) == EOF){
                       break;
                   }
               }
               i++;
           } while (i < strlen(tmp) - 1);

       }
   } else{
       int i = 0;
       char * find ;
       do{
           if((find = strstr(tmp+i, input_data)) == NULL)
               break;

           counter ++;
           ans[counter] = strlen(tmp) - strlen(find);
           i =  strlen(tmp) - strlen(find) + strlen(input_data)-1;
           if(byword_flag == 1){
               ans[counter] = word_counter(tmp , ans[counter]);
           }
           eo_ans[counter] = i;
           if(specifier == 0 || specifier == 3) {
               if(!is_replace)
                   wprintw(command_part , "%d->%d\n", ans[counter] , i);
               else{
                   if(counter == 1){
                       actual_pos = i - ans[counter] + 1 - strlen(to_be_replaced);
                       remove_str_fromfile(real_address, i - ans[counter]+1, 1, ans[counter], 'f');
                       insert_str_infile(real_address, to_be_replaced, 1, ans[counter]);
                   } else {
                       remove_str_fromfile(real_address, i - ans[counter]+1 , 1, ans[counter] - actual_pos, 'f');
                       insert_str_infile(real_address, to_be_replaced, 1, ans[counter] - actual_pos);
                       actual_pos += (i - ans[counter] + 1 - strlen(to_be_replaced));
                   }
               }
               if (specifier == 0) { return; }
           }
           i++;
       } while (1);
   }
    if(specifier == 1) {
        wprintw(command_part , "%d\n", counter);
        return;
    } else if(specifier == 2 && counter >= size){
        if(!is_replace)
            wprintw(command_part , "%d->%d\n", ans[size] , eo_ans[size]);
        else{
            remove_str_fromfile(real_address, eo_ans[size] - ans[size]+1, 1, ans[size], 'f');
            insert_str_infile(real_address, to_be_replaced, 1, ans[size]);
        }
        return;
    } else if( specifier == 3 && counter != 0 ){
        return;
    }
    wprintw(command_part , "-1\n");
}


void find(){
    char subcommand[10];
    char options[30];
    char tmp ;
    int word_flag = 0, options_specifier = 0 , at_size = 0;
    char real_address[50] = "../";
    char input_str[100];
    for (int i = 0; i < 2; ++i) {

        fscanf(command_file , " %c" , &tmp);
        if(tmp != '-'){
            invalid_input();
            wprintw(command_part , "invalid arguments for find\n");
            return;
        }
        fscanf(command_file , "%s", subcommand);

        if (!strcmp(subcommand, "-file")) {
            get_address(real_address);
            if (access(real_address, F_OK) != 0) {
                invalid_input();
                wprintw(command_part , "no such file or directory\n");
                return;
            }

        } else if (!strcmp(subcommand, "-str")) {
            get_str(input_str );
        }else {
            invalid_input();
            wprintw(command_part , "invalid arguments for find\n");
        }
    }
    fscanf(command_file , "%[^'\n']s" , options);

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
                wprintw(command_part , "logical error\n");
                return;
            }
        } else if(token[0]=='a' && token[1] == 't'){
            if(options_specifier == 0)
                options_specifier = 2;
            else {
                invalid_input();
                wprintw(command_part , "logical error\n");
                return;
            }
            sscanf(token, "at %d" , &at_size);
        }else if(!strcmp(token, "all")) {
            if(options_specifier == 0)
                options_specifier = 3;
            else {
                invalid_input();
                wprintw(command_part , "logical error\n");
                return;
            }
        }else if(!strcmp(token, "byword"))
            word_flag =1;

        token = strtok(NULL, s);
    }
    create_last_version(real_address);
    find_by_chars(input_str , real_address , options_specifier , word_flag, at_size , 0 , NULL);


}

void replace(){
    char subcommand[10];
    char options[30];
    char tmp ;
    int options_specifier = 0 , at_size = 0;
    char real_address[50] = "../";
    char input_str[100] , input_str2[100];
    for (int i = 0; i < 3; ++i) {

        fscanf(command_file , " %c" , &tmp);
        if(tmp != '-'){
            invalid_input();
            wprintw(command_part , "invalid arguments for find\n");
            return;
        }
        fscanf(command_file , "%s", subcommand);

        if (!strcmp(subcommand, "-file")) {
            get_address(real_address);
            if (access(real_address, F_OK) != 0) {
                invalid_input();
                wprintw(command_part , "no such file or directory\n");
                return;
            }

        } else if (!strcmp(subcommand, "-str1")) {
            get_str(input_str);
        }else if (!strcmp(subcommand, "-str2")) {
            get_str(input_str2);
        }
        else {
            invalid_input();
            wprintw(command_part , "invalid arguments for replace\n");
            return;
        }
    }
    fscanf(command_file , "%[^'\n']s" , options);

    const char s[2] = "-";
    char *token;
    token = strtok(options, s);
    while( token != NULL ) {
        if(token[strlen(token) -  1] == ' ')
            token[strlen(token) -  1] = '\0';
        if(token[0]=='a' && token[1] == 't'){
            if(options_specifier == 0)
                options_specifier = 2;
            else {
                invalid_input();
                wprintw(command_part , "logical error\n");
                return;
            }
            sscanf(token, "at %d" , &at_size);
        }else if(!strcmp(token, "all")) {
            if(options_specifier == 0)
                options_specifier = 3;
            else {
                invalid_input();
                wprintw(command_part , "logical error\n");
                return;
            }
        }
        token = strtok(NULL, s);
    }
    create_last_version(real_address);
    find_by_chars(input_str , real_address , options_specifier , 0 , at_size , 1 , input_str2);
    wprintw(command_part , "Done\n");
}
void search_line_by_line(int specifier , char all_address[20][50] , char  input_data[]){
    int counter = 0;
    for (int i = 0; i < 20 && all_address[i][0] != '\0'; ++i) {
        char *file1 = create_tmp_file(all_address[i]);
        create_last_version(all_address[i]);

        file1[strlen(file1) - 1] = '\0';

        if(strstr(file1 , input_data) != NULL) {
            char *file_name = strrchr(all_address[i] , '/');
            if(specifier != 1)
                wprintw(command_part , "/%s:\n", file_name);
            if(specifier == 2) {
                counter = 1;
                continue;
            }
        }
        else
            continue;
        const char s[2] = "\n\0";
        char *file1_token;
        file1_token = strsep(&file1, s);

        while (file1_token != NULL) {
            if (strstr(file1_token, input_data) != NULL) {
                if(specifier != 1)
                    wprintw(command_part , "%s\n", file1_token);
                counter++;
            }
            file1_token = strsep(&file1, s);
        }
    }
    if(counter == 0 && specifier != 1){
        wprintw(command_part , "not found\n");
    }
    if(specifier == 1)
        wprintw(command_part , "%d\n" , counter);
}
void grep(){
    char subcommand[10];
    char options[10];
    char tmp , tmp2 ;
    int word_flag = 0, options_specifier = 0 , at_size = 0;
    char real_address[50] = "../";
    char all_address[20][50] ={'\0'};
    char input_str[100];
    fgetc(command_file);
    tmp = fgetc(command_file);
    if(tmp != '-'){
        invalid_input();
        return;
    }
    tmp2 = fgetc(command_file);
    if(tmp2 == '-'){
        fscanf(command_file , "%s" , subcommand);
        get_str(input_str);
    }else if(tmp2 == 'c'){
        options_specifier = 1;
        fscanf(command_file , " %s" , subcommand);
        get_str(input_str);
    }else if (tmp2 == 'l'){
        options_specifier = 2;
        fscanf(command_file , " %s" , subcommand);
        get_str(input_str);
    }
    if (!strcmp(subcommand, "-c")){
        options_specifier = 1;
    } else if (!strcmp(subcommand, "-l")){
        options_specifier = 2;
    }

        fscanf(command_file , " %s", subcommand);

        if (!strcmp(subcommand, "--file")) {
            int i = 0;
            while (get_address(real_address) == 1) {
                if (access(real_address, F_OK) != 0) {
                    invalid_input();
                    wprintw(command_part , "no such file or directory\n");

                }
                strcpy(all_address[i], real_address);
                i++;
                for (int j = 3; j < strlen(real_address) ; ++j) {
                    real_address[j] = '\0';
                }
            }

        } else{
            invalid_input();
            return;
        }
    search_line_by_line(options_specifier, all_address , input_str );

}
void undo(){
    char subcommand[10];
    char first ;
    char real_address[50] = "../";

        fscanf(command_file , " %c" , &first);
        if(first != '-'){
            invalid_input();
            wprintw(command_part , "invalid arguments for insertstr\n");
            return;
        }
        fscanf(command_file , "%s", subcommand);

        if (!strcmp(subcommand, "-file")) {
            get_address(real_address);
            if (access(real_address, F_OK) != 0) {
                invalid_input();
                wprintw(command_part , "no such file or directory\n");
                return;
            }
        }else {
            invalid_input();
            wprintw(command_part , "invalid arguments for insertstr\n");
        }
    char * version_address = last_version_address(real_address);
    char * last_data = create_tmp_file(version_address);
    create_last_version(version_address);
    create_tmp_file(real_address);
    create_last_version(real_address);
    last_data[strlen(last_data) - 1] = '\0';
    FILE *file = fopen(real_address , "w");
    fputs(last_data , file);
    fclose(file);
    char * wrong_file = last_version_address(version_address);
    remove(wrong_file);
    wprintw(command_part , "Done!\n");
}
void compare_line_by_line(int specifier , char address1[] , char  address2[]){
    char * file1 = create_tmp_file(address1);
    create_last_version(address1);
    file1[strlen(file1) -1] = '\0';
    char * file2 = create_tmp_file(address2);
    create_last_version(address2);
    file2[strlen(file2) -1] = '\0';
    const char s[2] = "\n\0";
    char * file1_token;
    char * file2_token;
    file1_token = strsep(&file1, s);
    file2_token = strsep(&file2 , s);
    for (int i = 1; file1_token != NULL && file2_token != NULL ; i++) {
        if (strcmp(file1_token, file2_token)){
            wprintw(command_part , "######### line %d #########\n%s\n%s\n" , i , file1_token, file2_token );
        }
        file1_token = strsep(&file1, s);
        file2_token = strsep(&file2, s);
    }
    if(file1_token != NULL){
        wprintw(command_part , "!!!!!!!!! first file is longer !!!!!!!!!\n");
        while (file1_token != NULL){
            wprintw(command_part , "%s\n" , file1_token);
            file1_token = strsep(&file1 , s);
        }
    }
    if(file2_token != NULL){
        wprintw(command_part , "!!!!!!!!! second file is longer !!!!!!!!!\n");
        while (file2_token != NULL){
            wprintw(command_part , "%s\n" , file2_token);
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

    fscanf(command_file , "%s", subcommand);

    if (!strcmp(subcommand, "--files")) {
        get_address(real_address1);
        get_address(real_address2);
        if (access(real_address1, F_OK) != 0 || access(real_address2, F_OK) != 0 ) {
            invalid_input();
            wprintw(command_part , "no such file or directory\n");
            return;
        }
    }else {
        invalid_input();
        wprintw(command_part , "invalid arguments for compare\n");
    }
    fscanf(command_file , "%[^'\n']s" , options);

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
                wprintw(command_part , "logical error\n");
                return;
            }
        }else if (!strcmp(token, "I")) {
            if (options_specifier == 0)
                options_specifier = 2;
            else {
                invalid_input();
                wprintw(command_part , "logical error\n");
                return;
            }
        }

        token = strtok(NULL, s);
    }
    compare_line_by_line(options_specifier , real_address1, real_address2);
}
void indent(char *real_address){
    char *tmp = create_tmp_file(real_address);
    create_last_version(real_address);
    tmp[strlen(tmp) -1 ]= '\0';
    int indent_counter = 0;
    for (int i = 0; tmp[i] != '\0' ; ++i) {
        if(indent_counter < 0){
            wprintw(command_part , "wrong system!\n");
            return;
        }
        if(tmp[i] == '\n'){
            int j = i + 1 ;
            for (; tmp[j] == ' '  ; j++) { }
            if(tmp[j]!= '}'){
                memmove(&tmp[i + indent_counter * 4 + 1], &tmp[j], strlen(tmp) - i + 1);
                memcpy(&tmp[i + 1], "                              ", indent_counter * 4);
            }

        }
        if(tmp[i] == '{') {
            indent_counter++;
            int j = i - 1;
            for (; tmp[j] == ' ' && j >= 0 ; j--) { }
            j++;
            if(tmp[j-1] != '\n' && j >= 1) {
                memmove(&tmp[j + 1], &tmp[i], strlen(tmp) - i + 1);
                memcpy(&tmp[j], " ", 1);
                i = j + 1 ;
            }

            j = i + 1 ;
            int k = i + 1;
            for (; tmp[k] == ' '  ; k++) { }
            if(tmp[k] != '\n'){
                memmove(&tmp[j+4*indent_counter+1] ,&tmp[j] , strlen(tmp) - i + 1);
                memcpy(&tmp[j+1] ,  "                                                        " , 4*indent_counter);
                tmp[j] = '\n';
                i = j - 1;
            }

        }
        if(tmp[i] == '}') {
            indent_counter--;
            int flag = 0;
            if (tmp[i+1] == '\0' || indent_counter == 0){
                flag = 1;
            }

            int j = i - 1;
            for (; tmp[j] == ' ' && j >= 0 ; j--) { }
            j++;
            if(tmp[j-1] != '\n') {
                memmove(&tmp[j+4*indent_counter+1] ,&tmp[i] , strlen(tmp) - i + 1);
                memcpy(&tmp[j+1] ,  "                                                        " , 4*indent_counter);
                tmp[j] = '\n';
                i = j +(indent_counter)*4+1;
            }else{
                memmove(&tmp[j+(indent_counter)*4] ,&tmp[i] , strlen(tmp) - i + 1 );
                memcpy(&tmp[j+1] , "                                                  " , (indent_counter)*4 - 1 + flag);
                i = j+(indent_counter)*4 ;
            }

            j = i + 1 ;
            int k= i + 1;
            for (; tmp[k] == ' '  ; k++) { }
            if(tmp[k] != '\n'){
                memmove(&tmp[j+4*indent_counter+1] ,&tmp[j] , strlen(tmp) - j + 1);
                memcpy(&tmp[j+1] ,  "                                                        " , 4*indent_counter);
                tmp[j] = '\n';
                i = j-1;
            }
        }
    }
    if(indent_counter != 0 ){
        wprintw(command_part , "wrong system!\n");
        return;
    }
    FILE *file = fopen(real_address , "w");
    fputs(tmp , file);
    fclose(file);
    wprintw(command_part , "Done\n");
}
void autoindent(){
    char subcommand[10];
    char real_address[50] = "../";
    fscanf(command_file , "%s", subcommand);

    if (!strcmp(subcommand, "--file")) {
        get_address(real_address);
        if (access(real_address, F_OK) != 0) {
            invalid_input();
            wprintw(command_part , "no such file or directory\n");
            return;
        }
    }else {
        invalid_input();
        wprintw(command_part , "invalid arguments for autoindent\n");
    }
    indent(real_address);

}
void tree(){
    char subcommand[10];
    int depth;
    char real_address[50] = "../";
    fscanf(command_file , "%d" , &depth);
    if(depth<-1){
        invalid_input();
        wprintw(command_part , "invalid depth\n");
    }
    fscanf(command_file , "%s", subcommand);

    if (!strcmp(subcommand, "--dir")) {
        get_address(real_address);
        if (access(real_address, F_OK) != 0) {
            invalid_input();
            wprintw(command_part , "no such file or directory\n");
            return;
        }
    }else {
        invalid_input();
        wprintw(command_part , "invalid arguments for tree\n");
    }
    list(real_address , 0 , depth);
}

int file_lines = 1;
//int width  ;
int height = 30;
int width;
int mode = 0;
void mode_write(){
    int o_x , o_y;
    getyx(stdscr, o_y , o_x);
    init_pair(1 , COLOR_RED , COLOR_GREEN);
    wattron( status , COLOR_PAIR(1));
    wmove(status , 0 , 0 );
    if(mode == 0) {
        waddstr( status , "NORMALL");
    }else if(mode == 1) {
        waddstr( status ,"VISUAL ");
    }else if(mode == 2) {
        waddstr( status ,"INSERT ");
    }
    wattroff( status , COLOR_PAIR(1));
    wrefresh(status);
    move( o_y , o_x);
    refresh();
    //waddstr(text_part , "++");

    wrefresh(text_part);
}
void write_nums(){
    init_pair(2 , COLOR_CYAN , COLOR_BLACK);
    wattron(num , COLOR_PAIR(2));
    for (int i = 0; i < height; ++i) {
        wmove(num , i , 0);
        if(i + 1 <= file_lines)
            wprintw(num, "%d", i + 1);
        else
            wprintw(num, "  ");
    }
    wattroff(num , COLOR_PAIR(2));
    wrefresh(num);
}

char * save(int specifier){
    char *real_address = calloc(50 , sizeof(char ));
    wmove(status , 0 , 11);
    for (int i = 0; 1 ; ++i) {
        char tmp = winch(status) & A_CHARTEXT;
        wmove(status , 0 , 12+i);
        if( tmp != ' ')
            real_address[i] = tmp;
        else {
            if(i == 0) {
                real_address[0] = '.' ; real_address[1] = '.' ;
                char file_address[50];
                mvwprintw(command_part, 1 , 0 , "pleas enter an address to save\n");
                wrefresh(command_part);
                wgetstr(command_part , file_address);
                strcat(real_address , file_address);
                mvwaddstr(status , 0 , 11 , real_address);
            }
            break;
        }
    }
    if(specifier){
        char name[50];
        get_str(name);
        char * filename = strrchr(real_address , '/');
        for (int i = 1 ;*(filename+i) != '\0'; ++i) {
            *(filename+i) ='\0';
        }
        strcat(real_address , name);
    }

    FILE * file = fopen(real_address , "w");
    for (int j = 0; j < file_lines ; ++j) {
        char line_val[100];
        int i = 0;
        for (; (mvwinch(new_line_position, j ,  i) & A_CHARTEXT) != 'n' ; ++i) {}
        wmove(text_part, j, 0);
        winnstr(text_part , line_val , i);
        fputs(line_val, file);
        fputc('\n' , file);
    }
    fclose(file);
    wprintw(command_part ,"Done\n");
    wrefresh(command_part);
    mvwaddch(status , 0 , 9 , ' ');
    wmove(text_part , 0 , 0 );
    wrefresh(command_part);
    return real_address;
}
void open(int specifier , char * current_address){

    char subcommand[10];
    char real_address[50] = "../";
    if(specifier){
        strcpy(real_address , current_address);
        real_address[strlen(current_address)] = '\0';
    }else{
        fscanf(command_file, "%s", subcommand);
        if (!strcmp(subcommand, "--file")) {

            get_address(real_address);
            if (access(real_address, F_OK) != 0) {
                invalid_input();
                wprintw(command_part, "no such file or directory\n");
                return;
            }
            //mvwaddch(status , 0 , 9 , ' ');
            wmove(status, 0, 9);
            if ((winch(status) & A_CHARTEXT) == '+') {
                mvwprintw(command_part, 1, 0, "your file has not been saved & ");
                wrefresh(command_part);
                save(0);
                wmove(status, 0, 9);
            }

        } else {
            invalid_input();
            wprintw(command_part, "invalid arguments for open\n");
            return;
        }
    }
    wclear(text_part);
    wclear(new_line_position);
    wclrtoeol(status);
    wmove(status , 0 , 11);
    waddstr(status , real_address);
    wrefresh(status);
    char* tmp = create_tmp_file(real_address);
    create_last_version(real_address);
    file_lines = 0;
    if(strlen(tmp ) == 1){
        mvwaddch(new_line_position , 0 , 0 , 'n' );
        file_lines++;
    }
    tmp[strlen(tmp) - 1] = '\0';
    for (int i = 0 , j= 0 ; tmp[i] != '\0' ; ++i) {
        if(tmp[i] == '\n') {
            mvwaddch(new_line_position , file_lines , i - j - (1*(file_lines != 0)) , 'n' );
            j = i ;
            file_lines++;
        }
    }
    write_nums();
    wmove(text_part , 0 , 0);
    waddstr( text_part , tmp);
    wmove(text_part , 0 , 0);
    wrefresh(text_part);
}

int find_in_scr(char * to_be_searched , int find_pos[10][2] ){
    int k = 0;
    for (int j = 0; j < file_lines ; ++j) {
        char * finded;
        char line_val[100];
        int i = 0;
        for (; (mvwinch(new_line_position, j ,  i) & A_CHARTEXT) != 'n' ; ++i) {}
        wmove(text_part, j, 0);
        winnstr(text_part , line_val , i);
        if((finded = strstr(line_val , to_be_searched)) != NULL){
            wrefresh(command_part);
            find_pos[k][0]= j;
            find_pos[k][1] = strlen(line_val) - strlen(finded);
            mvwchgat(text_part , j , strlen(line_val) - strlen(finded) , strlen(to_be_searched) , A_STANDOUT , 0 , NULL );
            wrefresh(text_part);
            k++;
        }
    }
    return k;
}
void del_all_attrs(){
    int y = 0 , x = 0;
    getyx(text_part , y , x);
    for(int i = 0 ; i < file_lines ; i++){
        wmove(text_part, i, 0);
        wchgat(text_part , -1, A_NORMAL, 0, NULL);
    }
    wmove(text_part , y , x);
    wrefresh(text_part);
}
void movement(char check_mode , int specifier , int* curr_x){
    int x, y;
    if (check_mode == (int) 'a') {
        getyx(text_part, y, x);
        //x--;
        if(x > 0){
            if(specifier){
                curr_x[0] --;
                if((mvwinch(text_part , y , x - 1) & A_ATTRIBUTES) == A_STANDOUT) {
                    wmove(text_part , y  , x);
                    wchgat(text_part , 1, A_NORMAL, 0, NULL);

                }else if((mvwinch(text_part , y , x ) & A_ATTRIBUTES) != A_STANDOUT)
                    wchgat(text_part, 1 , A_STANDOUT , 0 , NULL);
            }
            wmove(text_part, y, x - 1);
            wrefresh(text_part);
        }
//        if( (winch(new_line_position) & A_CHARTEXT )!= 'n') {
//            wrefresh(text_part);
//        } else{
//            wmove(text_part, y, x);
//            wrefresh(text_part);
//        }
    } else if (check_mode == (int) 'd') {
        getyx(text_part, y, x);
        //x++;
        if( (mvwinch(new_line_position , y , x) & A_CHARTEXT )!= 'n') {
            if(specifier){
                curr_x[0] ++;
                if((mvwinch(text_part , y , x + 1) & A_ATTRIBUTES) == A_STANDOUT) {
                    wmove(text_part , y  , x);
                    wchgat(text_part , 1, A_NORMAL, 0, NULL);
                }else if((mvwinch(text_part, y , x ) & A_ATTRIBUTES) != A_STANDOUT)
                    wchgat(text_part, 1 , A_STANDOUT , 0 , NULL);
            }
            wmove(text_part, y, x+1);
            wrefresh(text_part);
        }
    } else if (check_mode == (int) 's') {
        getyx(text_part, y, x);
        if((y+1)<file_lines) {
            //wmove(text_part, y+1, x);
            int i = 0;
            for (; (mvwinch(new_line_position, y + 1, x - i) & A_CHARTEXT) == ' ' && x != i; ++i) {}
            if(x == i && (mvwinch(new_line_position, y + 1, x - i) & A_CHARTEXT) == ' ' )
                wmove(text_part, y+1, x);
            else
                wmove(text_part , y + 1 ,x - i );
        }
        wrefresh(text_part);

    } else if (check_mode == (int) 'w') {
        getyx(text_part, y, x);
        if(y > 0){
            int i = 0;
            for (; (mvwinch(new_line_position, y - 1, x - i) & A_CHARTEXT) == ' ' && x != i; ++i) {}
            if (x == i && (mvwinch(new_line_position, y - 1, x - i) & A_CHARTEXT) == ' ')
                wmove(text_part, y - 1, x);
            else
                wmove(text_part, y - 1, x - i);

            wrefresh(text_part);
        }
    }
}
void get_command() {
    char command[30];
    wgetstr(command_part , command_line);
    command_line[strlen(command_line)] = '\n';
    command_file = fopen("../.command.txt"  , "w+");
    fputs(command_line , command_file);
    fclose(command_file);
    command_file = fopen("../.command.txt"  , "r");
    fscanf(command_file , "%s", command);

    if (!strcmp(command, "exit"))
    {endwin();}
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
        replace();
    } else if (!strcmp(command, "grep")) {
        grep();
    } else if (!strcmp(command, "undo")) {
        undo();
    } else if (!strcmp(command, "autoindent")) {
        autoindent();
    } else if (!strcmp(command, "compare")) {
        compare();
    } else if (!strcmp(command, "tree")) {
        tree();
    } else if (!strcmp(command, "save")) {
        save(0);
    } else if (!strcmp(command, "saveas")) {
        save(1);
    }  else if (!strcmp(command, "open")) {
        open(0 , NULL);
    } else {
        invalid_input();
        wprintw(command_part , "invalid command\n");
    }
    wrefresh(command_part);
    wrefresh(status);
    fclose(command_file);

}
int main(){
    int y = 1 , x = 2;
    int find_pos[10][2] , k , next_k = 0;
    width = COLS ;
    char * ptr;
    char * ptr2;
    char * undo_file;
    char clipboard[1000] ;
    initscr();
    cbreak();
    scrollok(stdscr, TRUE);
    refresh();
    system("/bin/stty raw");
    start_color();
    text_part = newwin(height, width , y , x );
    last_text_part = newwin(height, width , y , x);
    command_part = newwin(5 , width , height + 2 , 0 );
    new_line_position = newwin(height, width , y , x );
    last_new_line_position = newwin(height, width , y , x );
    status = newwin(1 , width , height + 1 , 0);
    num = newwin(height , 2 , 1 , 0);
    waddch( new_line_position , 'n');
    keypad(text_part , TRUE);
    write_nums();

    move(y, x);
    refresh();
    mode_write();
    noecho();
    while (1){

        int check_mode = getch();

        if (check_mode == (int) 'e') {
            endwin();
            break;
        }else if(check_mode == (int) 'n' && k != 0){
            next_k ++;
            if(next_k <= k){
                wmove(text_part, find_pos[next_k][0], find_pos[next_k][1]);
                wrefresh(text_part);
            }

        }else if(check_mode == (int) 'u'){
//            copywin(last_text_part , text_part , 0 , 0 , 0 , 0 ,height , width , FALSE);
//            copywin(last_new_line_position ,new_line_position, 0 , 0 , 0 , 0 ,height , width , FALSE);
//            wrefresh(text_part);
            ptr = NULL;
            ptr  = save(0);
            ptr2 = last_version_address(ptr);
            char * tmp = create_tmp_file(ptr2);
            create_last_version(ptr2);
            tmp[strlen(tmp) - 1 ] = '\0';
            FILE * current = fopen(ptr , "w");
            fputs(tmp , current);
            fclose(current);
            open(1 , ptr);
            wclear(command_part);
            wrefresh(command_part);
            wmove(text_part , 0 , 0 );
            wrefresh(text_part);
        }else if(check_mode == (int) '='){
            ptr  = save(0);
            indent(ptr);
            open(1  , ptr);
        }else if(check_mode == (int) 'o'){
            wrefresh(new_line_position);
        }else if(check_mode == (int) '/'){
            getyx(text_part , y , x);
            find_pos[0][0] = y , find_pos[0][1] = x;
            echo();
            char to_be_searched[100];
            wmove(command_part , 0 , 0);
            wprintw( command_part , "/");
            wrefresh(command_part);
            wgetstr(command_part , to_be_searched);
            k = find_in_scr(to_be_searched , find_pos );
            wmove(text_part , find_pos[0][0] , find_pos[0][1]);
            wrefresh(text_part);
            noecho();
            wclear(command_part);
        }else if(check_mode == (int) 'p'){
            del_all_attrs();
            if(clipboard != NULL){
                getyx(text_part , y , x);
                winsstr(text_part, clipboard);
                mvwinsnstr( new_line_position , y , x , "                                                                                                                " , strlen(clipboard));
            }
            wrefresh(text_part);
        }else if (check_mode == (int) ':' ) {
            del_all_attrs();
            getyx(text_part , y , x);
            echo();
            wmove(command_part , 0 , 0);
            wprintw( command_part , ":");
            wrefresh(command_part);
            get_command();
            usleep(5000000);
            wclear(command_part);
            mode_write();
            wrefresh(command_part);
            noecho();
            wmove(text_part , y , x);
            wrefresh(text_part);
        }else if (check_mode == (int) 'i') {
            del_all_attrs();
            mvwaddch(status , 0 , 9 , '+');
            mode = 2;
            mode_write();
            //echo();
            while (1){

                int input = wgetch(text_part);
                if(input == 27 )
                    break;
                else if(input == (int) '\n' ) {
                    file_lines++;
                    getyx(text_part , y , x);
                    write_nums();
                    char prev_n[1000];
                    wmove(new_line_position , y , x);
                    winstr(new_line_position , prev_n);
                    wclrtoeol(new_line_position);
                    winsch(new_line_position , 'n');
                    mvwaddch(new_line_position, y , x + 1, '\n');
                    winsertln(new_line_position);
                    waddstr(new_line_position , prev_n);

                    char remaining[1000];
                    wmove(text_part , y , x);
                    winstr(text_part , remaining);
                    wclrtoeol(text_part);
                    waddch(text_part , '\n');
                    winsertln(text_part);
                    waddstr(text_part , remaining);
                    wmove(text_part, y+1 , 0 );
                }else{
                    getyx(text_part , y , x);
                    mvwinsch(new_line_position , y , x , ' ');
                    winsch(text_part , input);
                    wmove(text_part, y , x+1);
                }
                wrefresh(text_part);
            }
            wrefresh(text_part);
            mode = 0;
            mode_write();
        }else if (check_mode == (int) 'v') {
            del_all_attrs();
            int x_old , y_old;
            getyx(text_part , y_old , x_old);
            int curr_x[1];
            curr_x[0] = x_old;
            mode = 1;
            mode_write();
            wattron(text_part , A_STANDOUT);
            noecho();
            while (1){
                int input = wgetch(text_part);
                movement(input , 1 , curr_x);
                if(input == 27 ) {
                    del_all_attrs();
                    break;
                }else if(input == (int) 't' ) {
                    for (int i = x_old ; i <curr_x[0] ; i++) {
                        clipboard[i - x_old] = (mvwinch(text_part , y_old , x_old) & A_CHARTEXT);
                        wdelch(text_part);
                        mvwdelch(new_line_position , y_old , x_old);
                        clipboard[i - x_old + 1] = '\0';
                    }
                    del_all_attrs();
                    break;
                }else if(input == (int) 'c' ){
                    for (int i = x_old ; i <curr_x[0] ; i++) {
                        clipboard[i - x_old] = (mvwinch(text_part , y_old , i) & A_CHARTEXT);
                        clipboard[i - x_old + 1] = '\0';
                    }
//                    mvwaddstr(command_part, 0 , 0 , clipboard);
//                    wprintw(command_part , " %d %d"  , curr_x[0] , x_old);
//                    wrefresh(command_part);
                    del_all_attrs();
                    break;
                }
                wrefresh(text_part);
            }
            wrefresh(text_part);
            mode = 0;
            mode_write();
            wattroff(text_part , A_STANDOUT);
        }
        movement(check_mode , 0 , NULL);
        //refresh();
    }
    //get_command();
    return(0);
}