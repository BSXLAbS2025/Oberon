#include "oberon.h"

// Глобальный счетчик для детектирования Honeypot
static int open_ports_count = 0;

void* tcp_connect_mod(void* arg) {
    scan_task_t* task = (scan_task_t*)arg;
    
    // Используем SOCKET из твоего хедера
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        free(task);
        return NULL;
    }

    // Настройка таймаута
#ifdef _WIN32
    DWORD timeout = 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv));
#endif

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(task->port);
    // ИСПРАВЛЕНО: s_addr вместо s_s_addr
    addr.sin_addr.s_addr = inet_addr(task->ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        open_ports_count++;

        // --- OS FINGERPRINTING (TTL) ---
        int ttl = 0;
        socklen_t optlen = sizeof(ttl);
        char* os_guess = "Unknown";
        if (getsockopt(sock, IPPROTO_IP, IP_TTL, (char *)&ttl, &optlen) == 0) {
            if (ttl <= 64) os_guess = "Linux/Unix";
            else if (ttl <= 128) os_guess = "Windows";
            else if (ttl <= 255) os_guess = "Network Eq.";
        }

        // --- BANNER GRABBING ---
        char buffer[256] = {0};
        send(sock, "\r\n", 2, 0); 
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes > 0) {
            for(int i = 0; i < bytes; i++) {
                if(buffer[i] < 32 || buffer[i] > 126) buffer[i] = ' ';
            }
            printf(CLR_GREEN "[+] %-5d OPEN" CLR_RESET " | TTL: %-3d (%s) | Banner: %s\n", 
                   task->port, ttl, os_guess, buffer);
        } else {
            printf(CLR_GREEN "[+] %-5d OPEN" CLR_RESET " | TTL: %-3d (%s)\n", 
                   task->port, ttl, os_guess);
        }

        if (open_ports_count > 50) {
            printf(CLR_RED "[!] WARNING: Possible Honeypot on %s\n" CLR_RESET, task->ip);
            open_ports_count = -9999; 
        }
    }

    // ИСПРАВЛЕНО: используем close_socket из твоего хедера
    close_socket(sock);
    free(task);
    return NULL;
}
