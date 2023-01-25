#include "expand_tools.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "hash_map.h"
#include "my_string.h"
#include "parse_execute_loop.h"
#include "utils.h"

static int is_valid_char(char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;
    else if (c >= '0' && c <= '9')
        return 1;
    return c == '_';
}

static void resize_string(struct string *new_str, char *buf)
{
    size_t size = new_str->index + strlen(buf) + 1;
    if (size < new_str->len)
        return;
    new_str->len = size * 2;
    new_str->str = realloc(new_str->str, new_str->len);
}

void string_append(struct string *new_str, char *buf)
{
    resize_string(new_str, buf);
    my_strcat(new_str->str + new_str->index, buf);
    new_str->index += strlen(buf);
}

static inline char *is_special_var(char *str, int return_value)
{
    char buf[1000];

    /*if (!strcmp(str, "@") || !strcmp(str, "*")){}else */
    if (atoi(str) != 0 || !strcmp(str, "#") || !strcmp(str, "OLDPWD")
        || !strcmp(str, "PWD") || !strcmp(str, "IFS"))
    {
        const char *res = hash_map_get(hashM->hashmap, str);
        if (!res)
            return NULL;
        return strdup(res);
    }
    else if (!strcmp(str, "?"))
    {
        sprintf(buf, "%d", return_value);
        return strdup(buf);
    }
    else if (!strcmp(str, "$"))
    {
        sprintf(buf, "%d", getpid());
        return strdup(buf);
    }
    else if (!strcmp(str, "RANDOM"))
    {
        srand(time(NULL));
        // max random possible
        int temp = rand() % 32768;
        sprintf(buf, "%d", temp);
        return strdup(buf);
    }
    else if (!strcmp(str, "UID"))
    {
        uid_t uid = getuid();
        sprintf(buf, "%d", uid);
        return strdup(buf);
    }
    return NULL;
}

// $* and $@
static void append_no_quotes(struct string *new_str)
{
    int count = 1;
    char buf[1000] = { 0 };
    while (1)
    {
        sprintf(buf, "%d", count);
        const char *temp = hash_map_get(hashM->hashmap, buf);
        if (temp == NULL)
            break;
        char *value = strdup(temp);
        char *keep = value;
        char *tok = strtok(value, " ");
        while (tok != NULL)
        {
            new_str->v = vector_append(new_str->v, strdup(tok));
            tok = strtok(NULL, "    ");
        }
        count++;
        free(keep);
    }
}

// "$@"
static void append_args(struct string *new_str)
{
    int count = 1;
    char buf[1000] = { 0 };
    while (1)
    {
        sprintf(buf, "%d", count);
        const char *temp = hash_map_get(hashM->hashmap, buf);
        if (temp == NULL)
            break;
        char *value = strdup(temp);
        new_str->v = vector_append(new_str->v, value);
        count++;
    }
}

// "$*"
static void append_concatenate_args(struct string *new_str)
{
    struct string *res = init_string3(5);
    int count = 1;
    char buf[1000] = { 0 };
    sprintf(buf, "%d", count);
    const char *temp = hash_map_get(hashM->hashmap, buf);
    while (1)
    {
        if (temp == NULL)
            break;
        char *value = strdup(temp);
        string_append(res, value);

        count++;
        sprintf(buf, "%d", count);
        temp = hash_map_get(hashM->hashmap, buf);
        if (temp != NULL)
        {
            buf[0] = ' ';
            buf[1] = '\0';
            string_append(res, buf);
        }
        free(value);
    }
    res->str = realloc(res->str, res->index + 1);
    new_str->v = vector_append(new_str->v, strdup(res->str));
    destroy_string(res);
}

static void expand_from_hashmap(struct string *new_str, char *buf,
                                int return_value, int in_d_quotes)
{
    if (!in_d_quotes && (buf[0] == '@' || buf[0] == '*'))
        append_no_quotes(new_str);
    else if (buf[0] == '@')
        append_args(new_str);
    else if (buf[0] == '*')
        append_concatenate_args(new_str);
    else
    {
        char *value = NULL;
        if ((value = is_special_var(buf, return_value)) == NULL)
            value = hashmap_get_copy(hashM->hashmap, buf);
        if (value)
        {
            string_append(new_str, value);
            free(value);
        }
    }
}

static int is_special_char(char c)
{
    return c == '@' || c == '?' || c == '$' || c == '#' || c == '*';
}

static void expand_special_char(struct string *str, struct string *new_str, int return_value, int in_d_quotes)
{
    char *key = strndup(str->str + str->index, 1);
    expand_from_hashmap(new_str, key, return_value, in_d_quotes);
    free(key);
}

static void expand_special_char_v2(struct string *str, struct string *new_str, int return_value, int in_d_quotes)
{
    char *temp = strstr(str->str, "}");
    temp[0] = '\0';
    char *key = strndup(str->str + str->index, 1);
    expand_from_hashmap(new_str, key, return_value, in_d_quotes);
    free(key);
}


// 30 lines
static int exec_command_sub(char *str, int pipe_fds[2], struct string *new_str, int return_value)
{
	int status;

	pid_t pid = fork();

	// parent
	if (pid != 0)
	{
		//close write side
		close(pipe_fds[1]);

		char *buffer = NULL;
		int nb_bytes = 0;
		size_t capacity = 20;
		int nb_bytes_tot = 0;
		do
		{
			nb_bytes_tot += nb_bytes;
			buffer = realloc(buffer, capacity + 21);
			capacity += 20;
			nb_bytes = read(pipe_fds[0], buffer + nb_bytes_tot, capacity - nb_bytes_tot);
		} while(nb_bytes > 0);

		waitpid(pid, &status, 0);
		buffer[nb_bytes_tot] = '\0';
		if (status == 0)
			string_append(new_str, buffer);
		free(buffer);
		close(pipe_fds[0]);
		return status;
	}
	else
	{
		//close read side
		close(pipe_fds[0]);

		//create file for the string and set -c option
		FILE *filename = fmemopen(str, strlen(str), "r");
		struct flags *f = calloc(1, sizeof(struct flags));
		f->c = 1;

		dup2(pipe_fds[1], STDOUT_FILENO);

		int err = parse_execute_loop(filename, f);

		close(pipe_fds[1]);
		exit(err);
	}
}

static int command_substitution(struct string *str, struct string *new_str, int return_value)
{
	str->index += 1;
	int start = str->index;
	while(str->str[str->index] != ')' && str->str[str->index] != '\0')
		str->index += 1;
	if (str->str[str->index] == '\0')
		return 1;

	int pipe_fds[2];

	if (pipe(pipe_fds) < 0)
		return 1;
	
	char *s = strndup(str->str + start, str->index - start);

	int err = exec_command_sub(s, pipe_fds, new_str, return_value);

	free(s);

	return err;
}

// 37 lines
int dollar_expansion(struct string *str, struct string *new_str,
                     int return_value, int in_d_quotes)
{
    str->index += 1;
    char buf[5] = { 0 };
    // ${a}
    // need to handle error cases "echo ${+}" for example
    if (str->str[str->index] == '{')
    {
        str->index++;
        size_t start = str->index;
        if (is_special_char(str->str[str->index]))
        {
            expand_special_char_v2(str, new_str, return_value, in_d_quotes);
            return 0;
        }
        while (is_valid_char(str->str[str->index]))
            str->index += 1;
        char *key = strndup(str->str + start, str->index - start);
        if (str->str[str->index] == '}')
        {
            expand_from_hashmap(new_str, key, return_value, in_d_quotes);
            free(key);
            return 0;
        }
        fprintf(stderr, "bad substitution %s\n", key);
        free(key);
        // if no matching '}' => error
        return 1;
    }
	// command substitution case $()
	else if (str->str[str->index] == '(')
	{
		return command_substitution(str, new_str, return_value);
	}
    // $a $RANDOM $UID $HOME
    else if (is_valid_char(str->str[str->index]))
    {
        // case $n
        if (isdigit(str->str[str->index]))
        {
            buf[0] = str->str[str->index];
            expand_from_hashmap(new_str, buf, return_value, in_d_quotes);
        }
        // case $abc
        else
        {
            size_t start = str->index;
            while (is_valid_char(str->str[str->index]))
                str->index += 1;
            char *key = strndup(str->str + start, str->index - start);
            expand_from_hashmap(new_str, key, return_value, in_d_quotes);
            free(key);
            str->index -= 1;
        }
    }
    // case $@ $* $? $$ $#
    else if (is_special_char(str->str[str->index]))
    {
        expand_special_char(str, new_str, return_value, in_d_quotes);
        /*
        char *key = strndup(str->str + str->index, 1);
        expand_from_hashmap(new_str, key, return_value, in_d_quotes);
        free(key);
        */
    }
    // non substituable var
    else
    {
        // $_ with invalid char after $, just print $_
        buf[0] = '$';
        buf[1] = str->str[str->index];
        string_append(new_str, buf);
    }
    return 0;
}

static int is_valid_escape_d_quotes(char c)
{
    return c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
}

void slash_expansion_in_d_quotes(struct string *str, struct string *new_str,
                                 int in_d_quotes)
{
    str->index++;
    char buf[3] = { 0 };
    if (is_valid_escape_d_quotes(str->str[str->index]))
    {
        if (str->str[str->index] == '\n')
            return;
        buf[0] = str->str[str->index];
    }
    else
    {
        if (in_d_quotes)
        {
            buf[0] = '\\';
            buf[1] = str->str[str->index];
        }
        else
            buf[0] = str->str[str->index];
    }
    string_append(new_str, buf);
}
