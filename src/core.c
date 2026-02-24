#include "oberon.h"

void init_networking() {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif
}

void cleanup_networking() {
#ifdef _WIN32
    WSACleanup();
#endif
}

char* resolve_host(char *hostname) {
    struct hostent *he = gethostbyname(hostname);
    if (!he) return NULL;
    return inet_ntoa(*(struct in_addr *)he->h_addr);
}
