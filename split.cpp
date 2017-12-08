#include <iostream>
#include <cstring>

char *ReadFromFile(long *length, const char *PathIn) {
    FILE *FileIn;
    if ((FileIn = fopen(PathIn, "r")) == NULL) {
        perror("Error of opening file");
        exit(-1);
    }
    if (fseek(FileIn, 0, SEEK_END) != 0) {
        perror("Error of fseek");
        exit(1);
    }
    *length = ftell(FileIn);
    rewind(FileIn);

//    printf("length = %ld\n", *length);
    char *buf = (char *) calloc(*length, sizeof(char));
    if (fread(buf, sizeof(char), *length, FileIn) <= 0) {
        perror("Error of reading text");
        exit(2);
    }
    fclose(FileIn);
    return buf;
}

char **split(char *string, int *height, char *delim) {
    int counter = 1, i = 0;
    char **head;
    char *point;
    errno = 0;
    if (!(head = (char **) calloc(1, sizeof(char *)))) {
        perror("Calloc error");
        exit(EXIT_FAILURE);
    }
    point = strtok(string, delim);
    while (point != NULL) {
        if (i + 2 >= counter) {
            counter *= 2;
            if (!(head = (char **) realloc(head, counter * sizeof(char *)))) {
                printf("realloc: error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        head[i] = point;
        i++;
        point = strtok(NULL, " ");
    }
    *height = i;
    head[i] = (char *) NULL;
    return head;
}

int main(int argc, char **argv) {
    const char *FilePath = argv[2];
    long size = 0;
    char *string = ReadFromFile(&size, FilePath);
    char *tabs = (char *) " ,;";
    int length = 0;
    char **text = split(string, &length, tabs);
    for (int i = 0; i < length;) {
        printf("i = %d  %s \n", i, text[i]);
        i++;
    }
    free(string);
    free(text);
    return 0;
}
