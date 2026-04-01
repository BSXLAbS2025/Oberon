#include "oberon.h"
#include "modules.h"

#define MAX_THREADS 100

int top_ports[] = {21, 22, 23, 25, 53, 80, 110, 135, 139, 143, 443, 445, 993, 995, 1723, 3306, 3389, 5432, 5900, 8080, 8443};
int top_ports_count = sizeof(top_ports) / sizeof(top_ports[0]);

// Глобальный флаг
int json_mode = 0;

void print_bar(int current, int total) {
    if (json_mode) return;
    float progress = (float)current / total * 100;
    printf("\r" CLR_CYAN "Scanning... [%.1f%%] \n" CLR_RESET, progress);
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf(CLR_CYAN "\n[ OBERON MULTI-THREADED v4.0 RC3 ]\n" CLR_RESET);
        printf("Usage: %s <target> <start> <end> <mode> [options]\n", argv[0]);
        printf("Mode: -t (TCP), -u (UDP)\n");
        printf("Options: -s (Stealth), -j (JSON output)\n");
        return 0;
    }

    init_networking();

    char *ip = resolve_host(argv[1]);
    if (!ip) { 
        if (!json_mode) printf(CLR_RED "Error: Host %s not found\n" CLR_RESET, argv[1]); 
        return 1; 
    }
    
    int start = atoi(argv[2]);
    int end = atoi(argv[3]);
    char *mode = argv[4];
    
    for (int i = 5; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) sleep_ms(200); // Упрощенный stealth
        if (strcmp(argv[i], "-j") == 0) json_mode = 1;
    }

    int is_smart = (start == 0 && end == 0);
    int total_tasks = is_smart ? top_ports_count : (end - start + 1);

    if (json_mode) {
        printf("{\"target\":\"%s\",\"ip\":\"%s\",\"results\":[", argv[1], ip);
    } else {
        printf(CLR_CYAN "[*] Target: %s (%s) | Mode: %s %s\n" CLR_RESET, 
               argv[1], ip, mode, is_smart ? "[SMART]" : "");
    }

    for (int i = 0; i < total_tasks; i++) {
        int current_port = is_smart ? top_ports[i] : (start + i);
        
        scan_task_t *task = malloc(sizeof(scan_task_t));
        if (!task) continue;
        
        strncpy(task->ip, ip, 64);
        task->port = current_port;

        THREAD_HANDLE thread;
#ifdef _WIN32
        LPTHREAD_START_ROUTINE func = (strcmp(mode, "-u") == 0) ? 
            (LPTHREAD_START_ROUTINE)udp_raw_mod : (LPTHREAD_START_ROUTINE)tcp_connect_mod;
        thread = CreateThread(NULL, 0, func, task, 0, NULL);
        if (thread) CloseHandle(thread);
#else
        void* (*func)(void*) = (strcmp(mode, "-u") == 0) ? udp_raw_mod : tcp_connect_mod;
        if (pthread_create(&thread, NULL, func, task) == 0) pthread_detach(thread);
#endif

        if ((i + 1) % MAX_THREADS == 0) sleep_ms(50);
        print_bar(i + 1, total_tasks);
    }

    // Ожидание завершения потоков
    sleep_ms(2500);

    if (json_mode) {
        printf("]}\n"); // Закрываем JSON корректно
    } else {
        printf("\n" CLR_GREEN "[+] Scan finished." CLR_RESET "\n");
    }

    cleanup_networking();
    return 0;
}
