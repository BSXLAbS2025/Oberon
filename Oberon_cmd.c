#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define sleep_ms(ms) Sleep(ms)
    #define close_socket closesocket
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <sys/time.h>
    #define sleep_ms(ms) usleep((ms) * 1000)
    #define close_socket close
    typedef int SOCKET;
    #define INVALID_SOCKET -1
#endif

#define CLR_RESET  "\033[0m"
#define CLR_RED    "\033[1;31m"
#define CLR_GREEN  "\033[1;32m"
#define CLR_CYAN   "\033[1;36m"
#define CLR_YELLOW "\033[1;33m"

char* resolve_host(char *hostname) {
    struct hostent *he = gethostbyname(hostname);
    if (!he) return NULL;
    return inet_ntoa(*(struct in_addr *)he->h_addr);
}

void tcp_scan(char *ip, int port, int delay) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) return;

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    
#ifdef _WIN32
    DWORD timeout = 500;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv = {0, 500000};
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
#endif

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
        printf("\r" CLR_GREEN "[!] Port %-5d (TCP) is OPEN" CLR_RESET "          \n", port);
    }
    close_socket(sock);
    if (delay > 0) sleep_ms(delay / 1000);
}

void udp_scan(char *ip, int port, int delay) {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) return;

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
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    sendto(sock, "", 0, 0, (struct sockaddr *)&server, sizeof(server));
    char b;
    if (recvfrom(sock, &b, 1, 0, NULL, NULL) >= 0) {
        printf("\r" CLR_YELLOW "[!] Port %-5d (UDP) is OPEN/FILTERED" CLR_RESET " \n", port);
    }
    close_socket(sock);
    if (delay > 0) sleep_ms(delay / 1000);
}

void print_bar(int current, int total) {
    float progress = (float)current / total * 100;
    int bar_len = (int)(progress / 5);
    printf("\r" CLR_CYAN "Scanning: [" CLR_RESET);
    for(int i=0; i<20; i++) printf(i < bar_len ? "#" : " ");
    printf(CLR_CYAN "] %.1f%%" CLR_RESET, progress);
    fflush(stdout);
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    WSADATA wsa; 
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;
#endif

    if (argc < 5) {
        printf(CLR_CYAN "\n[ OBERON NETWORK SCANNER v3.3 ]\n" CLR_RESET);
        printf("Usage: %s <target> <start> <end> <mode> [options]\n", argv[0]);
        printf("Modes: -t (TCP), -u (UDP)\nOptions: -s (Stealth, 200ms delay)\n");
        return 0;
    }

    char *target = argv[1];
    int start = atoi(argv[2]);
    int end = atoi(argv[3]);
    int choice = 0, delay = 0;

    for (int i = 4; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0) choice = 1;
        else if (strcmp(argv[i], "-u") == 0) choice = 2;
        else if (strcmp(argv[i], "-s") == 0) delay = 200000;
    }

    char *ip = resolve_host(target);
    if (!ip) { printf(CLR_RED "Error: Host %s not found\n" CLR_RESET, target); return 1; }

    printf(CLR_CYAN "[*] Target: %s (%s)\n" CLR_RESET, target, ip);
    int total = (end >= start) ? (end - start + 1) : 1;

    time_t start_time = time(NULL); // Засекаем время

    for (int p = start; p <= end; p++) {
        if (choice == 1) tcp_scan(ip, p, delay);
        else if (choice == 2) udp_scan(ip, p, delay);
        print_bar(p - start + 1, total);
    }

    time_t end_time = time(NULL);
    double duration = difftime(end_time, start_time);

    printf("\n" CLR_GREEN "[+] Scan Completed in %.1f seconds." CLR_RESET "\n", duration);

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
