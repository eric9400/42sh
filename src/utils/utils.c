#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast.h"
#include "utils.h"

/*
 * \brief Parsing of inputs
 * \details auto parsing
 * \param argc argv from main, filename *.sh, d_opt to know which flags set
 * \return 0 if no args, 0 if -c, 1 if -p, 2 if both
 */
int BaBaJi(int argc, char **argv, char **filename, struct flags *flags)
{
    if (argc == 1)
        return 0;

    int opt;
    while ((opt = getopt(argc, argv, ":cp")) != -1)
    {
        switch (opt)
        {
            case 'c':
                flags->c = 1;
                break;
            case 'p':
                flags->p = 1; 
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
    else if ((argc == 2 && flags->p) || argc == 1)
        return 0;
    else
        errx(2,"Usage : 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n");
}
