#ifndef MODULES_H
#define MODULES_H

#include "oberon.h"

// Прототипы функций модулей
THREAD_FUNC tcp_connect_mod(void* arg);
THREAD_FUNC udp_raw_mod(void* arg);
THREAD_FUNC banner_grab_mod(void* arg);

#endif
