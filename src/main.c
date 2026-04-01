#include "oberon.h"
#include "modules.h"

#define MAX_THREADS 100

// --- SMART SCAN DATA ---
int top_ports[] = {21, 22, 23, 25, 53, 80, 110, 135, 139, 143, 443, 445, 993, 995, 1723, 3306, 3389, 5432, 5900, 8080, 8443};
int top_ports_count = sizeof(top_ports) / sizeof(top_ports[0]);

// Глобальные флаги для модулей
int json_mode = 0;

void print_bar(int current, int total) {
    if (json_mode) return; // В JSON режиме не мусорим в stdout барами
    float progress = (float)current / total * 100;
    printf("\r" CLR_CYAN "Scanning... [%.1f%%]" CLR_RESET, progress);
    fflush(stdout);
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
            snprintf(mod_path, sizeof(mod_path), "./modules/%s", entry->d_name);
#endif
            LIB_HANDLE hMod = LOAD_LIB(mod_path);
            if (hMod) {
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

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf(CLR_CYAN "\n[ OBERON MULTI-THREADED v4.0 RC3 ]\n" CLR_RESET);
        printf("Usage: %s <target> <start> <end> <mode> [options]\n", argv[0]);
        printf("Modes:   -t (TCP), -u (UDP + Banner)\n");
        printf("Smart:   Use '0 0' as range to scan top ports.\n");
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
    
    // Парсинг дополнительных опций
    int is_stealth = 0;
    for (int i = 5; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) is_stealth = 1;
        if (strcmp(argv[i], "-j") == 0) json_mode = 1;
    }

    int is_smart = (start == 0 && end == 0);
    int total_tasks = is_smart ? top_ports_count : (end - start + 1);

    if (!json_mode) {
        printf(CLR_CYAN "[*] Target: %s (%s) | Mode: %s %s %s\n" CLR_RESET, 
               argv[1], ip, mode, 
               is_smart ? "[SMART]" : "", 
               is_stealth ? "[STEALTH]" : "");
    } else {
        // Начало JSON объекта
        printf("{\"target\":\"%s\",\"ip\":\"%s\",\"results\":[\n", argv[1], ip);
    }

    // Если режим не стандартный, ищем плагин
    if (strcmp(mode, "-t") != 0 && strcmp(mode, "-u") != 0) {
        run_external_module(ip, is_smart ? 80 : start, mode);
        goto cleanup;
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

        if (is_stealth) {
            sleep_ms(200);
        } else if ((i + 1) % MAX_THREADS == 0) {
            sleep_ms(50);
        }

        print_bar(i + 1, total_tasks);
    }

    if (!json_mode) {
        printf("\n" CLR_GREEN "[+] Scan finished." CLR_RESET "\n");
    } else {
        // Закрываем JSON (тут логика запятых чуть сложнее, но для начала сойдет)
        printf("\n]}\n");
    }

    sleep_ms(2000); // Даем потокам доработать

cleanup:
    cleanup_networking();
    return 0;
}
