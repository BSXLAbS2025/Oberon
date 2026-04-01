#include "oberon.h"

extern int json_mode;

THREAD_FUNC banner_grab_mod(void* arg) {
    scan_task_t *task = (scan_task_t*)arg;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) { free(task); return 0; }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(task->ip);
    server.sin_port = htons(task->port);

#ifdef _WIN32
    DWORD timeout = 2000; 
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv = {2, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
#endif

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
        char buffer[1024] = {0};
        send(sock, "HEAD / HTTP/1.0\r\n\r\n", 19, 0);
        int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);
        
        if (bytes > 0) {
            for(int i=0; i<bytes; i++) if(buffer[i] < 32) buffer[i] = ' ';
            
            if (json_mode) {
                printf("{\"port\":%d,\"type\":\"TCP\",\"banner\":\"%s\"},", task->port, buffer);
            } else {
                printf("\r" CLR_GREEN "[!] Port %-5d OPEN. Banner: " CLR_RESET "%s\n", task->port, buffer);
            }
        } else {
            if (json_mode) {
                printf("{\"port\":%d,\"type\":\"TCP\",\"status\":\"open\"},", task->port);
            } else {
                printf("\r" CLR_GREEN "[!] Port %-5d OPEN (No banner)" CLR_RESET "          \n", task->port);
            }
        }
    }

    close_socket(sock);
    free(task);
    return 0;
}
