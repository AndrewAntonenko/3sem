#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <wait.h>

#define MAX_DISH_ENTRIES	1024
#define ALL_SENT			225

typedef struct dish {
    int dish_type;
    unsigned int wash_time;
    unsigned int wipe_time;
} dish_t;

int pipe_fd[2] = {-1, -1};

void	wash(int type, dish_t * dictionary);
void	wipe(dish_t dish);

dish_t	receive(dish_t * dictionary);
int		send(int type);

int		get_numberoftypes(FILE * dirty_dishes);
void	get_washtimes(dish_t * dictionary, FILE * wash_table);
void	get_wipetimes(dish_t * dictionary, FILE * wipe_table);
void	get_dishtypes(dish_t * dictionary, FILE * dirty_dishes);

long    strtoint(char *str, int base);

int main(int argc, char * argv[])
{
    long N = strtoint(argv[1], 10);
    if (N == -1)
        exit(EXIT_FAILURE);

    FILE * dirty_dishes = fopen(argv[2], "r");
    FILE * wash_table   = fopen(argv[3], "r");
    FILE * wipe_table   = fopen(argv[4], "r");

    pipe(pipe_fd);

    int number_of_types = get_numberoftypes(dirty_dishes);
    printf("number of types : %d\n", number_of_types);
    dish_t * dictionary = (dish_t *)calloc((size_t) number_of_types, sizeof(dish_t));
    get_dishtypes(dictionary, dirty_dishes);
    get_washtimes(dictionary, wash_table);
    get_wipetimes(dictionary, wipe_table);

    printf("Dictionary\n");
    for (int i = 0; i < number_of_types; i++)
    {
        printf(" %d: type:%d\twash:%d\twipe:%d\n", i, dictionary[i].dish_type, dictionary[i].wash_time, dictionary[i].wipe_time);
    }

    int pid = fork();
    if (pid == -1) {
        printf("Fork error\n");
        exit(EXIT_FAILURE);
    }
    if (pid != 0)   // parent - washer
    {
        printf("%d\n", pid);
        int quantity = 0;
        int type = 0;

        rewind(dirty_dishes);
        while(fscanf(dirty_dishes, "%d:%d%*[\n]", &type, &quantity) >= 2)
            for (int i = 0; i < quantity; i++)
            {
                wash(type, dictionary);
                send(type);
            }

        send(ALL_SENT);
        printf("Parent - all done\n");
        waitpid(pid, NULL, 0);
    } else {    // child - wiper

        dish_t * table = (dish_t *)calloc((size_t) N, sizeof(dish_t));
        int counter = 0;
        sleep(2);

        while(true)
        {
            wipe(table[counter]);
            table[counter] = receive(dictionary);

            if (table[counter].dish_type == -1)
            {
                for (int i = 0; i < N; i++)
                    wipe(table[i]);
                break;
            }
            counter = (int) ((counter + 1) % N);
        }

        free(table);

        printf("child - all done\n");
    }

    free(dictionary);
    fclose(dirty_dishes);
    fclose(wash_table);
    fclose(wipe_table);
    return 0;
}

void wash(int type, dish_t * dictionary)
{
    int i = 0;
    while(dictionary[i].dish_type != type)
        i++;
    printf("Washing %d!\n", dictionary[i].dish_type);
    sleep(dictionary[i].wash_time);
    printf("Washed  %d!\n", dictionary[i].dish_type);
}

void wipe(dish_t dish)
{
    if (dish.dish_type != 0  &&
        dish.dish_type != -1 &&
        dish.dish_type != ALL_SENT)
    {
        printf("Wiping %d!\n", dish.dish_type);
        sleep(dish.wipe_time);
        printf("Wiped  %d!\n", dish.dish_type);
        dish.dish_type = dish.wash_time = dish.wipe_time = 0;
    }
}

void get_dishtypes(dish_t * dictionary, FILE * dirty_dishes)
{
    rewind(dirty_dishes);
    int type = 0;
    int numbers = 0;
    bool stored = false;
    while(fscanf(dirty_dishes, "%d:%*i%*[\n]", &type) >= 1)
    {
        for (int i = 0; i < numbers; i++)
            if (dictionary[i].dish_type == type)
            {
                stored = true;
                break;
            }

        if(!stored)
        {
            dictionary[numbers].dish_type = type;
            numbers++;
        }

        stored = false;
    }
    rewind(dirty_dishes);
}

void get_washtimes(dish_t * dictionary, FILE * wash_table)
{
    rewind(wash_table);

    int type = 0;
    unsigned int wash_time = 0;
    int i = 0;

    while(fscanf(wash_table, "%d:%d%*[\n]", &type, &wash_time) >= 2)
    {
        while(dictionary[i].dish_type != type && dictionary[i].dish_type != 0)
            i++;

        dictionary[i].wash_time = wash_time;
        i = 0;
    }

    rewind(wash_table);
}

void get_wipetimes(dish_t * dictionary, FILE * wipe_table)
{
    rewind(wipe_table);

    int type = 0;
    unsigned int wipe_time = 0;
    int i = 0;
    while(fscanf(wipe_table, "%d:%d%*[\n]", &type, &wipe_time) >= 2)
    {
        while(dictionary[i].dish_type != type && dictionary[i].dish_type != 0)
            i++;

        dictionary[i].wipe_time = wipe_time;
        i = 0;
    }

    rewind(wipe_table);
}

int get_numberoftypes(FILE * dirty_dishes)
{
    int * stored_types = (int *)calloc(MAX_DISH_ENTRIES, sizeof(int));
    int type = 0;
    int numbers = 0;
    bool stored = false;
    while(fscanf(dirty_dishes, "%d:%*i%*[^\n]%*c", &type) >= 1)
    {
        for (int i = 0; i < numbers; i++)
            if (stored_types[i] == type)
            {
                stored = true;
                break;
            }

        if(!stored)
        {
            stored_types[numbers] = type;
            numbers++;
        }

        stored = false;
    }

    free(stored_types);
    rewind(dirty_dishes);
    return numbers;
}

int send(int type)
{
    int payload = type;
    write(pipe_fd[1], &payload, sizeof(int));
    return 0;
}

dish_t receive(dish_t * dictionary)
{
    dish_t dish;
    int type_got = 0;
    read(pipe_fd[0], &type_got, sizeof(int));

    int i = 0;
    if (type_got != ALL_SENT)
    {
        while(dictionary[i].dish_type != type_got)
            i++;
        dish = dictionary[i];
    }
    else
    {
        dish.dish_type = -1;
        dish.wash_time = 0;
        dish.wipe_time = 0;
    }

    return dish;
}

long strtoint(char *str, int base)
{
    char *endptr;
    long val;

    errno = 0;
    val = strtol(str, &endptr, base);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
    {
        perror("strtol");
        return -1;
    }

    if (endptr == str)
    {
        fprintf(stderr, "No digits were found\n%s", str);
        return -1;
    }
    if (*endptr != '\0')
    {
        fprintf(stderr, "Not numerical data encountered\n");
        return -1;
    }
    return val;
}