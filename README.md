# Oberon
A lightweight C-based network scanner with DNS resolution and banner grabbing.

# 🛰️ Oberon Network Scanner v3.0

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
   cd OberonScanner
2. Run:
   ```bash
   gcc oberon.c -o oberon
3. Use:
   ```bash
   ./oberon

### WARN! This tool is for educational and ethical testing purposes only. Scanning targets without prior authorization is illegal. The developer is not responsible for any misuse of this software.

### Additional info


### Used:
### - C (Language)
### - Posix SOCKETS

### You can freely use, distribute, modify the tool.
