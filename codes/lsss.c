# include<stdio.h>
# include<stdlib.h>
# include<string.h>
# include<ctype.h>
# include"lmatrix.h"
# include"lsss.h"

int access_matrix(char* bintree, int len, mat_t* matrix, maparr_t* maps)
{
    int row, column;
    int i, j;
    int** T;

    if (len > 0)
    {   
        row = 0;
        column = 1;
        for (i = 0; i < len; i++)
        {
            if (isalpha(bintree[i]))
                row++;
        }

        T = (int**)malloc(sizeof(int*) * len);
        for (i = 0; i < len; i++)
        {
            T[i] = (int*)malloc(sizeof(int) * row);
            memset(T[i], 0, sizeof(int) * row);
        }

        T[0][0] = 1;
        for (i = 0; i < len; i++)
        {
            if (bintree[i] == '&')
            {
                T[LCHILD(i)][column] = -1;
                memcpy(T[RCHILD(i)], T[i], sizeof(int) * row);
                T[RCHILD(i)][column] = 1;
                column++;
            }
            if (bintree[i] == '|')
            {
                memcpy(T[LCHILD(i)], T[i], sizeof(int) * row);
                memcpy(T[RCHILD(i)], T[i], sizeof(int) * row);
            }
        }

        matrix->row = row;
        matrix->column = column;
        matrix->content = (int**)malloc(sizeof(int*) * row);
        for (i = 0; i < row; i++)
            matrix->content[i] = (int*)malloc(sizeof(int) * column); 
        
        maps->content = (map_t*)malloc(sizeof(map_t) * row);
        for (i = 0, j = 0; i < len; i++)
        {
            if (isalpha(bintree[i]) )
            {
                memcpy(matrix->content[j], T[i], sizeof(int) * column);
                maps->content[j].att = bintree[i];
                maps->content[j].index = j;
                j++; 
            }
        }
        maps->len = j;

        for (i = 0; i < len; i++)
            free(T[i]);
        free(T);

        return j;
    }
    return 0;
}

void init_epsilon(vec_t* vec, int len)
{
    int i;

    vec->len = len;
    vec->content = (int*)malloc(sizeof(int) * len);
    vec->content[0] = 1;

    for (i = 1; i < len; i++)
        vec->content[i] = 0;
}

void generate_ma(char* atts, int len, maparr_t* maps, mat_t* matrix, mat_t** ma)
{
    int i, j, k;
    mat_t* mtmp;
    int row;

    row = 0;
    for (i = 0; i < len; i++)
    {
        for (j = 0; j < maps->len; j++)
        {
            if (atts[i] == maps->content[j].att)
            row++;
        }
    }
   
    mtmp = (mat_t*)malloc(sizeof(mat_t));
    mtmp->column = matrix->column;
    mtmp->row = row;
    mtmp->content = (int**)malloc(sizeof(int*) * mtmp->row);
    for (i = 0; i < mtmp->row; i++)
        mtmp->content[i] = (int*)malloc(sizeof(int) * mtmp->column);
    
    for (i = 0, k = 0; i < len; i++)
    {
        for (j = 0; j < maps->len; j++)
        {
            if (atts[i] == maps->content[j].att)
            {
                memcpy(mtmp->content[k], matrix->content[maps->content[j].index], sizeof(int)* matrix->column);
                k++;
            }
        }
    }
    *ma = transposition_matrix(mtmp);
    free_mat(mtmp);
}

int generate_omega(mat_t* ma, vec_t** omega)
{
    vec_t epsilon;
    
    init_epsilon(&epsilon, ma->row);
    augmented_matrix(ma, &epsilon);
    
    return mat_gauss(ma, omega);
}

void print_map(maparr_t* maps)
{
    int i;
    
    for (i = 0; i < maps->len; i++)
        printf("(%c, %d)\n", maps->content[i].att, maps->content[i].index);
}