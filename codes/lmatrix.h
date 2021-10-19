# ifndef LMATRIX_H
# define LMATRIX_H

typedef struct mat
{
    int row;
    int column;
    int** content;
} mat_t;

typedef struct vec
{
    int len;
    int* content;
} vec_t;

int mat_malloc(mat_t* matrix, int row, int column);
int vec_malloc(vec_t* vec, int len);
void free_mat(mat_t* matrix);
void free_vec(vec_t* vec);
void mat_set(mat_t* matrix, int n);
void vec_set(vec_t* vec, int n);
void print_mat(mat_t* matrix);
void print_vec(vec_t* vec);
void mat_input(mat_t* matrix);

int mat_gauss(mat_t* matrix, vec_t** vec);
void swap_row(mat_t* matrix, int c1, int c2);
int augmented_matrix(mat_t* matrix, vec_t* vec);
mat_t* transposition_matrix(mat_t* matrix);

# endif