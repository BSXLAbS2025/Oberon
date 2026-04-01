#include "oberon.h"

extern int json_mode; // Берем флаг из main.c
static int first_json_entry = 1;

void* tcp_connect_mod(void* arg) {
    scan_task_t* task = (scan_task_t*)arg;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock == INVALID_SOCKET) {
        free(task);
        return NULL;
    }

    // Таймауты
#ifdef _WIN32
    DWORD timeout = 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
#endif

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(task->port);
    addr.sin_addr.s_addr = inet_addr(task->ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        // Определение ОС (TTL)
        int ttl = 0;
        socklen_t optlen = sizeof(ttl);
        char* os = "Unknown";
        if (getsockopt(sock, IPPROTO_IP, IP_TTL, (char *)&ttl, &optlen) == 0) {
            if (ttl <= 64) os = "Linux";
            else if (ttl <= 128) os = "Windows";
        }

        // Баннер
        char banner[128] = "none";
        send(sock, "\r\n", 2, 0); 
        int b = recv(sock, banner, sizeof(banner) - 1, 0);
        if (b > 0) {
            for(int i = 0; i < b; i++) if(banner[i] < 32) banner[i] = ' ';
            banner[b] = '\0';
        }

        if (json_mode) {
            // Чтобы JSON был валидным, объекты разделяются запятыми
            // В многопоточности это может быть "грязновато", но для CLI пойдет
            printf("%s{\"port\":%d,\"ttl\":%d,\"os\":\"%s\",\"banner\":\"%s\"}", 
                   first_json_entry ? "" : ",", task->port, ttl, os, banner);
            first_json_entry = 0;
        } else {
            printf(CLR_GREEN "[+] %-5d OPEN" CLR_RESET " | TTL: %d (%s) | Banner: %s\n", 
                   task->port, ttl, os, banner);
        }
    }

    close_socket(sock);
    free(task);
    return NULL;
}
