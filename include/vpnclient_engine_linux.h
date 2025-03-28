#ifndef VPNCLIENT_ENGINE_LINUX_H
#define VPNCLIENT_ENGINE_LINUX_H

#include <stdint.h>

int tun_create(char *dev_name);
int encrypt_data(const unsigned char *plaintext, int plaintext_len,
                unsigned char *key, unsigned char *iv,
                unsigned char *ciphertext);
void vpnclient_engine_linux_loop(int tun_fd, int sock_fd);

#endif
