#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "execute.h"
#include "parser.h"
#include "utils.h"
#include "parse_execute_loop.h"

int main(int argc, char **argv)
{        
    char *filename = NULL;
    struct flags *flags = calloc(1, sizeof(struct flags));
    FILE *file = NULL;

    // parsing arguments
    if (BaBaJi(argc, argv, &filename, flags) == 2)
    {
        free(flags);
        return 2;
    }
    // switch of file type
    if (flags->c)
        file = fmemopen(filename, strlen(filename), "r");
    else if (filename != NULL)
        file = fopen(filename, "r");
    else
        file = stdin;

    if (!file)
    {
        free(filename);
        free(flags);
        return 2;
    }

    int res = parse_execute_loop(file, flags);
    free(filename);
    return res;
}
