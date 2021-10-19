#include <stdio.h>
#include <gsl/gsl_matrix.h>

void gsl_matrix_inv(gsl_matrix *a)

int
main (void)
{
    double x = 5.0;
    double y = gsl_sf_bessel_J0 (x);
    printf ("J0(%g) = %.18e/n", x, y);
    return 0;
}


void gsl_matrix_inv(gsl_matrix *a)
{
	size_t n=a->size1;
	size_t m=a->size2;
 
	gsl_matrix *temp1=gsl_matrix_calloc(n,n);
	gsl_matrix_memcpy(temp1,a);
 
	gsl_permutation *p=gsl_permutation_calloc(n);
	int sign=0;
	gsl_linalg_LU_decomp(temp1,p,&sign);
	gsl_matrix *inverse=gsl_matrix_calloc(n,n);
 
	gsl_linalg_LU_invert(temp1,p,inverse);
	gsl_matrix_memcpy(a,inverse);
 
	gsl_permutation_free(p);
	gsl_matrix_free(temp1);
	gsl_matrix_free(inverse);
}