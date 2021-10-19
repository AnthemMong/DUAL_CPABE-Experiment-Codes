# ifndef LAES_H
# define LAES_H

# define AES_128 128
# define AES_192 192
# define AES_256 256

int aes_encrypt(char* in, char* key, char* out, int msglen, int keybits);

int aes_decrypt(char* in, char* key, char* out, int msglen, int keybits);

void random_key(char* ssk, int len);

void print_hex(char* encrypted, int len);

# endif