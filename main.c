
#include <stdio.h>
#include <stdlib.h>

int get_command(){

}

int main()
{
    char a[30];
        FILE * fp;

        fp = fopen ("../root/file.txt", "r+");
        fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);

        fclose(fp);
        fp = fopen("../root/file.txt","r+");
        fscanf(fp , "%s" , a);
        fclose(fp);

        printf("%s", a);



    return(0);

}