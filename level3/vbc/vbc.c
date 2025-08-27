 #include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
 #include <string.h>
 #include <stdio.h>
  #include <stdlib.h>
void alarm_handler(int sig)
{
    (void)sig;
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    int pid;
    int status;
    pid = fork();
    if (pid == -1)
        return -1;
    sigaction(SIGALRM, &(struct  sigaction){.sa_handler=alarm_handler},NULL);
    if (pid == 0)
    {
        f();
        exit(0);
    }
    alarm(timeout);
    waitpid(pid,&status,0);
    if (errno == EINTR)
    {

        kill(pid,SIGKILL);
        waitpid(pid,&status,0);
        if (verbose)
            printf("Bad function: timed out after %d seconds\n",timeout);
        return 0;
    }
    else if (WIFEXITED(status))
    {
        if(WEXITSTATUS(status) == 0)
        {

            if (verbose)
                printf("Nice function!\n");
            return 1;
        }
        else
        {
            if (verbose)
                printf("Bad function: exited with code %d\n",WEXITSTATUS(status));
            return 0;
        }
    }
    else if (WIFSIGNALED(status))
    {
        if (verbose)
            printf("Bad function: %s\n",strsignal(WTERMSIG(status)));
        return 0;
    }
    return -1;
}

c1r7p5:~ exam$ cat rendu/vbc/vbc.c
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
	enum {
		ADD,
		MULTI,
		VAL
	}	type;
	int	val;
	struct node	*l;
	struct node	*r;
}		node;

node	*new_node(node n)
{
	node *ret = calloc(1, sizeof(node));
	if (!ret)
		return NULL;
	*ret = n;
	return ret;
}

void	destroy_tree(node *n)
{
	if (!n)
		return;
	if (n->type != VAL)
	{
		destroy_tree(n->l);
		destroy_tree(n->r);
	}
	free(n);
}

void	unexpected(char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		printf("Unexpected end of input\n");
}

int		accept(char **s, char c)
{
	if (**s == c)
	{
		(*s)++;
		return 1;
	}
	return 0;
}

int expect(char **s, char c)
{
	if (accept(s, c))
		return 1;
	unexpected(**s);
	return 0;
}


node	*parse_addition(char **s);
node	*parse_multiplication(char **s);
node	*parse_NumxParenth(char **s);




int	eval_tree(node *tree)
{
	switch (tree->type)
	{
		case ADD:
			return (eval_tree(tree->l) + eval_tree(tree->r));
		case MULTI:
			return (eval_tree(tree->l) * eval_tree(tree->r));
		case VAL:
			return tree->val;
	}
}


node	*parse_multiplication(char **s)
{
    node *root;
    node *left;
       node tmp = {0};

    root = parse_NumxParenth(s);
    if (!root)
        return NULL;
    while (**s == '*')
    {
        left = root;
        root = new_node(tmp);
        if (!root)
            return NULL;
        root->type = MULTI;
        root->l = left;
        (*s)++;
        root->r = parse_NumxParenth(s);
        if (!root->r)
        {
            destroy_tree(root);
            return NULL;
        }
    }
    return root;
}


node	*parse_addition(char **s)
{
    node *root;
    node *left;
       node tmp = {0};

    root = parse_multiplication(s);
    if (!root)
        return NULL;
    while (**s == '+')
    {
        left = root;
        root = new_node(tmp);
        if (!root)
            return NULL;
        root->type = ADD;
        (*s)++;
        root->l = left;
        root->r = parse_multiplication(s);
        if (!root->r)
        {
            destroy_tree(root);
            return NULL;
        }
    }
    return root;
}


node	*parse_NumxParenth(char **s)
{
    node *root;
    node tmp = {0};
    if (**s == '(')
    {
        (*s)++;
        root = parse_addition(s);
        if (!root)
            return NULL;
        if (**s != ')')
        {
            unexpected(**s);
            destroy_tree(root);
            return NULL;
        }
        (*s)++;
        return root;
    }
    else if (isdigit(**s) || **s == '-')
    {
        root = new_node(tmp);
        if (!root)
            return NULL;
        root->type = VAL;
        root->val = **s - 48;
        (*s)++;

        if (isdigit(**s))
        {
            unexpected(**s);
            destroy_tree(root);
            return NULL;
        }
        return root;
    }
    else
    {
        unexpected(**s);
        return NULL;
    }
    return NULL;
}


int main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	node *tree = parse_addition(&argv[1]);
	if (!tree)
		return 1;
    if(argv[1][0])
    {
        unexpected(argv[1][0]);
        destroy_tree(tree);
        return 1;
    }
	printf("%d\n", eval_tree(tree));

}
