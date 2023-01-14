#include "utils.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast.h"
// #include "hash_map.h"

// struct hash_map *hashmap = NULL;

/*
 * \brief Parsing of inputs
 * \param argc argv from main, filename *.sh, d_opt to know which flags set
 * \return 0 if no error, 2 otherwise
 */
int BaBaJi(int argc, char **argv, char **filename, struct flags *flags)
{
    // hashmap = hash_map_init(20);
    if (argc == 0)
        return 0;
    int i = 1;
    for (; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            for (int k = 1; argv[i][k]; k++)
            {
                if (argv[i][k] == 'c')
                    flags->c = 1;
                else if (argv[i][k] == 'u')
                    flags->u = 1;
                else if (argv[i][k] == 'p')
                    flags->p = 1;
                else
                    return 2;
            }
            if (!flags->c)
                continue;
            if (++i >= argc)
                return 2;
        }
        *filename = strdup(argv[i++]);
        break;
    }
    /*
        int count = 1;
        char buf[1000] = { 0 };
        for (; i < argc; i++)
        {
            sprintf(buf, "%d", count);
            hash_map_insert(hashmap, buf, argv[i]);
            count++;
        }

        hash_map_insert(hashmap, "#", buf);

        hash_map_insert(hashmap, "@", buf);
        hash_map_insert(hashmap, "*", buf);
    */
    return 0;
}
