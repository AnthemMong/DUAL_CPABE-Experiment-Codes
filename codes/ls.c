# include<stdio.h>
# include<stdlib.h>
# include<string.h>
# include<ctype.h>

# define LCHILD(x) (2*(x)+1)
# define RCHILD(x) (2*(x)+2)

void print_matrix(int **M, int row, int column);

int main(void)
{
    char* bintree = "&E|^^|&^^^^&&||^^^^^^^^ABCDEFGH";
    int n, t;
    int i, j;
    int c;
    int** M;
    int** T;

    n = 0;
    c = 1;
    t = strlen(bintree);
    for (i = 0; i < t; i++)
    {
        if (isalpha(bintree[i]))
            n++;
    }

    T = (int**)malloc(sizeof(int*) * t);
    for (i = 0; i < t; i++)
    {
        T[i] = (int*)malloc(sizeof(int) * n);
        memset(T[i], 0, sizeof(int) * n);
    }

    T[0][0] = 1;    
    for (i = 0; i < t; i++)
    {
        if (bintree[i] == '&')
        {
            T[LCHILD(i)][c] = -1;
            memcpy(T[RCHILD(i)], T[i], sizeof(int) * n);
            T[RCHILD(i)][c] = 1;
            c++;
        }
        if (bintree[i] == '|')
        {
            memcpy(T[LCHILD(i)], T[i], sizeof(int) * n);
            memcpy(T[RCHILD(i)], T[i], sizeof(int) * n);
        }
    }

    M = (int**)malloc(sizeof(int*) * n);
    for (i = 0; i < n; i++)
        M[i] = (int*)malloc(sizeof(int) * c);

    for (i = 0, j = 0; i < t; i++)
    {
        if (isalpha(bintree[i]) )
        {
            memcpy(M[j], T[i], sizeof(int) * c);
            j++; 
        }
    }

    print_matrix(M, n, c);
    return 0;
}

void print_matrix(int **M, int row, int column)
{
    int i, j;
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < column; j++)
            printf("%d ", M[i][j]);
        printf("\n");
    }
}
