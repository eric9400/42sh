#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int BaBaJi(int argc, char *argv[], char **filename, int *d_opt)
{
    if (argc == 1)
        return 0;

    int opt;
    while ((opt = getopt(argc, argv, ":cp")) != -1)
    {
        switch (opt)
        {
        case 'c':
            if (*d_opt >= 1)
                *d_opt = 2;
            else
                *d_opt = 0; 
            break;
        //pretty-print opt
        case 'p':
            if (*d_opt >= 0)
                *d_opt = 2;
            else   
                *d_opt = 1;
            break;
        case ':':
            errx(2,"Usage : 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n");
        case '?':
            errx(2,"Usage : 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n");
        }
    }
    if (argc - optind == 1)
    {
        *filename = strdup(argv[optind]);
        return 0;
    }
    else
        errx(2,"Usage : 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n");
}

int main(int argc, char* argv[])
{        
    char *filename = NULL;
    int opt = -1;

    BaBaJi(argc, argv, &filename, &opt);

    return opt;
}
