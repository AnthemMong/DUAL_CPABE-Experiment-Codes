# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include<time.h>
# include <openssl/aes.h>

# define AES_BITS 128
# define MSG_LEN 128

int aes_encrypt(char* in, char* key, char* out, int msglen, int keybits)
{
    unsigned char iv[AES_BLOCK_SIZE];
    AES_KEY aes;

    if(!in || !key || !out) 
        return 0;
    
    for(int i=0; i<AES_BLOCK_SIZE; ++i)
        iv[i]=0;
    
    if(AES_set_encrypt_key((unsigned char*)key, keybits, &aes) < 0)
        return 0;
    
    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, msglen, &aes, iv, AES_ENCRYPT);
    return 1;
}

int aes_decrypt(char* in, char* key, char* out, int msglen, int keybits)
{
    unsigned char iv[AES_BLOCK_SIZE];
    AES_KEY aes;

    if(!in || !key || !out) 
        return 0;
    
    for(int i=0; i<AES_BLOCK_SIZE; ++i)
        iv[i]=0;
    
    if(AES_set_decrypt_key((unsigned char*)key, keybits, &aes) < 0)
        return 0;

    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, msglen, &aes, iv, AES_DECRYPT);
    return 1;
}

void random_key(char* ssk, int len)
{
    int i;
    char ch;

    srand((unsigned)time(NULL));
    for (i = 0; i < len; i++)
    {
        ch = random() % 16;
        if (ch < 10)
            ch += '0';
        else
            ch = (ch - 10) + 'A';
        ssk[i] = ch;
    }
}

void print_hex(char* encrypted, int len)
{
    int i;

    for (i = 0; i < len; i++)
        printf("%X", encrypted[i]);
    printf("\n");
}