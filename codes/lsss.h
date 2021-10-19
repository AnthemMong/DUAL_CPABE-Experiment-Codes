# ifndef LSSS_H
# define LSSS_H
# include"lmatrix.h"

# define LCHILD(x) (2*(x)+1)
# define RCHILD(x) (2*(x)+2)

typedef struct map
{
    char att;
    int index;
} map_t;

typedef struct maparr
{
    int len;
    map_t* content;
} maparr_t;

int access_matrix(char* bintree, int len, mat_t* matrix, maparr_t* maps);
void print_map(maparr_t* maps);
void init_epsilon(vec_t* vec, int len);
void generate_ma(char* atts, int len, maparr_t* maps, mat_t* matrix, mat_t** ma);
int generate_omega(mat_t* ma, vec_t** omega);

# endif