//2021-08-24
//Revocable attribute-based data storage in mobile clouds
//compaile: gcc deng.c lmatrix.c lsss.c laes.c -o deng -lpbc -lgmp -lcrypto
//execute: ./deng a.param v m [r]
# include<stdio.h>
# include<stdlib.h>
# include<string.h>
# include<pbc/pbc.h>
# include<pbc/pbc_test.h>
# include"lmatrix.h"
# include"lsss.h"
# include"laes.h"

# define MSG_LEN 128

void produce_lamda(element_t* lamdai, element_t* v, mat_t* m, pairing_t* pairing);
void H0(char x, element_t* e);
void H1(element_t* e1, char* m, int len,  element_t* e2);
void H2(element_t* e, char* r, int len);
void strxor(char*str1, char* str2, char* str3, int len);

int main(int argc, char* argv[])
{
    mat_t A;
    vec_t* omega;
    mat_t* ma;
    maparr_t maps;

    int len, n, l, rflag;
    int* I;

    // char* bintree = "&A|^^|&^^^^&&||^^^^^^^^BCDEFHIJ";
    char* bintree = "&|&||||||||||||||||||||||||||||ABCDEFGHIJKLMNOPQRSTUVWXabcdefgh";
    char* U = "ABCDEFGHIJKLMNOPQRSTUVWXabcdefghijklmnopqrstuvwx";
    char* S = "AReF";
    char* msg = "This is the user decrypted data. Hello ABE";

    char sourcedata[MSG_LEN];
    char encrypteddata[MSG_LEN];
    char userdecrypted[MSG_LEN]; 
    char M[AES_128/8];
    char r[AES_128/8];
    char C2[AES_128/8];

    char Ur[AES_128/8];
    char UM[AES_128/8];
    
    pairing_t pairing;
    double time1, time2, time3, time4;
    int i, j;
    int v, m;

    pbc_demo_pairing_init(pairing, argc, argv);

    if (argc < 4)
    {
        printf("parameter error\n");
        exit(0);
    }
    else 
    {
        if (argc == 4)
            rflag = 0;
        else
            rflag = 1;
        v = atoi(argv[2]);
        m = atoi(argv[3]);
    }

    element_t* V, *h, *ht, *sv, *lamdai, *hs;
    element_t alpha, t, z, iz, s, *rl, Us, Ztemp1, Ztemp2;
    element_t g, u, f, msk, K, L, *Kx, C1, C3, *Di, *Ei, G1temp1, G1temp2, G1temp3, G1temp4;
    element_t P1, R, C0, *MU, Mp, UR, T2, GTtemp1, GTtemp2, GTtemp3, GTtemp4;

    element_init_Zr(alpha, pairing);
    element_init_Zr(t, pairing);
    element_init_Zr(z, pairing);
    element_init_Zr(s, pairing);
    element_init_Zr(iz, pairing);
    element_init_Zr(Us, pairing);
    element_init_Zr(Ztemp1, pairing);
    element_init_Zr(Ztemp2, pairing);

    element_init_G1(g, pairing);
    element_init_G1(u, pairing);
    element_init_G1(f, pairing);
    element_init_G1(msk, pairing);
    element_init_G1(K, pairing);
    element_init_G1(L, pairing);
    element_init_G1(C1, pairing);
    element_init_G1(C3, pairing);
    element_init_G1(G1temp1, pairing);
    element_init_G1(G1temp2, pairing);
    element_init_G1(G1temp3, pairing);
    element_init_G1(G1temp4, pairing);

    element_init_GT(P1, pairing);
    element_init_GT(R, pairing);
    element_init_GT(C0, pairing);
    element_init_GT(Mp, pairing);
    element_init_GT(UR, pairing);
    element_init_GT(T2, pairing);
    element_init_GT(GTtemp1, pairing);
    element_init_GT(GTtemp2, pairing);
    element_init_GT(GTtemp3, pairing);
    element_init_GT(GTtemp4, pairing);

# ifdef DEBUG
    printf("setup...\n");
# endif
    time1 = pbc_get_time();

    element_random(alpha);
    
    element_random(g);
    element_random(u);
    element_random(f);

    element_pairing(GTtemp1, g, g);
    element_pow_zn(P1, GTtemp1, alpha);

    element_pow_zn(msk, g, alpha);

    time3 = pbc_get_time();
    V = (element_t*)malloc(sizeof(element_t) * v);
    for (i = 0; i < v; i++)
    {
        element_init_Zr(V[i], pairing);
        element_random(V[i]);
    }

    h = (element_t*)malloc(sizeof(element_t) * m);
    for (i = 0; i < m; i++)
    {
        element_init_G1(h[i], pairing);
        element_random(h[i]);
    }

    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of setup phase %fs\n", time2 - time1);
    printf("the time of elements initialization %fs\n", time2 - time3);
# endif
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif 
# ifdef DEBUG
    printf("user key generation...\n");
# endif
    time1 = pbc_get_time();

    element_random(t);
    element_random(z);

    element_invert(iz, z);
    element_mul(Ztemp1, alpha, iz);
    element_pow_zn(G1temp1, g, Ztemp1);

    element_pow_zn(G1temp2, u, t);

    element_pow_zn(G1temp3, h[0], V[0]);
    for (i = 1; i < v; i++)
    {
        element_pow_zn(G1temp4, h[i], V[i]);
        element_mul(G1temp3, G1temp3, G1temp4);
    }

    element_mul(G1temp3, G1temp3, f);
    element_pow_zn(G1temp3, G1temp3, t);

    element_mul(G1temp4, G1temp1, G1temp2);
    element_mul(K, G1temp4, G1temp3);

    element_pow_zn(L, g, t);

    ht = (element_t*)malloc(sizeof(element_t) * (m-v));
    for (i = 0; i < (m-v); i++)
    {
        element_init_G1(ht[i], pairing);
        element_pow_zn(ht[i], h[v+i], t);
    }

    Kx = (element_t*)malloc(sizeof(element_t) * strlen(S));
    for (i = 0; i < strlen(S); i++)
    {
        element_init_G1(Kx[i], pairing);
        H0(S[i], &G1temp1);
        element_pow_zn(Kx[i], G1temp1, t);
        //element_printf("%B\n", Kx[i]);
    }

    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of key generation phase %fs\n", time2 - time1);
# endif 
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif 
# ifdef DEBUG
    printf("owner encryption...\n");
# endif 
    time1 = pbc_get_time();

    memset((char*)sourcedata, 0 ,MSG_LEN);
    memset((char*)encrypteddata, 0 ,MSG_LEN);
    strcpy((char*)sourcedata, msg);
    random_key(M, AES_128/8);

    //print_hex(M, AES_128/8);

    if(!aes_encrypt(sourcedata, M, encrypteddata, MSG_LEN, AES_128))
    {
        printf("encrypt error\n");
        return -1;
    }

    element_random(R);
    //element_printf("R: %B\n", R);
    H1(&R, M, AES_128/8, &s);
    //element_printf("%B\n", s);
    H2(&R, r, AES_128/8);

    len = access_matrix(bintree, strlen(bintree), &A, &maps);
    // print_map(&maps);
    // print_mat(&A);
    n = A.column;
    l = A.row;

    sv = (element_t*)malloc(sizeof(element_t) * n);

    for (i = 0; i < n; i++)
    {
        element_init_Zr(sv[i], pairing);
        if (i == 0)
        {
            element_set(sv[i], s);
            //element_printf("s: %B\n", s);
        }
        else
            element_random(sv[i]);
    }

    lamdai = (element_t*)malloc(sizeof(element_t) * A.row);
    produce_lamda(lamdai, sv, &A, &pairing);

    rl = (element_t*)malloc(sizeof(element_t) * l);
    for (i = 0; i < l; i++)
    {
        element_init_Zr(rl[i], pairing);
        element_random(rl[i]);
    }

    element_pow_zn(GTtemp1, P1, s);
    element_mul(C0, R, GTtemp1);

    element_pow_zn(C1, g, s);

    strxor(M, r, C2, AES_128/8);

    element_pow_zn(G1temp1, h[0], V[0]);
    for (i = 1; i < v; i++)
    {
        element_pow_zn(G1temp2, h[i], V[i]);
        element_mul(G1temp1, G1temp1, G1temp2);
    }
    element_mul(G1temp3, G1temp1, f);
    element_pow_zn(C3, G1temp3, s);

    hs = (element_t*)malloc(sizeof(element_t) * (m-v));
    for (i = 0; i < (m-v); i++)
    {
        element_init_G1(hs[i], pairing);
        element_pow_zn(hs[i], h[v+i], s);
    }

    Di = (element_t*)malloc(sizeof(element_t) * l);
    Ei = (element_t*)malloc(sizeof(element_t) * l);

    for (i = 0; i < l; i++)
    {
        element_init_G1(Di[i], pairing);
        element_init_G1(Ei[i], pairing);
        element_pow_zn(G1temp1, u, lamdai[i]);
        H0(maps.content[i].att, &G1temp2);
        element_neg(Ztemp1, rl[i]);
        element_pow_zn(G1temp3, G1temp2, Ztemp1);
        element_mul(Di[i], G1temp1, G1temp3);

        element_pow_zn(Ei[i], g, rl[i]);
    }
    
    I = (int *)malloc(sizeof(int) * strlen(S));
    for (i = 0; i < strlen(S); i++)
    {
        for (j = 0; j < maps.len; j++)
        {
            if (S[i] == maps.content[j].att)
                I[i] = maps.content[j].index;
        }
    }
    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of encryption phase %fs\n", time2 - time1);
# endif
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif 

    //revocation
    if (rflag)
    {
# ifdef DEBUG
        printf("sysupdate...\n");
# endif
        time1 = pbc_get_time();

        element_t* Vtemp;

        Vtemp = (element_t*)malloc(sizeof(element_t) * v);
        memcpy(Vtemp, V, sizeof(element_t) * v);
        v++;
        V = realloc(V, sizeof(element_t) * v);
        memcpy(V, Vtemp, sizeof(element_t) * (v-1));
        free(Vtemp);
        element_init_Zr(V[v-1], pairing);
        element_random(V[v-1]);

        //printf("okupdate...\n");
        element_pow_zn(G1temp1, ht[0], V[v-1]);
        element_mul(K, K, G1temp1);

        //printf("re-encryption...\n");
        element_pow_zn(G1temp1, hs[0], V[v-1]);
        element_mul(C3, C3, G1temp1);

        time2 = pbc_get_time();
# ifdef DEBUG
        printf("the time of revocation phase %fs\n", time2 - time1);
# endif
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif 
    }
    
# ifdef DEBUG
    printf("csp decryption...\n");
# endif 
    time1 = pbc_get_time();

    generate_ma(S, strlen(S), &maps, &A, &ma);
    if (generate_omega(ma, &omega) <= 0)
    {
        printf("none\n");
        exit(0);
    }
    // else
    //     print_vec(omega);

    element_pairing(GTtemp1, C1, K);
    //element_printf("C1: %B\n", GTtemp1);


    MU = (element_t*)malloc(sizeof(element_t) * strlen(S));
    for (i = 0; i < strlen(S); i++)
    {
        element_init_GT(MU[i], pairing);
        element_pairing(GTtemp2, L, Di[I[i]]);
        element_pairing(GTtemp3, Ei[I[i]], Kx[i]);
        element_mul(GTtemp4, GTtemp2, GTtemp3);

        element_set_si(Ztemp1, omega->content[i]);
        element_pow_zn(MU[i], GTtemp4, Ztemp1);
    }

    element_set(GTtemp2, MU[0]);
    for (i = 1; i < strlen(S); i++)        
        element_mul(GTtemp2, GTtemp2, MU[i]);
   
    element_pairing(GTtemp3, C3, L);
    element_mul(GTtemp4, GTtemp2, GTtemp3);
    element_div(T2, GTtemp1, GTtemp4);
    //element_printf("T2: %B\n", T2);

    time2 = pbc_get_time();

# ifdef DEBUG 
    printf("the time of csp decryption phase %fs\n", time2 - time1);
# endif
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif
# ifdef DEBUG 
    printf("user decryption...\n");
# endif
    time1 = pbc_get_time();

    element_pow_zn(GTtemp1, T2, z);
    element_div(UR, C0, GTtemp1);

    //element_printf("UR: %B\n", UR);

    H2(&R, Ur, AES_128/8);
    strxor(C2, Ur, UM, AES_128/8);
    
    //print_hex(UM, AES_128/8);

    H1(&UR, UM, AES_128/8, &Us);
    //element_printf("Us: %B\n", Us);

    element_pow_zn(GTtemp1, P1, Us);
    element_mul(GTtemp1, UR, GTtemp1);

    element_invert(iz, z);
    element_mul(Ztemp1, Us, iz);
    element_pow_zn(GTtemp2, P1, Ztemp1);
    
    if (element_cmp(C0, GTtemp1) == 0 && element_cmp(T2, GTtemp2) == 0)
    {
# ifdef DEBUG
        printf("accept!\n");
# endif
        time3 = pbc_get_time();
        aes_decrypt(encrypteddata, UM, userdecrypted, MSG_LEN, AES_128);
# ifdef DEBUG
        printf("%s\n", userdecrypted);
# endif
    }    
    else
        printf("reject!\n");
    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of user decryption phase %fs\n", time2 - time1);
    printf("the time of symmetry decryption %fs\n", time2 - time3);
# endif
# ifdef ST
    printf("%.2f\n", 1000 * (time2 - time1));
# endif
}

void produce_lamda(element_t* lamdai, element_t* v, mat_t* m, pairing_t* pairing)
{
    int i, j;
    element_t me, t;


    element_init_Zr(me, *pairing);
    element_init_Zr(t, *pairing);
    for (i = 0; i < m->row; i++)
    {
        element_init_Zr(lamdai[i], *pairing);
        element_set0(lamdai[i]);
        for (j = 0; j < m->column; j++)
        {
            element_set_si(me, m->content[i][j]);
            element_mul(t, me, v[j]);
            element_add(lamdai[i], lamdai[i], t);
        }
    }
}

void H0(char x, element_t* e)
{
    char* T;
    int i = 1;

    T = (char*)malloc(sizeof(char) * element_length_in_bytes(*e));
    for (i = 0; i < element_length_in_bytes(*e); i++)
        T[i] = x;
    element_from_bytes_compressed(*e, T);
    free(T);
}

void H1(element_t* e1, char* m, int len,  element_t* e2)
{
    char* T1;
    char* T2;

    T1 = (char*)malloc(sizeof(char) * element_length_in_bytes(*e1));
    T2 = (char*)malloc(sizeof(char) * element_length_in_bytes(*e2));

    element_to_bytes(T1, *e1);

    strncpy(T2, T1, element_length_in_bytes(*e2));
    strncpy(T2, m, len);

    element_from_bytes(*e2, T2);

    free(T1);
    free(T2);
}

void H2(element_t* e, char* r, int len)
{
    char* T;

    T = (char*)malloc(sizeof(char) * element_length_in_bytes(*e));
    element_to_bytes(T, *e);
    
    strncpy(r, T, len);
    free(T);
}

void strxor(char*str1, char* str2, char* str3, int len)
{
    int i;

    for(i = 0; i < len; i++)
        str3[i] = str1[i] ^ str2[i];
}