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
    char *s2 = strdup("-Ee");
    char *argv2[] = {s1, s2, strdup("test\\n"), NULL};
    echo(argv2);
    free(s1);
    free(s2);
    return 0;
}
