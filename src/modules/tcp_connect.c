#include "modules.h"

THREAD_FUNC tcp_connect_mod(void* arg) {
    scan_task_t *task = (scan_task_t*)arg;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) goto end;

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(task->ip);
    server.sin_port = htons(task->port);

    // Ставим короткий таймаут для скорости
#ifdef _WIN32
    DWORD timeout = 1000;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv = {1, 0};
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
#endif

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
        printf("\r" CLR_GREEN "[!] Port %-5d (TCP) OPEN" CLR_RESET "          \n", task->port);
    }

    close_socket(sock);
end:
    free(task); // Освобождаем память задачи
    return 0;
}
