#include "../include/vpnclient_engine_linux.h"
#include <assert.h>

void test_tun_creation() {
    char dev_name[] = "tun0";
    int tun_fd = tun_create(dev_name);
    assert(tun_fd >= 0);
    close(tun_fd);
}

int main() {
    test_tun_creation();
    printf("All tests passed!\n");
    return 0;
}
