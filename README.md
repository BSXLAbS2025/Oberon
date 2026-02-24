# Oberon
A lightweight C-based network scanner with DNS resolution and banner grabbing.

# 🛰️ Oberon Network Scanner v4.0

**Oberon** is a lightweight, high-performance network reconnaissance tool written in pure **C**. Designed for ethical hackers and security enthusiasts, it provides essential features for network discovery and service identification.

Developed as a personal project to understand low-level socket programming and network protocols.

## ✨ Features
- **TCP Connect Scan**: Reliable identification of open TCP ports.
- **UDP Scanning**: Detection of open/filtered UDP services.
- **DNS Resolution**: Scan targets using both IP addresses and Domain Names (e.g., `google.com`).
- **Banner Grabbing**: Attempts to identify service versions (HTTP, SSH, etc.) upon connection.
- **Visual Progress Bar**: Real-time feedback during long scans.
- **Fast & Lightweight**: Zero dependencies, compiled to a tiny binary.

## 🚀 Getting Started

### Prerequisites
You need a C compiler (like `gcc`) installed on your system (Linux, macOS, or Android/NetHunter).

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com
   cd Oberon
2. Run:
   ```bash
   gcc oberon.c -o oberon
### Use:
**WINDOWS**:
   1. Download Archive from "Releases" page
   2. Unzip to a folder
   3. Run: oberon <target> <start> <end> <mode> [options]

**Linux**
   1. Download or compile Oberon
   2. Open terminal
   3. Give chmod rights (+x) 
   4. Send Oberon to $PATH (for using oberon, instead of ./oberon)
   5. Use 

**MacOS**
   1. Download Archive
   2. Unzip to a folder
   3. Give chmod rights (+x)
   4. Use (./)

### Options, ports, modes and targets
   - Targets (IP Adresses, hosts (e.g. google.com)
   - Start/End (Ports, e.g. 80 80, or 80 100)
   - Mode (Scan modes (-t (TCP), -u (UDP)) 
   - Options (-s (Stealth, 200ms per every check) 

### WARN! This tool is for educational and ethical testing purposes only. Scanning targets without prior authorization is illegal. The developer is not responsible for any misuse of this software.

### Used:
1. - C (Language)
2. - Posix SOCKETS

### You can freely use, distribute, modify the tool.

There will be updates of utility soon
