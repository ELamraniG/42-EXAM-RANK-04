#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h> // change this to <stdlib.h>


typedef struct	json {
	enum {
		MAP,
		INTEGER,
		STRING
	} type;
	union {
		struct {
			struct pair	*data;
			size_t		size;
		} map;
		int	integer;
		char	*string;
	};
}	json;

typedef struct	pair {
	char	*key;
	json	value;
}	pair;

int parse_string(json *dst,FILE *stream);

int parse_int(json *dst,FILE *stream);

int parse_map(json *dst,FILE *stream);
void	free_json(json j);
int	argo(json *dst, FILE *stream);

int	peek(FILE *stream)
{
	int	c = getc(stream);
	ungetc(c, stream);
	return c;
}

void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF)
		printf("unexpected token '%c'\n", peek(stream));
	else
		printf("unexpected end of input\n");
}

int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return 1;
	}
	return 0;
}

int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return 1;
	unexpected(stream);
	return 0;
}

void	free_json(json j)
{
	switch (j.type)
	{
		case MAP:
			for (size_t i = 0; i < j.map.size; i++)
			{
				free(j.map.data[i].key);
				free_json(j.map.data[i].value);
			}
			free(j.map.data);
			break ;
		case STRING:
			free(j.string);
			break ;
		default:
			break ;
	}
}

void	serialize(json j)
{
	switch (j.type)
	{
		case INTEGER:
			printf("%d", j.integer);
			break ;
		case STRING:
			putchar('"');
			for (int i = 0; j.string[i]; i++)
			{
				if (j.string[i] == '\\' || j.string[i] == '"')
					putchar('\\');
				putchar(j.string[i]);
			}
			putchar('"');
			break ;
		case MAP:
			putchar('{');
			for (size_t i = 0; i < j.map.size; i++)
			{
				if (i != 0)
					putchar(',');
				serialize((json){.type = STRING, .string = j.map.data[i].key});
				putchar(':');
				serialize(j.map.data[i].value);
			}
			putchar('}');
			break ;
	}
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	char *filename = argv[1];
	FILE *stream = fopen(filename, "r");
	json	file;
	if (argo (&file, stream) != 1)
	{
		free_json(file);
		return 1;
	}
	serialize(file);
	printf("\n");
}


int	argo(json *dst, FILE *stream)
{
	char c;
	c = peek(stream);
	if (c == '"')
		return (parse_string(dst,stream));
	if (c == '-' || isdigit(c))
		return (parse_int(dst,stream));
	if (c == '{')
		return (parse_map(dst,stream));
	unexpected(stream);
	return -1;
}

int parse_string(json *dst,FILE *stream)
{
	if (!expect(stream,'"'))
		return -1;
	char buffer[4096];
	char c;
	int i = 0;
	while (1)
	{
		c = getc(stream);
		if (c == EOF)
		{
			unexpected(stream);
			return (-1);
		}
		if (c == '"')
			break;
		else if (c == '\\')
		{
			c = getc(stream);
			if (c == EOF)
			{
				unexpected(stream);
				return (-1);
			}
		}
		buffer[i++] = c;
	}
	
	buffer[i] = 0;
	char *ret;
	ret = strdup(buffer);
	dst->type = STRING;
	dst->string = ret;
	return (1);
}

int parse_int(json *dst,FILE *stream)
{
	int n;
	if (fscanf(stream,"%d", &n) == 1)
	{
		dst->integer = n;
		dst->type = INTEGER;
		return 1;
	}
	unexpected(stream);
	return -1;
}

int parse_map(json *dst,FILE *stream)
{
	pair *items = NULL;
	json key;
	int size  = 0;

	if (!expect(stream,'{'))
		return -1;
	while (!accept(stream,'}'))
	{
		items = realloc(items,sizeof(pair) * (size + 1));
		if (parse_string(&key,stream) == -1)
		{
			free(items);
			return (-1);
		}
		if (!expect(stream,':'))
		{
			free(items);
			free(key.string);
			return -1;
		}
		if (argo(&items[size].value,stream) == -1)
		{
			free(items);
			free(key.string);
			return -1;
		}
		items[size].key = key.string;
		size++;
		if(!accept(stream,',') && peek(stream) != '}')
		{
			unexpected(stream);
			free(items);
			return -1;
		}
	}
	dst->type = MAP;
	dst->map.data = items;
	dst->map.size = size;
	return 1;
}
