// Allowed functions : atoi, fprintf, write, calloc, malloc, free, realloc,stdout, stderr

#include <stdio.h>
#include <stdlib.h>

int is_valid_q(int *arr, int row,int col)
{
    for (int i = 0; i < row; i++)
    {
       if (arr[i] == col || arr[i] - i == col - row || arr[i] + i == col + row)
            return 0;
    }
    return 1;
}

void print_q(int n, int *arr)
{
	for (int i = 0; i < n; i++)
	{
		printf("%d",arr[i]);
		if (i +1 != n)
			printf(" ");
	}
	printf("\n");
}
void	ft_nqueens(int n, int *arr, int row)
{
	if (n == row)
		print_q(n, arr);
	for (int i = 0; i < n; i++)
	{
		if (is_valid_q(arr, row, i))
		{
			arr[row] = i;
			ft_nqueens(n,arr,row + 1);
		}
	}
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		printf("\n");
		return (0);
	}
	int n = atoi(av[1]);
	if (n == 0)
	{
		printf("\n");
		return (0);
	}
	int arr[n];
	for (int i = 0; i < n; i++)
		arr[i] = 0;
	ft_nqueens(atoi(av[1]), arr, 0);
}