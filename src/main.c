#include "oberon.h"
#include "modules.h"

#define MAX_THREADS 100

// Прогресс-бар для визуализации
void print_bar(int current, int total) {
    float progress = (float)current / total * 100;
    printf("\r" CLR_CYAN "Scanning... [%.1f%%]" CLR_RESET, progress);
    fflush(stdout);
}

// Поиск и запуск внешних плагинов (.so/.dll)
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
                // Ищем в плагине строку module_flag
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
    // 1. Проверка аргументов (минимум 5: exe target start end mode)
    if (argc < 5) {
        printf(CLR_CYAN "\n[ OBERON MULTI-THREADED v4.0-Patch 2 ]\n" CLR_RESET);
        printf("Usage: %s <target> <start> <end> <mode> [options]\n", argv[0]);
        printf("Modes:   -t (TCP), -u (UDP + Banner)\n");
        printf("Options: -s (Stealth mode, 200ms delay per port)\n");
        return 0;
    }

    init_networking();

    // 2. Инициализация параметров
    char *ip = resolve_host(argv[1]);
    if (!ip) { 
        printf(CLR_RED "Error: Host %s not found\n" CLR_RESET, argv[1]); 
        return 1; 
    }
    
    int start = atoi(argv[2]);
    int end = atoi(argv[3]);
    char *mode = argv[4];
    
    // Проверяем наличие 5-го аргумента для Stealth режима
    int is_stealth = (argc > 5 && strcmp(argv[5], "-s") == 0);
    int total = end - start + 1;

    printf(CLR_CYAN "[*] Target: %s (%s) | Mode: %s %s\n" CLR_RESET, 
           argv[1], ip, mode, is_stealth ? "[STEALTH ACTIVE]" : "");

    // 3. Если режим не стандартный (-t/-u), пробуем загрузить плагин
    if (strcmp(mode, "-t") != 0 && strcmp(mode, "-u") != 0) {
        printf(CLR_YELLOW "[*] Searching for external module for flag: %s\n" CLR_RESET, mode);
        run_external_module(ip, start, mode);
        goto cleanup;
    }

    // 4. Основной цикл сканирования
    for (int p = start; p <= end; p++) {
        scan_task_t *task = malloc(sizeof(scan_task_t));
        if (!task) continue;
        
        strncpy(task->ip, ip, 64);
        task->port = p;

        THREAD_HANDLE thread;
#ifdef _WIN32
        // Выбираем функцию в зависимости от режима
        LPTHREAD_START_ROUTINE func = (strcmp(mode, "-u") == 0) ? 
            (LPTHREAD_START_ROUTINE)udp_raw_mod : (LPTHREAD_START_ROUTINE)tcp_connect_mod;
            
        thread = CreateThread(NULL, 0, func, task, 0, NULL);
        if (thread) CloseHandle(thread);
#else
        void* (*func)(void*) = (strcmp(mode, "-u") == 0) ? udp_raw_mod : tcp_connect_mod;
        
        if (pthread_create(&thread, NULL, func, task) == 0) {
            pthread_detach(thread);
        }
#endif

        // 5. Обработка задержки (Stealth vs Normal)
        if (is_stealth) {
            sleep_ms(200); // 200мс между каждым портом
        } else {
            // Обычный контроль нагрузки (пауза 50мс каждые 100 потоков)
            if ((p - start + 1) % MAX_THREADS == 0) {
                sleep_ms(50);
            }
        }

        // Обновляем бар каждые 10 портов
        if (p % 10 == 0 || p == end) print_bar(p - start + 1, total);
    }

    printf("\n" CLR_GREEN "[+] Scan finished. Cleaning up..." CLR_RESET "\n");
    sleep_ms(1500); // Даем время последним потокам вывести результат

cleanup:
    cleanup_networking();
    return 0;
}
