//2021-09-08
//Attribute-based fine-grained access control with efficient revocation in cloud storage systems
//compaile: gcc yang.c lmatrix.c lsss.c laes.c -o yang -lpbc -lgmp -lcrypto
//execute: ./yang a.param v m [r]
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
void M1(char* ssk, int len, element_t* e);
void M2(char* ssk, int len, element_t* e);

int main(int argc, char* argv[])
{
    mat_t m;
    vec_t* omega;
    mat_t* ma;
    maparr_t maps;
    int len, n, rflag;
    int i, j, l, rSindex, rUindex;
    int* I;

    //char* bintree = "&A|^^|&^^^^&&||^^^^^^^^BCDEFHIJ";
        char* bintree = "&|&||||||||||||||||||||||||||||ABCDEFGHIJKLMNOPQRSTUVWXabcdefgh";
    char* U = "ABCDEFGHIJKLMNOPQRSTUVWXabcdefghijklmnopqrstuvwx";
    char* TMP = "AReFBCDEGHIJKLMNOPQSTUVWXabcdfgh";
    char* S;
    char* msg = "This is the user decrypted data. Hello ABE";

    char sourcedata[MSG_LEN];
    char encrypteddata[MSG_LEN];
    char userdecrypted[MSG_LEN];
    char ssk[AES_128/8];
    char Ussk[AES_128/8];
    char revocatedatt = 'F';

    pairing_t pairing;
    double time1, time2;
    element_t alpha, beta, ibeta, gamma, a, *vx, t, *v, *lamdai, s, *rl, vr, UK1, UK2, Ztemp1, Ztemp2, Ztemp3;
    element_t g, P1, P2, P3, *PK1, *PK2, K, L, *Kx, Cp, *Cl, *D1l, *D2l, PKw1, PKw2, G1temp1, G1temp2, G1temp3, G1temp4;
    element_t P4, C, k, *MU, Uk, GTtemp1, GTtemp2, GTtemp3, GTtemp4;

    pbc_demo_pairing_init(pairing, argc, argv);

    if (argc == 2)
        rflag = 0;
    else
    {
        if (strncmp(argv[2], "r", 1) == 0)
            rflag = 1;
        else
            rflag = 0;
    }

    S = (char*)malloc(sizeof(char) * (strlen(TMP) + 1));
    memset(S, 0, sizeof(char) * (strlen(TMP) + 1));
    strncpy(S, TMP, strlen(TMP));

    element_init_Zr(alpha, pairing);
    element_init_Zr(beta, pairing);
    element_init_Zr(ibeta, pairing);
    element_init_Zr(gamma, pairing);
    element_init_Zr(a, pairing);
    element_init_Zr(t, pairing);
    element_init_Zr(s, pairing);
    element_init_Zr(vr, pairing);
    element_init_Zr(UK1, pairing);
    element_init_Zr(UK2, pairing);
    element_init_Zr(Ztemp1, pairing);
    element_init_Zr(Ztemp2, pairing);
    element_init_Zr(Ztemp3, pairing);

    element_init_G1(g, pairing);
    element_init_G1(P1, pairing);
    element_init_G1(P2, pairing);
    element_init_G1(P3, pairing);
    element_init_G1(K, pairing);
    element_init_G1(L, pairing);
    element_init_G1(Cp, pairing);
    element_init_G1(PKw1, pairing);
    element_init_G1(PKw2, pairing);
    element_init_G1(G1temp1, pairing);
    element_init_G1(G1temp2, pairing);
    element_init_G1(G1temp3, pairing);
    element_init_G1(G1temp4, pairing);

    element_init_GT(P4, pairing);
    element_init_GT(C, pairing);
    element_init_GT(k, pairing);
    element_init_GT(Uk, pairing);
    element_init_GT(GTtemp1, pairing);
    element_init_GT(GTtemp2, pairing);
    element_init_GT(GTtemp3, pairing);
    element_init_GT(GTtemp4, pairing);
# ifdef DEBUG
    printf("init...\n");
# endif
    time1 = pbc_get_time();

    element_random(alpha);
    element_random(beta);
    element_random(gamma);
    element_random(a);
    element_random(g);

    element_pow_zn(P1, g, a);

    element_invert(ibeta, beta);
    element_pow_zn(P2, g, ibeta);

    element_pow_zn(P3, g, beta);

    element_pairing(P4, g, g);
    element_pow_zn(P4, P4, alpha);

    vx = (element_t*)malloc(sizeof(element_t) * strlen(U));
    PK1 = (element_t*)malloc(sizeof(element_t) * strlen(U));
    PK2 = (element_t*)malloc(sizeof(element_t) * strlen(U));

    for (i = 0; i < strlen(U); i++)
    {
        element_init_Zr(vx[i], pairing);
        element_init_G1(PK1[i], pairing);
        element_init_G1(PK2[i], pairing);

        element_random(vx[i]);

        H0(U[i], &G1temp1);
        element_pow_zn(PK1[i], G1temp1, vx[i]);

        element_mul(Ztemp1, vx[i], gamma);
        element_pow_zn(PK2[i], G1temp1, Ztemp1);
    }
    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of init phase %fs\n", time2 - time1);
# endif
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif 
# ifdef DEBUG
    printf("user key generation...\n");
# endif
    time1 = pbc_get_time();

    element_random(t);

    element_mul(Ztemp1, alpha, ibeta);
    element_pow_zn(G1temp1, g, Ztemp1);
    element_mul(Ztemp1, a, t);
    element_mul(Ztemp1, Ztemp1, ibeta);
    element_pow_zn(G1temp2, g, Ztemp1);
    element_mul(K, G1temp1, G1temp2);

    element_pow_zn(L, g, t);

    I = (int *)malloc(sizeof(int) * strlen(S));
    for (i = 0; i < strlen(S); i++)
    {
        for (j = 0; j < strlen(U); j++)
        {
            if (S[i] == U[j])
            {
                I[i] = j;
                //printf("%d\n", j);
                break;
            }    
        }
    }

    Kx = (element_t*)malloc(sizeof(element_t) * strlen(S));
    for (i = 0; i < strlen(S); i++)
    {
        element_init_G1(Kx[i], pairing);
        element_mul(Ztemp1, beta, beta);
        element_pow_zn(G1temp1, L, Ztemp1);

        element_mul(Ztemp1, vx[I[i]], t);
        element_mul(Ztemp1, Ztemp1, beta);
        H0(S[i], &G1temp2);
        element_pow_zn(G1temp2, G1temp2, Ztemp1);
        element_mul(Kx[i], G1temp1, G1temp2);
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
    random_key(ssk, AES_128/8);

    if(!aes_encrypt(sourcedata, ssk, encrypteddata, MSG_LEN, AES_128))
    {
        printf("encrypt error\n");
        return -1;
    }

    len = access_matrix(bintree, strlen(bintree), &m, &maps);
    // print_map(&maps);
    // print_mat(&m);
    n = m.column;
    l = m.row;
    v = (element_t*)malloc(sizeof(element_t) * n);

    for (i = 0; i < n; i++)
    {
        element_init_Zr(v[i], pairing);
        if (i == 0)
        {
            element_random(s);
            element_set(v[i], s);
        }
        else
            element_random(v[i]);
    }

    lamdai = (element_t*)malloc(sizeof(element_t) * m.row);
    produce_lamda(lamdai, v, &m, &pairing);

    rl = (element_t*)malloc(sizeof(element_t) * l);
    for (i = 0; i < l; i++)
    {
        element_init_Zr(rl[i], pairing);
        element_random(rl[i]);
    }

    M1(ssk, AES_128/8, &k);
    //element_printf("k: %B\n", k);
    element_pow_zn(GTtemp1, P4, s);
    element_mul(C, k, GTtemp1);

    element_pow_zn(Cp, P3, s);

    Cl = (element_t*)malloc(sizeof(element_t) * l);
    D1l = (element_t*)malloc(sizeof(element_t) * l);
    D2l = (element_t*)malloc(sizeof(element_t) * l);

    for (i = 0; i < l; i++)
    {
        element_init_G1(Cl[i], pairing);
        element_init_G1(D1l[i], pairing);
        element_init_G1(D2l[i], pairing);

        element_pow_zn(G1temp1, P1, lamdai[i]);
        element_neg(Ztemp1, rl[i]);
        element_pow_zn(G1temp2, P3, Ztemp1);
        H0(maps.content[i].att, &G1temp3);
        element_mul(Ztemp2, Ztemp1, vx[maps.content[i].index]);
        element_pow_zn(G1temp4, G1temp3, Ztemp2);
        element_mul(G1temp1, G1temp1, G1temp2);
        element_mul(Cl[i], G1temp1, G1temp4);

        element_mul(Ztemp1, vx[maps.content[i].index], rl[i]);
        element_mul(Ztemp1, Ztemp1, gamma);
        element_pow_zn(D1l[i], G1temp3, Ztemp1);

        element_pow_zn(D2l[i], P2, rl[i]);
    }

    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of owner encryption phase %fs\n", time2 - time1);
# endif
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif
# ifdef DEBUG
    printf("revocation...\n");
# endif
    time1 = pbc_get_time();

    if (rflag)
    {
        for (i = 0; i < strlen(U); i++)
        {
            if (revocatedatt == U[i])
            {
                rUindex = i;
                break;
            }
        }

        for (i = 0; i < strlen(S); i++)
        {
            if (revocatedatt == S[i])
            {
                rSindex = i;
                break;
            }
        }
        
        element_random(vr);

        element_invert(Ztemp1, vx[rUindex]);
        element_mul(UK1, vr, Ztemp1);

        element_mul(Ztemp1, vx[rUindex], gamma);
        element_invert(Ztemp2, Ztemp1);
        element_neg(Ztemp1, vr);
        element_add(Ztemp3, vx[rUindex], Ztemp1);
        element_mul(UK2, Ztemp3, Ztemp2);

        H0(revocatedatt, &G1temp1);
        element_pow_zn(PKw1, G1temp1, vr);

        element_mul(Ztemp1, vr, gamma);
        element_pow_zn(PKw2, G1temp1, Ztemp1);

        element_mul(Ztemp1, beta, beta);
        element_pow_zn(G1temp1, L, Ztemp1);
        element_div(G1temp2, Kx[rSindex], G1temp1);
        element_pow_zn(Kx[rSindex], G1temp2, UK1);

        char* tmp = (char*)malloc(sizeof(char) * (strlen(S)));
        memset(tmp, 0, sizeof(char) * strlen(S));
        for (i = 0, j = 0; i < strlen(S); i++)
        {
            if (S[i] != revocatedatt)
                tmp[j++] = S[i];
        }
        S = tmp;
       
       for (i = 0; i < l; i++)
       {
           if (maps.content[i].att == revocatedatt)
           {
               element_pow_zn(G1temp1, D1l[i], UK2);
               element_mul(Cl[i], Cl[i], G1temp1);

               element_pow_zn(D1l[i], D1l[i], UK1);
           }
       }
    }

    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of revocation phase %fs\n", time2 - time1);
# endif
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif
# ifdef DEBUG
    printf("user decryptioin...\n");
# endif
    time1 = pbc_get_time();

    generate_ma(S, strlen(S), &maps, &m, &ma);
    if (generate_omega(ma, &omega) <= 0)
    {
        printf("none\n");
        exit(0);
    }
    // else
    //     print_vec(omega);

    element_pairing(GTtemp1, Cp, K);

    MU = (element_t*)malloc(sizeof(element_t) * strlen(S));
    for (i = 0; i < strlen(S); i++)
    {
        element_init_GT(MU[i], pairing);
        element_pairing(GTtemp2, L, Cl[I[i]]);
        element_pairing(GTtemp3, D2l[I[i]], Kx[i]);
        element_mul(GTtemp4, GTtemp2, GTtemp3);
        element_set_si(Ztemp1, omega->content[i]);
        element_pow_zn(MU[i], GTtemp4, Ztemp1);
    }

    element_set(GTtemp2, MU[0]);
    for (i = 1; i < strlen(S); i++)        
        element_mul(GTtemp2, GTtemp2, MU[i]);

    element_div(GTtemp3, GTtemp1, GTtemp2);
    element_div(Uk, C, GTtemp3);

    M2(Ussk, AES_128/8, &Uk);
    aes_decrypt(encrypteddata, Ussk, userdecrypted, MSG_LEN, AES_128);
# ifdef DEBUG
    printf("%s\n", userdecrypted);
# endif

    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of user decryption phase %fs\n", time2 - time1);
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

void M1(char* ssk, int len, element_t* e)
{   
    int i, n;
    char* T;

    if (element_length_in_bytes(*e) % (AES_128/8) != 0)
    {
        printf("error!\n");
        exit(0);
    }

    n = element_length_in_bytes(*e) / (AES_128/8);
    T = (char*)malloc(sizeof(char) * element_length_in_bytes(*e));
    for (i = 0; i < n; i++)
        strncpy(T + i * len, ssk, len);

    element_from_bytes(*e, T);
    free(T);
}

void M2(char* ssk, int len, element_t* e)
{
    int i, n;
    char* T;

    if (element_length_in_bytes(*e) % (AES_128/8) != 0)
    {
        printf("error!\n");
        exit(0);
    }

    T = (char*)malloc(sizeof(char) * element_length_in_bytes(*e));
    element_to_bytes(T, *e);
    strncpy(ssk, T, len);
    free(T);
}