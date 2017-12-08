#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>


char**	split(char*, int*);
char* readtxt(long* length, const char*);
/*Функция читает строку из файла */
char** doArr(char* buf, int* nStrings, long length);
int Exec(char** text, int nFunc, int nStrings);


int main(int argc, const char *argv[]) {
    const char *pathIn = argv[2];
    int nFunc = 0;
    long length = 0;
    char* buf = readtxt(&length, pathIn);
    printf("%s", buf);
    int nStrings = 0;int exit_code = 0;
    char** text = doArr(buf, &nStrings, length);
     if ((nFunc = atoi(text[0])) < 0) {
        perror("Less arguments, than expected ");
        exit(-1);
    }
    Exec(text, nFunc, nStrings);
    free(buf);
    free(text);
    return 0;
}


char** split (char* s, int * length ){
    int counter = 1, i = 0;
    char** head;
    char* point;
    errno = 0;
    if (!(head = (char**) calloc (1, sizeof(char*))))
    {
        perror("Calloc error");
        exit(EXIT_FAILURE);
    }
    char* delim = "    ;,.:";
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
    *length = i;
    head[i] = (char*) NULL;
    return head;
}

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

    printf("length = %ld\n", *length);
    char *buf = (char *) calloc(*length, sizeof(char));
    if ( fread(buf, sizeof(char), *length, fileIn) <= 0) {
        perror("Error of reading text");
        exit(2);
    }
    fclose(fileIn);
    return buf;
}

char** doArr(char* buf, int* nStrings, long length){
    int i = 0;
    for (i = 0; i < length - 1; i++) {
        if (buf[i] == '\n') {
            (*nStrings)++;

        }
    }
    printf("File consists of %d strings.\n\n\n", *nStrings);
    char **text = (char **) calloc(*nStrings + 1, sizeof(*text));
    if (text == NULL) {
        perror("Error of creating array 'text'");
        exit(-2);
    }
    int line = 1;
    text[0] = buf;
    for (i = 1; i < length - 1; i++) {
        if (buf[i] == '\n') {
            text[line] = buf + i + 1;
            buf[i] = '\0';
            line++;
        }
    }
    return text;
}

int Exec(char** text, int nFunc, int nStrings){
    int i =0, fatherSleep = 0, exit_code = -1;
    for (i = 1;i <= nFunc; i++) {
        char **pString = split(text[i], &nStrings);
        int time = atoi(pString[nStrings - 1]);
        pString[nStrings - 1] = (char *) NULL;
        if (fatherSleep < time)
            fatherSleep = time;
        pid_t cpid = fork();
        switch (cpid) {
            case -1:
                perror("fork failed");
                exit(EXIT_FAILURE);
            case 0:
                sleep(time);
                printf("process %d, running new programm\n", i);
                execvp(pString[0], pString);
                perror("exec failed");
                exit_code = 37;
                exit(exit_code);
            default:
                exit_code = 0;
                break;
        }
        free(pString);

        if (cpid) {
            int stat_val;
            pid_t child_pid = wait(&stat_val);
            if (WIFEXITED(stat_val))
                printf("Child exited with code %d \n", WEXITSTATUS(stat_val));
            else printf("Child terminated abnormally\n");
        }

    }
}