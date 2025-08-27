#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
	enum
	{
		ADD,
		MULTI,
		VAL
	} type;
	int			val;
	struct node	*left;
	struct node	*right;
}				node;

node	*new_node(node n)
{
	node	*ret;

	ret = calloc(1, sizeof(n));
	if (!ret)
		return (NULL);
	*ret = n;
	return (ret);
}

void	destroy_tree(node *n)
{
	if (!n)
		return ;
	if (n->type != VAL)
	{
		destroy_tree(n->left);
		destroy_tree(n->right);
	}
	free(n);
}

void	unexpected(char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		printf("Unexpected end of input\n"); //changed this from file to input
}

node			*parse_add(char **s);

node	*pars_paren_n(char **s)
{
	node	*root;
	node tmp = {0};

	root = NULL;
	if (**s == '(')
	{
		(*s)++;
		root = parse_add(s);
		if (!root)
			return (NULL);
		if (**s == ')')
		{
			(*s)++;
			return (root);
		}
		else
		{
			destroy_tree(root);
			unexpected(**s);
			return (NULL);
		}
	}
	else if (isdigit(**s))
	{
		root = new_node(tmp);
		if (!root)
			return (NULL);
		root->type = VAL;
		root->val = **s - 48;
		(*s)++;
		if (isdigit(**s))
		{
			destroy_tree(root);
			unexpected(**s);
			return (NULL);

		}
	}
	else
	{
		unexpected(**s);
		return (NULL);
	}
	return (root);
}

node	*pars_mult(char **s)
{
	node	*root;
	node	*left;
	node tmp = {0};

	root = pars_paren_n(s);
	while (**s == '*')
	{
		left = root;
		root = new_node(tmp);
		if (!root)
		{
			destroy_tree(left);
			return (NULL);
		}
		root->type = MULTI;
		root->left = left;
		(*s)++;
		root->right = pars_paren_n(s);
		if (!root->right)
		{
			destroy_tree(root);
			return (NULL);
		}
	}
	return (root);
}
node	*parse_add(char **s)
{
	node	*root;
	node	*left;
	node	tmp = {0};

	root = pars_mult(s);
	while (**s == '+')
	{
		left = root;
		root = new_node(tmp);
		if (!root)
		{
			destroy_tree(left);
			return (NULL);
		}
		root->type = ADD;
		root->left = left;
		(*s)++;
		root->right = pars_mult(s);
		if (!root->right)
		{
			destroy_tree(root);
			return (NULL);
		}
	}
	return (root);
}

int	eval_tree(node *tree)
{
	switch (tree->type)
	{
	case ADD:
		return (eval_tree(tree->left) + eval_tree(tree->right));
	case MULTI:
		return (eval_tree(tree->left) * eval_tree(tree->right));
	case VAL:
		return (tree->val);
	}
}

int	main(int argc, char **argv)
{
	node	*tree;

	if (argc != 2)
		return (1);
	tree = parse_add(&(argv[1]));
	if (!tree)
		return (1);
	if (argv[1][0])
	{
		unexpected(*argv[1]);
		destroy_tree(tree);
		return (1);
	}
		printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
}

