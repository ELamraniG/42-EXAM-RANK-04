//Allowed functions: puts, write
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int lvl_check(char *av)
{
	int balance = 0;
	int opened = 0;
	for(int i = 0;av[i]; i++)
	{
		if (av[i] == ')' && balance == 0)
			opened++;
		else if (av[i] == ')')
			balance--;
		else if (av[i] == '(')
			balance++;
	}
	return (balance + opened);
}

int    is_valid(char *str)
{
    int    i;
    int    res;

    i = 0;
    res = 0;
    while (str[i])
    {
        if (str[i] == '(')
            res++;
        if (str[i] == ')')
            res--;
        if (res < 0)
            return (0);
        i++;
    }
    if (res == 0)
        return (1);
    return (0);
}
void ft_rip(char *av,int changes,int lvl,int pos)
{
	if (changes > lvl)
		return;
	if (is_valid(av) && changes == lvl)
	{
		puts(av);
		return;
	}
	for (int i = pos; av[i];i++)
	{
		char tmp = av[i];
		changes++;
		av[i] = '_';
		ft_rip(av,changes,lvl, i + 1);
		av[i] = tmp;
		changes--;
	}
}

int main(int ac,char **av)
{
	if (ac != 2 || !av[1])
		return 0;
	av++;
	int lvl = lvl_check(*av);
	ft_rip(*av,0,lvl,0);
}