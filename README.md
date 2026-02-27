### Oberon Network Scanner v4.0

Oberon is a lightweight, high-performance network reconnaissance tool written in C99. Designed for ethical hackers and security enthusiasts, it provides essential features for discovery and service identification across Linux, Windows, macOS, and Android (Termux).

### ✨ Features
Multi-threaded Engine: High-speed scanning with a managed thread pool.

TCP Connect Scan: Reliable identification of open TCP ports.

UDP Scanning: Detection of open/filtered UDP services + Banner grabbing.

Stealth Mode: Optional -s flag to add a 200ms delay between requests to bypass simple IDS.

DNS Resolution: Scan targets using IP addresses or hostnames (e.g., google.com).

Dynamic Modules: Extend functionality without recompiling the core.

### 🚀 Installation & Build
Prerequisites
CMake (v3.10+)

C Compiler (GCC, Clang, or MSVC)

Building from source
Bash
`
git clone https://github.com/BSXLAbS2025/Oberon
cd Oberon
cmake -B build
cmake --build build --config Release
The executable will be located in the build/ directory. On Windows, check build/Release/.
`
### 1. Creating Custom Modules (Plugins)
Oberon v4.0 supports dynamic loading. Just drop a .so (Linux/macOS) or .dll (Windows) into the ./modules/ folder.

2. Compilation Commands
Linux / Termux / macOS:
Bash:
`gcc -shared -fPIC my_plugin.c -I./include -o modules/my_plugin.so`

Windows (MinGW):
Bash:
`gcc -shared my_plugin.c -I./include -o modules/my_plugin.dll -lws2_32`

### 📖 Usage

Bash:
`oberon <target> <start_port> <end_port> <mode> [options]`

`Argument|Description
<target>"|IP Address or Hostname (e.g., 192.168.1.1 or google.com)"
<mode>   |"-t (TCP), -u (UDP), or custom module flag (e.g., -info)"
[options]|-s (Stealth Mode: 200ms delay per port)`

### ⚠️ Disclaimer
This tool is for educational and ethical testing purposes only. Scanning targets without prior authorization is illegal. The developer is not responsible for any misuse of this software.

### Project Status: Alive & Active 🚀
