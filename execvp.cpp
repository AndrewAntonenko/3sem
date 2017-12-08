#include <iostream>
#include <unistd.h>
#include <cstring>

char**	split(char*, int*);

char* readtxt(long* length);

/*
fixit: дайте более ясное название для ф-и.
неясно, зачем нужна, и что именно делает + можете добавить комментарий
*/
char** doArr(char* buf, int* nStrings, long length);

/*
fixit: если вам нужен некий путь на вход программы, то передайте его через аргументы командной строки.
вряд ли у кого-то кроме вас на компьютере будет такой путь.
*/
const char *pathIn = "/home/andrew/CLionProjects/exec/configuration.txt";

int main(int argc, const char *argv[]) {
    int i = 0, nFunc = 0, fatherSleep = 0;
    long length = 0;
    char* buf = readtxt(&length);
    pid_t cpid = -1;
    printf("%s", buf);
    int nStrings = 0;
    char** text = doArr(buf, &nStrings, length);
     if ((nFunc = atoi(text[0])) < 0) {
        perror("Less arguments, than expected ");
        exit(-1);
    }
    for (i = 1; i <= nFunc; i++)
    {
        char** pString = split(text[i], &nStrings);
        int  time = atoi(pString[nStrings - 1]);
        pString[nStrings - 1] = (char*) NULL;
        if (fatherSleep < time)
            fatherSleep = time;
        cpid = fork();
        if (cpid == 0)
        {
            sleep(time);
            printf("process %d, running new programm\n", i);
            execvp(pString[0], pString);
            perror("exec failed");
            exit(-1);
        }
        if (cpid == -1)
        {
            perror("fork failed");
            exit(-1);
        }
        free(pString);
    }
    
    /*
    fixit: неясен смысл этого sleep'а ...
    вероятно хотели добиться того, чтобы запускающий родительский процесс
    дождался завершения всех детей? тогда нужно wait использовать
    */
    sleep(fatherSleep + 1);
    /*
    fixit: не уверен, что почистили всю выделенную память
    */
    free(text);
    return 0;
}


char** split (char* s, int * length){
    int counter = 1, i = 0;
    char** head;
    char* point;
    errno = 0;
    if (!(head = (char**) calloc (1, sizeof(char*))))
    {
        perror("Calloc error");
        exit(-1);
    }
    /*
    fixit: разделителем могут быть ещё и символы табуляции, например
    сделайте строку с разделителями параметром ф-и split
    */
    point = strtok (s, " ");
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
char* readtxt( long* length){
    FILE* fileIn;
    if ((fileIn = fopen(pathIn, "r")) == NULL) {
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
    /*
    fixit: что за константа -5?
    */
    for (i = 0; i < length - 5; i++) {
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
