#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of file\n");
}

int accept(char **s, char c)
{
    if (**s)
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}

int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            return (eval_tree(tree->l) + eval_tree(tree->r));
        case MULTI:
            return (eval_tree(tree->l) * eval_tree(tree->r));
        case VAL:
            return (tree->val);
    }
}

node	*parse_mult(char **s);
node	*parse_grp(char **s);

node	*parse_add(char **s)
{
	node	*left;
	node	*right;
	node	tmp;

	left = parse_mult(s);
	if(!left)
		return NULL;
	while(**s == '+')
	{
		(*s)++;
		right = parse_mult(s);
		if(!right)
		{
			destroy_tree(left);
			return NULL;
		}
		tmp.type = ADD;
		tmp.l = left;
		tmp.r = right;
		left = new_node(tmp);
	}
	return left;
}

node	*parse_mult(char **s)
{
	node	*left;
	node	*right;
	node	tmp;

	left = parse_grp(s);
	if(!left)
		return NULL;
	while(**s == '*')
	{
		(*s)++;
		right = parse_grp(s);
		if(!right)
		{
			destroy_tree(left);
			return NULL;
		}
		tmp.type = MULTI;
		tmp.l = left;
		tmp.r = right;
		left = new_node(tmp);
	}
	return left;
}

node	*parse_grp(char **s)
{
	node	*res;
	node	tmp;

	res = NULL;
	if(**s == '(')
	{
		(*s)++;
		res = parse_add(s);
		if(!res || **s != ')')
		{
			destroy_tree(res);
			unexpected(**s);
			return NULL;
		}
		(*s)++;
		return res;
	}
	if(isdigit(**s))
	{
		tmp.val = **s - '0';
		tmp.type = VAL;
		res = new_node(tmp);
		(*s)++;
		return res;
	}
	unexpected(**s);
	return NULL;
}

int	ft_balance(char *str)
{
	int i = 0;
	int balance = 0;

	while(str[i])
	{
		if(str[i] == '(')
			balance++;
		else if(str[i] == ')')
		{
			balance--;
			if(balance < 0)
				return -1;
		}
		i++;
	}
	return balance;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
	if(ft_balance(argv[1]) == -1)
	{
		printf("Unexpected token ')'\n");
		return 1;
	}
    node *tree = parse_add(&argv[1]);
    if (!tree)
        return (1);
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
}