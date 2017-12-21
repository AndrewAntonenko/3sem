#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>


char **split(char *, int *);

char **readtxt(FILE *text);


int main(int argc, char *argv[]) {
    int words = 0, i = 0;
    FILE* text = fopen(argv[1], "r");
    char** ptrarr = readtxt(text);
    fclose(text);
    int amount = strtol(ptrarr[0], NULL, 10);
    pid_t id[amount];
    for (i = 0; i < amount; i++) {
        char **dictionary = split(ptrarr[i + 1], &words);
        int time = atoi(dictionary[words - 1]);
        dictionary[words - 1] = (char *) NULL;
        id[i] = fork();
        if (id[i] == 0) {
            sleep(time);
            printf("process %d, running new programm\n", i + 1);
            execvp(dictionary[0], dictionary);
            perror("exec failed");
            exit(-1);
        } else if (id[i] == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        free(dictionary);
    }
    for (i = 0; i < amount; i++)
        waitpid(id[i], 0, 0);
    free(ptrarr);
    return 0;
}


char **split(char *s, int *length) {
    int counter = 1, i = 0;
    char **head;
    char *point;
    errno = 0;
    if (!(head = (char **) calloc(1, sizeof(char *)))) {
        perror("Calloc error");
        exit(EXIT_FAILURE);
    }
    char *delim = "    ;,.:";
    point = strtok(s, delim);
    while (point != NULL) {
        if (i + 2 >= counter) {
            counter *= 2;
            if (!(head = (char **) realloc(head, counter * sizeof(char *)))) {
                printf("realloc: error: %s\n", strerror(errno));
                exit(-1);
            }
        }
        head[i] = point;
        i++;
        point = strtok(NULL, " ");
    }
    *length = i;
    head[i] = (char *) NULL;
    return head;
}

char **readtxt(FILE *text) {
    int i = 0, j = 1, count = 0;
    long int length = 0;
    char **ptrarr;
    char *filestring;
    errno = 0;
    if (fseek(text, 0, SEEK_END) == -1) {
        perror("fseek crush");
        exit(EXIT_FAILURE);
    }
    length = ftell(text);
    rewind(text);
    if (!(filestring = (char *) calloc(length + 1, sizeof(char)))) {
        perror("calloc error");
        exit(EXIT_FAILURE);
    }
    if ((fread(filestring, sizeof(char), length, text)) != length) {
        perror("read wrong");
        exit(EXIT_FAILURE);
    }
    filestring[length] = '\n';
    for (i = 0; i < length + 1; i++) {
        if (filestring[i] == '\n') {
            count++;
        }
    }
    filestring[length] = '\0';
    if (!(ptrarr = (char **) calloc(count, sizeof(char *)))) {
        perror("Calloc error: ");
        exit(EXIT_FAILURE);
    }
    ptrarr[0] = &(filestring[0]);
    for (i = 0; i < length; i++) {
        if (filestring[i] == '\n') {
            filestring[i] = '\0';
            ptrarr[j] = &(filestring[i + 1]);
            j++;
        }
    }
    return ptrarr;
}


