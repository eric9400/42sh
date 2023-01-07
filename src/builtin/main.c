#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <stdlib.h>

#include "builtin.h"

int main(void)
{
    // char *argv[] = {strdup("echo"), strdup("-e"), strdup("ca marche"), strdup("-e"), strdup("\nbien"), NULL};
    // char *argv1[] = {strdup("echo"), strdup("-e"), strdup("retour ligne\n\ttab"), NULL};
    // char *argv1[] = {strdup("echo"), strdup("-E"), strdup("retour ligne\\n\\ttab"), NULL};
    // char *argv1[] = {strdup("echo"), strdup("-n"), strdup("test"), NULL};
    // char *argv2[] = {strdup("echo"), strdup("test"), NULL};
    char *s1 = strdup("echo");
    char *argv2[] = {s1, strdup("'test'asdsd"), NULL};
    echo(argv2);
    free(s1);
    return 0;
}
