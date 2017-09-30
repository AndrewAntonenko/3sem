#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int argc, char* argv[])
{
    pid_t pid, pid0, ppid;
    int n = atoi(argv[1]);
    printf("%d\n", n);
    ppid = getpid();
    int exitStatus = 0;
    int i = 0;
    pid0 = getpid();
    while ((getpid() == pid0) && (i < atoi(argv[1])))
    {
        switch(pid = fork()) {
            case -1:
                perror("Error in fork");
                exit(1);
                break;
            case 0:
                printf("New process is created: PID =  %d"
                        "; PPID = %d\n", getpid(), getppid());
                pid0 = getpid();
                break;
            default:
                //pid0 = getpid();
                pid0 = 0;
                waitpid(0,&exitStatus,0 );
                break;
        }
        i++;
    }
    //getchar();
    if(getpid() == ppid)
    printf("This is parental process, my PID = %d\nAll processes are finished.\n", ppid);
    return 0;
}