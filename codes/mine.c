//2021-08-10
//compaile: gcc mine.c lmatrix.c lsss.c laes.c -o mine -lpbc -lgmp -lcrypto
//execute: ./mine a.param
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
void M1(char* ssk1, char* ssk2, int len, element_t* e);
void M2(char* ssk1, char* ssk2, int len, element_t* e);
void H1(char* ssk, int len, element_t* e);

int main(int argc, char* argv[])
{
    mat_t m;
    vec_t* omega;
    mat_t* ma;
    maparr_t maps;
    int len, n, rflag;
    int i, j, l;
    int* I;

    // char* bintree = "&A|^^|&^^^^&&||^^^^^^^^BCDEFHIJ";
    char* bintree = "&|&||||||||||||||||||||||||||||ABCDEFGHIJKLMNOPQRSTUVWXabcdefgh";
    char* U = "ABCDEFGHIJKLMNOPQRSTUVWXabcdefghijklmnopqrstuvwx";
    char* S = "AReFBCDEGHIJKLMNOPQSTUVWXabcdfgh";
    char* msg = "This is the user decrypted data. Hello ABE";

    char sourcedata[MSG_LEN];
    char encrypteddata[MSG_LEN];
    char encrypteddatap[MSG_LEN];
    char userdecrypted[MSG_LEN];
    char ssk1[AES_128/8];
    char ssk2[AES_128/8];
    char ssk1p[AES_128/8];
    char ssk2p[AES_128/8];
    char ssk1ta[AES_128/8];
    char ssk2ta[AES_128/8];
    char temp[AES_128/8];

    pairing_t pairing;
    double time1, time2, time3;
    element_t* hi, *v, *lamdai;
    element_t alpha, a, s, t, z, iz, r, negr, beta, Ztemp1, Ztemp2, Ztemp3;
    element_t g, msk, P2, K, L, *Kx, Cp, *Cl, *Cpl, *Dl, SG, G1temp1, G1temp2;
    element_t P1, C, *MU, TP, GTtemp1, GTtemp2, GTtemp3, GTtemp4;

    pbc_demo_pairing_init(pairing, argc, argv);

    //if there are three parameters and the last one is "r" , it means the program should excutes revocadtion operation.
    if (argc == 2)
        rflag = 0;
    else
    {
        if (strncmp(argv[2], "r", 1) == 0)
            rflag = 1;
        else
            rflag = 0;
    }

    element_init_Zr(alpha, pairing);
    element_init_Zr(a, pairing);
    element_init_Zr(s, pairing);
    element_init_Zr(t, pairing);
    element_init_Zr(z, pairing);
    element_init_Zr(iz, pairing);
    element_init_Zr(r, pairing);
    element_init_Zr(negr, pairing);
    element_init_Zr(beta, pairing);
    element_init_Zr(Ztemp1, pairing);
    element_init_Zr(Ztemp2, pairing);
    element_init_Zr(Ztemp3, pairing);

    element_init_G1(g, pairing);
    element_init_G1(msk, pairing);
    element_init_G1(P2, pairing);
    element_init_G1(K, pairing);
    element_init_G1(L, pairing);
    element_init_G1(Cp, pairing);
    element_init_G1(SG, pairing);
    element_init_G1(G1temp1, pairing);
    element_init_G1(G1temp2, pairing);

    element_init_GT(P1, pairing);
    element_init_GT(C, pairing);
    element_init_GT(TP, pairing);
    element_init_GT(GTtemp1, pairing);
    element_init_GT(GTtemp2, pairing);
    element_init_GT(GTtemp3, pairing);
    element_init_GT(GTtemp4, pairing);

# ifdef DEBUG
    printf("init...\n");
# endif
    time1 = pbc_get_time();

    element_random(alpha);
    element_random(a);
    element_random(g);

    element_pairing(GTtemp1, g, g);
    element_pow_zn(P1, GTtemp1, alpha);
    element_pow_zn(P2, g, a);
    element_pow_zn(msk, g, alpha);

    time3 = pbc_get_time();
    hi = (element_t*)malloc(sizeof(element_t) * strlen(U));
    for (i = 0; i < strlen(U); i++)
    {
        element_init_G1(hi[i], pairing);
        element_random(hi[i]);
    }

    time2 = pbc_get_time();

# ifdef DEBUG
    printf("the time of setup phase %fs\n", time2 - time1);
    printf("the time of init %fs\n", time2 - time3);
# endif

# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif 

# ifdef DEBUG
    printf("user encryption...\n");
# endif 
    time1 = pbc_get_time();

    memset((char*)sourcedata, 0 ,MSG_LEN);
    memset((char*)encrypteddata, 0 ,MSG_LEN);
    strcpy((char*)sourcedata, msg);
    random_key(ssk1, AES_128/8);

    //print_hex(ssk1, AES_128/8);

    if(!aes_encrypt(sourcedata, ssk1, encrypteddata, MSG_LEN, AES_128))
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
            //element_printf("s: %B\n", s);
        }
        else
            element_random(v[i]);
        //element_printf("v: %B\n", v[i]);
    }

    lamdai = (element_t*)malloc(sizeof(element_t) * m.row);
    produce_lamda(lamdai, v, &m, &pairing);
    // for (i = 0; i < m.row; i++)
    //     element_printf("lamda %B\n", lamdai[i]);
    
    M1(ssk1, ssk1, AES_128/8, &GTtemp1);
    //element_printf("M1: %B\n", GTtemp1);
    element_pow_zn(GTtemp2, P1, s);
    element_mul(C, GTtemp1, GTtemp2);
    //element_printf("C: %B\n", C);

    element_pow_zn(Cp, g, s);

    Cl = (element_t*)malloc(sizeof(element_t) * l);
    for (i = 0; i < l; i++)
    {
        element_init_G1(Cl[i], pairing);
        element_pow_zn(Cl[i], P2, lamdai[i]);
    }

    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of user encryption phase %fs\n", time2 - time1);
# endif 

# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif 

# ifdef DEBUG
    printf("ta encryption...\n");
# endif
    time1 = pbc_get_time();

    Cpl = (element_t*)malloc(sizeof(element_t) * l);
    Dl = (element_t*)malloc(sizeof(element_t) * l);

    for (i = 0; i < l; i++)
    {
        element_random(r);
        element_init_G1(Cpl[i], pairing);
        element_init_G1(Dl[i], pairing);
        element_neg(negr, r);

        element_pow_zn(G1temp1, hi[maps.content[i].index], negr);
        element_mul(Cpl[i], Cl[i], G1temp1);

        element_pow_zn(Dl[i], g, r);
    }

    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of ta encryption phase %fs\n", time2 - time1);
# endif

# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif

# ifdef DEBUG 
    printf("att-key generation...\n");
# endif
    time1 = pbc_get_time();

    element_random(t);
    element_random(z);

    element_invert(iz, z);
    element_mul(Ztemp1, alpha, iz);
    element_pow_zn(G1temp1, g, Ztemp1);
    element_pow_zn(G1temp2, P2, t);
    element_mul(K, G1temp1, G1temp2);

    element_pow_zn(L, g, t);

    I = (int *)malloc(sizeof(int) * strlen(S));
    for (i = 0; i < strlen(S); i++)
    {
        for (j = 0; j < maps.len; j++)
        {
            if (S[i] == maps.content[j].att)
                I[i] = maps.content[j].index;
        }
    }

    Kx = (element_t*)malloc(sizeof(element_t) * strlen(S));
    for (i = 0; i < strlen(S); i++)
    {
        element_init_G1(Kx[i], pairing);
        element_pow_zn(Kx[i], hi[I[i]], t); 
    }

    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of att-key generation phase %fs\n", time2 - time1);
# endif

# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif

# ifdef DEBUG
    printf("cloud init...\n");
# endif
    time1 = pbc_get_time();

    element_random(beta);
    element_pow_zn(SG, g, beta);

    memset(ssk2, '0', AES_128/8);

    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of cloud init phase %fs\n", time2 - time1);
# endif

    if (rflag)
    {
# ifdef DEBUG
        printf("symmetry key generation and encryption...\n");
# endif
        time1 = pbc_get_time();

        if (strncmp(ssk2, "0000000000000000", AES_128/8) == 0)
        {
            element_mul(Ztemp1, alpha, beta);
            element_pow_zn(G1temp1, g, Ztemp1);
            H1(ssk2, AES_128/8, &G1temp1);
            aes_encrypt(encrypteddata, ssk2, encrypteddatap, MSG_LEN, AES_128);
            //print_hex(ssk2, AES_128/8);
        }
        else
        {
            aes_decrypt(encrypteddatap, ssk2, encrypteddata, MSG_LEN, AES_128);
            element_mul(Ztemp1, alpha, beta);
            element_pow_zn(G1temp1, g, Ztemp1);
            H1(ssk2, AES_128/8, &G1temp1);
            aes_encrypt(encrypteddata, ssk2, encrypteddatap, MSG_LEN, AES_128);
        }

        time2 = pbc_get_time();
# ifdef DEBUG
        printf("the time of symmetry key generation and encryption phase %fs\n", time2 - time1);
# endif
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif
    }

    if (rflag)
    {
# ifdef DEBUG
        printf("ta re-encrytpion...\n");
# endif
        time1 = pbc_get_time();

        element_pow_zn(G1temp1, SG, alpha);
        H1(ssk2ta, AES_128/8, &G1temp1);

        element_pairing(GTtemp1, Cp, msk);
        element_div(GTtemp2, C, GTtemp1);
        M2(ssk1ta, temp, AES_128/8, &GTtemp2);

        M1(ssk1ta, ssk2ta, AES_128/8, &GTtemp3);
        //element_pairing(GTtemp2, Cp, msk);
        element_mul(C, GTtemp3, GTtemp1);

        time2 = pbc_get_time();
# ifdef DEBUG
        printf("the time of ta re-encrytpion phase %fs\n", time2 - time1);
# endif
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif
    } 
# ifdef DEBUG
    printf("cloud server decryption...\n");
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
        element_pairing(GTtemp2, Cpl[I[i]], L);
        element_pairing(GTtemp3, Dl[I[i]], Kx[i]);
        element_mul(GTtemp4, GTtemp2, GTtemp3);
        element_set_si(Ztemp1, omega->content[i]);
        element_pow_zn(MU[i], GTtemp4, Ztemp1);
    }

    element_set(GTtemp2, MU[0]);
    for (i = 1; i < strlen(S); i++)        
        element_mul(GTtemp2, GTtemp2, MU[i]);

    element_div(TP, GTtemp1, GTtemp2);
    //element_printf("TP: %B\n", TP);

    time2 = pbc_get_time();
# ifdef DEBUG
    printf("the time of ta decryption phase %fs\n", time2 - time1);
# endif
# ifdef ST
    printf("%.2f, ", 1000 * (time2 - time1));
# endif

# ifdef DEBUG
    printf("user decryption...\n");
# endif
    time1 = pbc_get_time();

    element_pow_zn(GTtemp1, TP, z);
    element_div(GTtemp2, C, GTtemp1);

    M2(ssk1p, ssk2p, AES_128/8, &GTtemp2);
    // print_hex(ssk1p, AES_128/8);
    // print_hex(ssk2p, AES_128/8);
    if (strncmp(ssk1p, ssk2p, AES_128/8) == 0)
    {
        //printf("same\n");
        aes_decrypt(encrypteddata, ssk1p, userdecrypted, MSG_LEN, AES_128);
        printf("%s\n", userdecrypted);
    }
    else
    {
        //printf("not same\n");
        aes_decrypt(encrypteddatap, ssk2p, encrypteddata, MSG_LEN, AES_128);
        aes_decrypt(encrypteddata, ssk1p, userdecrypted, MSG_LEN, AES_128);
# ifdef DEBUG
        printf("%s\n", userdecrypted);
# endif
    }

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

void M1(char* ssk1, char* ssk2, int len, element_t* e)
{   
    int i, n;
    char* T, *T1;

    if (element_length_in_bytes(*e) % ((AES_128/8) * 2) != 0)
    {
        printf("error!\n");
        exit(0);
    }

    n = element_length_in_bytes(*e) / ((AES_128/8) * 2);
    T1 = (char*)malloc(sizeof(char) * len * 2);
    strncpy(T1, ssk1, len);
    strncpy(T1 + len, ssk2, len);

    T = (char*)malloc(sizeof(char) * element_length_in_bytes(*e));
    for (i = 0; i < n; i++)
        strncpy(T + 2 * i * len, T1, 2 * len);

    element_from_bytes(*e, T);
    free(T);
    free(T1);
}

void M2(char* ssk1, char* ssk2, int len, element_t* e)
{
    int i, n;
    char* T;

    if (element_length_in_bytes(*e) % ((AES_128/8) * 2) != 0)
    {
        printf("error!\n");
        exit(0);
    }

    T = (char*)malloc(sizeof(char) * element_length_in_bytes(*e));
    
    element_to_bytes(T, *e);

    strncpy(ssk1, T, len);
    strncpy(ssk2, T+len, len);
    free(T);
}

void H1(char* ssk, int len, element_t* e)
{
    char* T;

    T = (char*)malloc(sizeof(char) * element_length_in_bytes(*e));
    element_to_bytes(T, *e);
    strncpy(ssk, T, len);

    free(T);
}

    // len = access_matrix(bintree, strlen(bintree), &mat, &maps);
    // print_mat(&mat);
    // print_map(&maps);
    // generate_ma("EG", 2, &maps, &mat, &ma);
    // print_mat(ma);
    // if (generate_omega(ma, &vec))
    //     print_vec(vec);
    // else
    //     printf("none\n");