#include "oberon.h"

// Глобальный счетчик для детектирования Honeypot (ловушек)
// Если открытых портов слишком много, это подозрительно
static int open_ports_count = 0;

void* tcp_connect_mod(void* arg) {
    scan_task_t* task = (scan_task_t*)arg;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        free(task);
        return NULL;
    }

    // 1. Настройка таймаута соединения (чтобы не ждать по 20 секунд)
#ifdef _WIN32
    DWORD timeout = 1000; // 1 сек
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
    addr.sin_family = AF_INET;
    addr.sin_port = htons(task->port);
    addr.sin_addr.s_s_addr = inet_addr(task->ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        open_ports_count++;

        // --- ФИЧА: OS FINGERPRINTING (TTL) ---
        int ttl = 0;
        socklen_t optlen = sizeof(ttl);
        char* os_guess = "Unknown";
        if (getsockopt(sock, IPPROTO_IP, IP_TTL, (char *)&ttl, &optlen) == 0) {
            if (ttl <= 64) os_guess = "Linux/Unix";
            else if (ttl <= 128) os_guess = "Windows";
            else if (ttl <= 255) os_guess = "Network Eq.";
        }

        // --- ФИЧА: BANNER GRABBING ---
        char buffer[256] = {0};
        // Посылаем пустой запрос, чтобы спровоцировать ответ (например, от HTTP)
        send(sock, "\r\n", 2, 0); 
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes > 0) {
            // Очистка баннера от спецсимволов для красивого вывода
            for(int i = 0; i < bytes; i++) {
                if(buffer[i] < 32 || buffer[i] > 126) buffer[i] = ' ';
            }
            printf(CLR_GREEN "[+] %-5d OPEN" CLR_RESET " | TTL: %-3d (%s) | Banner: %s\n", 
                   task->port, ttl, os_guess, buffer);
        } else {
            printf(CLR_GREEN "[+] %-5d OPEN" CLR_RESET " | TTL: %-3d (%s)\n", 
                   task->port, ttl, os_guess);
        }

        // --- ФИЧА: HONEYPOT CHECK ---
        if (open_ports_count > 50) {
            printf(CLR_RED "[!] WARNING: More than 50 ports open. Possible Honeypot detected on %s\n" CLR_RESET, task->ip);
            // Сбрасываем счетчик, чтобы не спамить
            open_ports_count = -9999; 
        }
    }

    CLOSE_SOCKET(sock);
    free(task);
    return NULL;
}
