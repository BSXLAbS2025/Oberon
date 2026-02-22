#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

// DNS Resolver
char* resolve_host(char *hostname) {
    struct hostent *he = gethostbyname(hostname);
    if (he == NULL) return NULL;
    return inet_ntoa(*(struct in_addr *)he->h_addr);
}

// TCP Scan Logic
int tcp_scan(char *ip, int port, int delay) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    struct timeval tv = {0, 500000}; 

    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    int res = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (res == 0) {
        printf("\r[!] Port %d (TCP) is OPEN!                 \n", port);
    }
    close(sock);
    if (delay > 0) usleep(delay);
    return (res == 0);
}

// UDP Scan Logic
int udp_scan(char *ip, int port, int delay) {
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in server;
    struct timeval tv = {1, 0}; // UDP needs more time for timeout

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    sendto(sock, "", 0, 0, (struct sockaddr *)&server, sizeof(server));
    char b;
    int res = recvfrom(sock, &b, 1, 0, NULL, NULL);
    
    if (res >= 0) {
        printf("\r[!] Port %d (UDP) is OPEN!                 \n", port);
    }
    
    close(sock);
    if (delay > 0) usleep(delay);
    return (res >= 0);
}

void print_bar(int current, int total) {
    float progress = (float)current / total * 100;
    int bar_len = (int)(progress / 5);
    printf("\rScanning: [");
    for(int i=0; i<20; i++) printf(i < bar_len ? "#" : " ");
    printf("] %.1f%%", progress);
    fflush(stdout);
}

int main() {
    char target[256];
    int choice, mode, start, end, delay = 0;

    while(1) {
        printf("\n\033[1;36m[ OBERON NETWORK SCANNER v3.2 ]\033[0m\n");
        printf("1. TCP Connect Scan\n2. UDP Scan\n0. Exit\nChoice: ");
        if (scanf("%d", &choice) != 1 || choice == 0) break;

        printf("Target (IP or Domain): ");
        scanf("%255s", target);
        
        char *ip = resolve_host(target);
        if (!ip) { printf("Error: Host not found\n"); continue; }
        printf("[*] Target IP: %s\n", ip);

        printf("\nSelect Speed Mode:\n1. Fast Scan (No delay)\n2. Stealth Mode (200ms delay)\nChoice: ");
        scanf("%d", &mode);
        delay = (mode == 2) ? 200000 : 0;

        printf("Start Port: ");
        scanf("%d", &start);
        printf("End Port: ");
        scanf("%d", &end);

        int total = end - start + 1;
        printf("\n[*] Starting reconnaissance on %s...\n", target);

        for (int p = start; p <= end; p++) {
            if (choice == 1) tcp_scan(ip, p, delay);
            else if (choice == 2) udp_scan(ip, p, delay);
            
            print_bar(p - start + 1, total);
        }
        printf("\n\033[1;32m[+] Scan Completed.\033[0m\n");
    }
    return 0;
}
