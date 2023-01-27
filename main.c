
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
int get_address(char real_address[]){
    char file_address[50] ;
    char tmp;
    tmp = getchar();
    if(tmp == '\n')
        return 0;
    scanf("%c" , &tmp);

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
        return 0;
    }
    strcat(real_address , file_address);
    return 1;
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

char * create_tmp_file(char real_address[]){
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
char * strrstr(char *string, char *find, ssize_t len , int size)
{
    //I see the find in string when i print it
    //printf("%s", string);
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
                           printf("%d->%d\n", ans[counter] , i);
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
                           printf("%d->%d\n", ans[counter] , i);
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
                   printf("%d->%d\n", ans[counter] , i);
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
        printf("%d\n", counter);
        return;
    } else if(specifier == 2 && counter >= size){
        if(!is_replace)
            printf("%d->%d\n", ans[size] , eo_ans[size]);
        else{
            remove_str_fromfile(real_address, eo_ans[size] - ans[size]+1, 1, ans[size], 'f');
            insert_str_infile(real_address, to_be_replaced, 1, ans[size]);
        }
        return;
    } else if( specifier == 3 && counter != 0 ){
        return;
    }
    printf("-1\n");
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

        } else if (!strcmp(subcommand, "-str1")) {
            get_str(input_str);
        }else if (!strcmp(subcommand, "-str2")) {
            get_str(input_str2);
        }
        else {
            invalid_input();
            printf("invalid arguments for replace\n");
            return;
        }
    }
    scanf("%[^'\n']s" , options);

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
        }
        token = strtok(NULL, s);
    }

    find_by_chars(input_str , real_address , options_specifier , 0 , at_size , 1 , input_str2);
    printf("Done\n");
}
void search_line_by_line(int specifier , char all_address[20][50] , char  input_data[]){
    int counter = 0;
    for (int i = 0; i < 20 && all_address[i][0] != '\0'; ++i) {
        char *file1 = create_tmp_file(all_address[i]);

        file1[strlen(file1) - 1] = '\0';

        if(strstr(file1 , input_data) != NULL) {
            char *file_name = strrchr(all_address[i] , '/');
            if(specifier != 1)
                printf("/%s:\n", file_name);
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
                    printf("%s\n", file1_token);
                counter++;
            }
            file1_token = strsep(&file1, s);
        }
    }
    if(counter == 0 && specifier != 1){
        printf("not found\n");
    }
    if(specifier == 1)
        printf("%d\n" , counter);
}
void grep(){
    char subcommand[10];
    char options[10];
    char tmp , tmp2 ;
    int word_flag = 0, options_specifier = 0 , at_size = 0;
    char real_address[50] = "../";
    char all_address[20][50] ={'\0'};
    char input_str[100];
    getchar();
    tmp = getchar();
    if(tmp != '-'){
        invalid_input();
        return;
    }
    tmp2 = getchar();
    if(tmp2 == '-'){
        scanf("%s" , subcommand);
        get_str(input_str);
    }else if(tmp2 == 'c'){
        options_specifier = 1;
        scanf(" %s" , subcommand);
        get_str(input_str);
    }else if (tmp2 == 'l'){
        options_specifier = 2;
        scanf(" %s" , subcommand);
        get_str(input_str);
    }
    if (!strcmp(subcommand, "-c")){
        options_specifier = 1;
    } else if (!strcmp(subcommand, "-l")){
        options_specifier = 2;
    }

        scanf(" %s", subcommand);

        if (!strcmp(subcommand, "--file")) {
            int i = 0;
            while (get_address(real_address) == 1) {
                if (access(real_address, F_OK) != 0) {
                    invalid_input();
                    printf("no such file or directory\n");

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
        printf("!!!!!!!!! first file is longer !!!!!!!!!\n");
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
void indent(char *real_address){
    char *tmp = create_tmp_file(real_address);
    tmp[strlen(tmp) -1 ]= '\0';
    int indent_counter = 0;
    for (int i = 0; tmp[i] != '\0' ; ++i) {
        if(indent_counter < 0){
            printf("wrong system!\n");
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
        printf("wrong system!\n");
        return;
    }
    FILE *file = fopen(real_address , "w");
    fputs(tmp , file);
    fclose(file);
    printf("Done\n");
}
void autoindent(){
    char subcommand[10];
    char real_address[50] = "../";
    scanf("%s", subcommand);

    if (!strcmp(subcommand, "--file")) {
        get_address(real_address);
        if (access(real_address, F_OK) != 0) {
            invalid_input();
            printf("no such file or directory\n");
            return;
        }
    }else {
        invalid_input();
        printf("invalid arguments for autoindent\n");
    }
    indent(real_address);

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
        printf("invalid arguments for tree\n");
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