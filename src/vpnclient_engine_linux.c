#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <arpa/inet.h>
#include <openssl/evp.h>
#include "vpnclient_engine_linux.h"

// Создание TUN-устройства
int tun_create(char *dev_name) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("Cannot open /dev/net/tun");
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

    if (*dev_name) {
        strncpy(ifr.ifr_name, dev_name, IFNAMSIZ);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    printf("TUN device %s created\n", ifr.ifr_name);
    return fd;
}

// Шифрование данных (AES-256-GCM)
int encrypt_data(const unsigned char *plaintext, int plaintext_len,
                unsigned char *key, unsigned char *iv,
                unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len, ciphertext_len;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

// Основной цикл VPN-клиента
void vpnclient_engine_linux_loop(int tun_fd, int sock_fd) {
    fd_set read_fds;
    char buffer[4096];
    int max_fd = (tun_fd > sock_fd) ? tun_fd : sock_fd;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(tun_fd, &read_fds);
        FD_SET(sock_fd, &read_fds);

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select()");
            break;
        }

        if (FD_ISSET(tun_fd, &read_fds)) {
            // Чтение из TUN и отправка в сеть
            int n = read(tun_fd, buffer, sizeof(buffer));
            send(sock_fd, buffer, n, 0);
        }

        if (FD_ISSET(sock_fd, &read_fds)) {
            // Чтение из сети и запись в TUN
            int n = recv(sock_fd, buffer, sizeof(buffer), 0);
            write(tun_fd, buffer, n);
        }
    }
}
