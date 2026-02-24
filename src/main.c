#include "oberon.h"
#include "modules.h"

#define MAX_THREADS 100

void print_bar(int current, int total) {
    float progress = (float)current / total * 100;
    printf("\r" CLR_CYAN "Scanning... [%.1f%%]" CLR_RESET, progress);
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    init_networking();

    if (argc < 5) {
        printf(CLR_CYAN "\n[ OBERON MULTI-THREADED v4.0-Patch 2 ]\n" CLR_RESET);
        printf("Usage: %s <target> <start> <end> <mode> (-t, -u, -b)\n", argv[0]);
        return 0;
    }

    char *ip = resolve_host(argv[1]);
    if (!ip) { printf(CLR_RED "Host not found\n" CLR_RESET); return 1; }
    
    int start = atoi(argv[2]);
    int end = atoi(argv[3]);
    char *mode = argv[4];
    int total = end - start + 1;

    printf(CLR_CYAN "[*] Target: %s (%s) | Threads: %d\n" CLR_RESET, argv[1], ip, MAX_THREADS);
    
    time_t start_time = time(NULL);

    for (int p = start; p <= end; p++) {
        scan_task_t *task = malloc(sizeof(scan_task_t));
        strcpy(task->ip, ip);
        task->port = p;

        THREAD_HANDLE thread;
#ifdef _WIN32
        thread = CreateThread(NULL, 0, 
            (strcmp(mode, "-t") == 0) ? (LPTHREAD_START_ROUTINE)tcp_connect_mod :
            (strcmp(mode, "-u") == 0) ? (LPTHREAD_START_ROUTINE)udp_raw_mod : (LPTHREAD_START_ROUTINE)banner_grab_mod, 
            task, 0, NULL);
        CloseHandle(thread); // Не ждем завершения
#else
        pthread_create(&thread, NULL, 
            (strcmp(mode, "-t") == 0) ? tcp_connect_mod :
            (strcmp(mode, "-u") == 0) ? udp_raw_mod : banner_grab_mod, 
            task);
        pthread_detach(thread);
#endif

        if ((p - start) % MAX_THREADS == 0) sleep_ms(50); // Пауза для контроля нагрузки
        if (p % 10 == 0) print_bar(p - start + 1, total);
    }

    printf("\n" CLR_GREEN "[+] Scan finished. Waiting for last threads..." CLR_RESET "\n");
    sleep_ms(2000); // Даем последним потокам доработать
    cleanup_networking();
    return 0;
}

void run_external_module(char *target_ip, int port, char *requested_flag) {
    char mod_path[512];
    
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA("modules/*.dll", &findData);
    if (hFind == INVALID_HANDLE_VALUE) return;
    do {
        snprintf(mod_path, sizeof(mod_path), "modules/%s", findData.cFileName);
#else
    DIR *dir = opendir("modules");
    struct dirent *entry;
    if (!dir) return;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, LIB_EXT)) {
            snprintf(mod_path, sizeof(mod_path), "modules/%s", entry->d_name);
#endif
            
            LIB_HANDLE hMod = LOAD_LIB(mod_path);
            if (hMod) {
                // Ищем в модуле строку с его флагом
                char **mod_flag = (char**)GET_FUNC(hMod, "module_flag");
                
                if (mod_flag && strcmp(*mod_flag, requested_flag) == 0) {
                    module_run_func run = (module_run_func)GET_FUNC(hMod, "run_module");
                    if (run) run(target_ip, port);
                }
                CLOSE_LIB(hMod);
            }
#ifdef _WIN32
    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);
#else
        }
    }
    closedir(dir);
#endif
}
