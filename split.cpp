#include <iostream>
#include <cstring>

char* readtxt( long* length, const char* PathIn){
    FILE* fileIn;
    if ((fileIn = fopen(PathIn, "r")) == NULL) {
        perror("Error of opening file");
        exit(-1);
    }
    if (fseek(fileIn, 0, SEEK_END) != 0) {
        perror("Error of fseek");
        exit(1);
    }
    *length = ftell(fileIn);
    rewind(fileIn);

//    printf("length = %ld\n", *length);
    char *buf = (char *) calloc(*length, sizeof(char));
    if ( fread(buf, sizeof(char), *length, fileIn) <= 0) {
        perror("Error of reading text");
        exit(2);
    }
    fclose(fileIn);
    return buf;
}
char** split (char* s, int * height, char* delim ){
    int counter = 1, i = 0;
    char** head;
    char* point;
    errno = 0;
    if (!(head = (char**) calloc (1, sizeof(char*))))
    {
        perror("Calloc error");
        exit(EXIT_FAILURE);
    }
    point = strtok (s, delim);
    while (point != NULL)
    {
        if (i + 2 >= counter)
        {
            counter *= 2;
            if (!(head = (char**) realloc (head,  counter * sizeof(char*))))
            {
                printf("realloc: error: %s\n", strerror(errno));
                exit(-1);
            }
        }
        head[i] = point;
        i++;
        point = strtok (NULL, " ");
    }
    *height = i;
    head[i] = (char*) NULL;
    return head;
}
int main(int argc, char** argv) {
    const char* path = argv[2];
    long size = 0;
    char* string = readtxt(&size, path );
    char* tabs =  (char*)" ,;";
    int length = 0;
    char** text = split(string, &length, tabs);
    for (int i = 0; i < length; ){
        printf("i = %d  %s \n", i, text[i]);
        i++;
    }
    free(string);
    free(text);
    return 0;
}
