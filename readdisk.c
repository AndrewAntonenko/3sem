#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 100


enum Result {
    NotFound,
    Found

};

enum Result search(char *path, int deep, const char *file, char *answer);

void toParentDir(char *path);

void append(char *string, char *extra_dir);

void toParentDir(char *path) {
    size_t length = 0;
    if (path == NULL) return;
    length = strlen(path) - 1;
    while (length != 0) {
        if (path[length] == '/')
            return;
        length--;
    }

}

void append(char *string, char *extra_dir) {
    strcat(string, "/");
    strcat(string, extra_dir);

}


enum Result search(char *path, int deep, const char *file, char *answer) {
    DIR *dir;
    struct dirent *current;
    enum Result success = Found;
    if (deep < 0) {
        return !success;
    }
    if ((dir = opendir(path)) == NULL) {
        perror("Can't open directory in search function");
        return EXIT_FAILURE;
    }
    int count = 0;
    while ((current = readdir(dir))) {
        if (!strcmp(current->d_name, file)) {
            strcpy(answer, path);
            printf("File %s was found in %s directory\n", file, answer);
	    count ++;	
        }
        if (current->d_type == 4 && strcmp(".", current->d_name)
            && strcmp("..", current->d_name)) {
            append(path, current->d_name);
            success = search(path, deep - 1, file, answer);
            if (success) {
                printf("File %s was found in %s directory\n", file, answer);
		count ++;
            } else {
                toParentDir(path);
            }
        }
    }
    if (count) return Found;
    else return NotFound;
}

/*
 Первый аргумент- название директории, потом глубина поиска и искомый файл
 */
int main(int argc, char **argv) {
    if (argc != 4)
        printf("Incorect number of arguments\n");

    int depth = atoi(argv[2]);
    int ret = 0;

    char *directory = (char *) calloc(MAX_SIZE, sizeof(char));
    char *file = (char *) calloc(MAX_SIZE, sizeof(char));
    char *answer = (char *) calloc(MAX_SIZE, sizeof(char));
    strcpy(directory, argv[1]);
    strcpy(file, argv[3]);

    if (!search(directory, depth, file, answer))
        printf("File %s wasn't found in %s directory\n", file, answer);

    free(directory);
    free(answer);
    free(file);

}
