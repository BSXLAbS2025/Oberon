#include "modules.h"

THREAD_FUNC udp_raw_mod(void* arg) {
    scan_task_t *task = (scan_task_t*)arg;
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) { free(task); return 0; }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    
#ifdef _WIN32
    DWORD timeout = 800;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv = {0, 800000};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif

    server.sin_family = AF_INET;
    server.sin_port = htons(task->port);
    server.sin_addr.s_addr = inet_addr(task->ip);

    sendto(sock, "", 0, 0, (struct sockaddr *)&server, sizeof(server));
    char b;
    if (recvfrom(sock, &b, 1, 0, NULL, NULL) >= 0) {
        printf("\r" CLR_YELLOW "[!] Port %-5d (UDP) OPEN/FILTERED" CLR_RESET " \n", task->port);
    }
    
    close_socket(sock);
    free(task);
    return 0;
}
