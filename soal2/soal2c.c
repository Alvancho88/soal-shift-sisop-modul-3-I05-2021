#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int p1[2];
    int p2[2];

    char *execv1[] = {"ps", "aux", NULL};
    char *execv2[] = {"sort", "-nrk", "3,3", NULL};
    char *execv3[] = {"head", "-5", NULL};

    pipe(p1);

    if(fork() == 0)
    {
        close(p1[0]);
        dup2(p1[1], 1);
        close(p1[1]);
        execv("/usr/bin/ps", execv1);
    }
    close(p1[1]);

    pipe(p2);
    if(fork() == 0)
    {
        close(p2[0]);
        dup2(p1[0], 0);
        close(p1[0]);
        dup2(p2[1], 1);
        close(p2[1]);
        execv("/usr/bin/sort", execv2);
    }
    close(p1[0]);
    close(p2[1]);

    if(fork() == 0)
    {
        dup2(p2[0], 0);
        close(p2[0]);
        execv("/usr/bin/head", execv3);
    }
    close(p2[0]);

    return 0;
}