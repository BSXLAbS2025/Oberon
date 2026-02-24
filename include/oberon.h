#ifndef OBERON_H
#define OBERON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    #define sleep_ms(ms) Sleep(ms)
    #define close_socket closesocket
    typedef int socklen_t;
    // Макрос для запуска потока на Windows
    #define THREAD_FUNC DWORD WINAPI
    #define THREAD_HANDLE HANDLE
    #define LIB_HANDLE HMODULE
    #define LOAD_LIB(path) LoadLibraryA(path)
    #define GET_FUNC GetProcAddress
    #define CLOSE_LIB FreeLibrary
    #define LIB_EXT ".dll"
#else
    #include <dlfcn.h>
    #include <dirent.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <pthread.h>
    #define sleep_ms(ms) usleep((ms) * 1000)
    #define close_socket close
    typedef int SOCKET;
    #define INVALID_SOCKET -1
    // Макрос для запуска потока на Linux/Mac
    #define THREAD_FUNC void*
    #define THREAD_HANDLE pthread_t
    #define LIB_HANDLE void*
    #define LOAD_LIB(path) dlopen(path, RTLD_LAZY)
    #define GET_FUNC dlsym
    #define CLOSE_LIB dlclose
    #define LIB_EXT ".so"
#endif

// Цвета
#define CLR_RESET  "\033[0m"
#define CLR_GREEN  "\033[1;32m"
#define CLR_CYAN   "\033[1;36m"
#define CLR_RED    "\033[1;31m"
#define CLR_YELLOW "\033[1;33m"

// Структура задачи для потока
typedef struct {
    char ip[64];
    int port;
    int delay;
} scan_task_t;

#endif
