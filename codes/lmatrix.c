# include<stdio.h>
# include<stdlib.h>
# include<string.h>
# include"lmatrix.h"

static void mat_row_muladd(mat_t* matrix, int des, int src, int m);
static int first_none_zero(mat_t* matrix, int row);

int mat_malloc(mat_t* matrix, int row, int column)
{
    int i, j;

    if (row > 0 && column > 0)
    {
        matrix->row = row;
        matrix->column = column;
        if ((matrix->content = (int**)malloc(row * sizeof(int*))) == NULL)
            return 0;
        for (i = 0; i < row; i++)
        {
            if ((matrix->content[i] = (int*)malloc(column * sizeof(int))) == NULL)
                return 0;
        }
        return 1;
    }
    return 0;
}

int vec_malloc(vec_t* vec, int len)
{
    if (len > 0)
    {
        vec->len = len;
        if ((vec->content = (int*)malloc(sizeof(int) * len)) == NULL)
            return 0;
        return 1;
    }
    return 1;
}

void free_mat(mat_t* matrix)
{
    int i;
    for (i = 0; i < matrix->row; i++)
        free(matrix->content[i]);
    free(matrix->content);
}

void free_vec(vec_t* vec)
{
    free(vec->content);
}

void set_mat(mat_t* matrix, int n)
{
    int i, j;

    if (matrix->content != NULL)
    {
        for (i = 0; i < matrix->row; i++)
        {
            for (j = 0; j < matrix->column; j++)
                matrix->content[i][j] = 0;
        }
    }
}

void set_vec(vec_t* vec, int n)
{
    int i;

    if (vec->content != NULL)
    {
        for (i = 0; i < vec->len; i++)
            vec->content[i] = n;
    }
}

void print_mat(mat_t* matrix)
{
    int i, j;
    for (i = 0; i < matrix->row; i++)
    {
        printf("[");
        for (j = 0; j < matrix->column; j++)
        {
            if (j == matrix->column -1)
                printf("%d]\n", matrix->content[i][j]);
            else
                printf("%d, ", matrix->content[i][j]);
        }
    }
}

void print_vec(vec_t* vec)
{
    int i;
    
    printf("[");
    for (i = 0; i < vec->len; i++)
    {
        if (i == vec->len-1)
                printf("%d]\n", vec->content[i]);
            else
                printf("%d, ", vec->content[i]);
    }
}

void mat_input(mat_t* matrix)
{
    int i, j;

    printf("Row and Column:");
    scanf("%d %d", &(matrix->row), &(matrix->column));
    
    mat_malloc(matrix, matrix->row, matrix->column);

    for (i = 0; i < matrix->row; i++)
    {
        for (j = 0; j < matrix->column; j++)
            scanf("%d", &(matrix->content[i][j]));
    }
}

//solving the linear equations employing the guass method
int mat_gauss(mat_t* matrix, vec_t** vec)
{
    int last, last1;
    int i, j, k, l;
    int m;
    int t;

    //transform the matrix to upper triangular matrix
    last = matrix->column;
    last1 = matrix->column - 1;
    for (i = 0; i < matrix->row; i++)
    {
        for (k = 0; k < last1; k++)
        {
            j = 0;
            if (i == k && matrix->content[i][i] == 0)
            {
                for (j = i + 1; j < matrix->row; j++)
                {
                    if (matrix->content[j][i] != 0)
                    {
                        swap_row(matrix, i, j);
                        break;
                    }   
                }   
            }

           if (matrix->content[i][k] != 0)
           {
               for (l = 0; l < i; l++)
               {
                   if (first_none_zero(matrix, l) == k)
                   {
                       m = -(matrix->content[i][k]/matrix->content[l][k]);
                       mat_row_muladd(matrix, i, l, m);
                   }
               }
           }
           if (j == matrix->row)
                break;
        }
    }

    //transform the matrix to diagonal matrix
    for (i = 0; i < matrix->row; i++)
    {
        for (k = 0; k < last1; k++)
        {
            if (matrix->content[i][k] != 0)
            {
                for (j = i + 1; j < matrix->row; j++)
                {
                    if (first_none_zero(matrix, j) == k)
                    {
                        m = -(matrix->content[i][k]/matrix->content[j][k]);
                        mat_row_muladd(matrix, i, j, m);
                    }
                }
            }
        }
    }
    
    //obtin the solution
    (*vec) = (vec_t*)malloc(sizeof(vec_t));
    (*vec)->len = last1;
    (*vec)->content = (int*)malloc(sizeof(int) * (*vec)->len);
    set_vec((*vec), 0);

    for (i = 0; i < matrix->row; i++)
    {
        if ((t = first_none_zero(matrix, i)) == last1)
                return 0;
        if (t != -1)
            (*vec)->content[t] = matrix->content[i][last1]/matrix->content[i][t];
    }

    return 1;
}

void swap_row(mat_t* matrix, int c1, int c2)
{
    int* temp;

    if ((temp = malloc(sizeof(int) * matrix->column)) == NULL)
        exit(0);
    memcpy(temp, matrix->content[c1], sizeof(int) * matrix->column);
    memcpy(matrix->content[c1], matrix->content[c2], sizeof(int) * matrix->column);
    memcpy(matrix->content[c2], temp, sizeof(int) * matrix->column);
}

int augmented_matrix(mat_t* matrix, vec_t* vec)
{
    int *tmp;
    int i;

    if (matrix->row != vec->len)
        return 0;

    tmp = (int*)malloc(sizeof(int) * matrix->column);
    for (i = 0; i < matrix->row; i++)
    {
        memcpy(tmp, matrix->content[i], sizeof(int) * matrix->column);
        matrix->content[i] = (int*)realloc(matrix->content[i], sizeof(int) * (matrix->column + 1));
        memcpy(matrix->content[i], tmp, sizeof(int) * matrix->column);
        matrix->content[i][matrix->column] = vec->content[i];
    }
    matrix->column++;
    return 1;
}

mat_t* transposition_matrix(mat_t* matrix)
{
    int i, j;
    mat_t* mt;
    
    mt = (mat_t*)malloc(sizeof(mat_t));
    mt->row = matrix->column;
    mt->column = matrix->row;
    mt->content = (int**)malloc(sizeof(int*) * mt->row);
    for (i = 0; i < mt->row; i++)
        mt->content[i] = (int*)malloc(sizeof(int) * mt->column);

    for (i = 0; i < matrix->row; i++)
    {
        for (j = 0; j < matrix->column; j++)
            mt->content[j][i] = matrix->content[i][j];
    }
    return mt;
}

//des = des += m * src
static void mat_row_muladd(mat_t* matrix, int des, int src, int m)
{
    int i;

    for (i = 0; i < matrix->column; i++)
        matrix->content[des][i] += matrix->content[src][i] * m;
}

static int first_none_zero(mat_t* matrix, int row)
{
    int i;

    for (i = 0; i < matrix->column; i++)
    {
        if (matrix->content[row][i] == 0)
            continue;
        return i;
    }
    return -1;
}