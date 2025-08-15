//Allowed functions: atoi, printf, fprintf, malloc, calloc, realloc, free, stdout,
#include <stdio.h>
#include <unistd.h>

void ft_power_set(int *nums,int pos,int size)
{
	
}
int main(char ac, char **av)
{
	if (ac <= 2)
		return 0;
	int nums[ac - 2];
	int i = 2;
	while (av[i])
	{
		nums[i - 2] = atoi(av[i]);
		i++;
	}
	ft_power_set(nums,0,ac - 2);
}