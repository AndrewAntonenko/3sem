#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX_SYM 100

void *FuncWrite(void *fd_1) {
    int fd = *((int *) fd_1);
    char *input;
    input = (char *) calloc(MAX_SYM, sizeof(char));
    while (1) {
        fgets(input, MAX_SYM * sizeof(char), stdin);
        if (strlen(input) < 1) continue;
        else {
            printf(":");
            write(fd, input, MAX_SYM * sizeof(char));
        }
    }
}

void *FuncRead(void *fd_2) {
    int fd = *((int *) fd_2);
    ssize_t num_of_bytes = 0;
    char *output;
    output = (char *) calloc(MAX_SYM, sizeof(char));
    do {
        printf(":");
        num_of_bytes = read(fd, output, MAX_SYM);
        printf("%s", output);
    } while (num_of_bytes > 0);
}
// В качестве аргументов надо передать два числа,
// от них зависит какой из файлов открывается на запись, а какой на чтение


int main(int argc, char **argv) {
    int fd1, fd2;
    int firstTerm = strtol(argv[1], NULL, 10);
    int secondTerm = strtol(argv[2], NULL, 10);
    pthread_t th1;
    pthread_t th2;
    char *pathTo = "fifoOut.fifo";
    char *pathIn = "fifoIn.fifo";
    if (access(pathTo, F_OK) == -1)
        if (mknod(pathTo, S_IFIFO | 0666, 0) < 0) {
            exit(EXIT_FAILURE);
        }
    if (access(pathIn, F_OK) == -1)
        if (mknod(pathIn, S_IFIFO | 0666, 0) < 0) {
            exit(EXIT_FAILURE);
        }
    if (firstTerm != secondTerm) {
        if (firstTerm > secondTerm) {
            fd1 = open(pathTo, O_WRONLY);
            fd2 = open(pathIn, O_RDONLY);
        } else {
            fd2 = open(pathTo, O_RDONLY);
            fd1 = open(pathIn, O_WRONLY);
        }
    } else {
        printf("Use different tetminals to chat\n");
    }
    pthread_create(&th1, NULL, FuncWrite, &fd1);
    pthread_create(&th2, NULL, FuncRead, &fd2);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    return 0;
}