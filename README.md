# My_sniffer

A cross-platform network packet sniffer written in C++20. It captures raw frames from a network interface and parses them by hand (Ethernet, IPv4, TCP, UDP), with no networking library doing the parsing.

> **Status: `1.x` (work in progress).** Capture and Ethernet parsing work. Filtering, analysis and logging are being added. See [Roadmap](#roadmap).

## Features

| Feature | Status |
|---|---|
| Raw capture on Linux (`AF_PACKET`) | Done |
| Raw capture on Windows (Npcap, capture/read only) | Done |
| Ethernet, IPv4, TCP, UDP parsing | Done |
| Terminal filter / search | Testing |
| Traffic analysis (spikes, beaconing, odd ports...) | In Progress |
| Logging / storage | Planned |
| ARP, ICMP, IPv6, DNS, HTTP | Bonus, planned |

## Requirements

| OS | Compiler | Dependencies |
|---|---|---|
| Linux | GCC 13+ or Clang 17+ (C++20 with `std::format`) | CMake >= 3.20 |
| Windows | MSVC 2022 (x64) | CMake >= 3.20, [Npcap](https://npcap.com/#download) runtime, Npcap SDK (by default inside npcap-sdk folder) |

### Windows setup

1. Install the **Npcap** runtime from <https://npcap.com/#download>. Tick *"Install Npcap in WinPcap API-compatible Mode"*.
2. (Optional) Download the **Npcap SDK** and extract it into `./npcap-sdk` (it must contain `Include/` and `Lib/x64/`). You can also pass `-DNPCAP_SDK_DIR=<path>` to CMake.

## Build

### Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Windows (x64)

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

The executable is `build/My_sniffer` (Linux) or `build/Release/My_sniffer.exe` (Windows).

## Install

### Linux

```bash
sudo cmake --install build                       # to /usr/local
cmake --install build --prefix ~/.local          # or without root
```

Or build a package:

```bash
cpack --config build/CPackConfig.cmake           # .tar.gz and .deb
sudo dpkg -i my-sniffer-*.deb
```

### Windows

```powershell
cmake --install build --config Release --prefix "C:\Program Files\My_sniffer"
cpack --config build/CPackConfig.cmake -C Release   # .zip
```

## Privileges

Capturing raw traffic needs elevated privileges.

- **Linux:** run with `sudo`, or grant only the needed capabilities to the installed binary (then no `sudo` is needed):
  ```bash
  sudo setcap cap_net_raw,cap_net_admin=eip /usr/local/bin/My_sniffer
  ```
- **Windows:** run the terminal **as Administrator**. Npcap must be installed.

## Usage

```bash
sudo ./My_sniffer
```

1. The program lists the available network interfaces.
2. Type the interface name (Linux: `eth0`, `wlo1`, or `all`; Windows: the full device name such as `\Device\NPF_{...}`) and press Enter.
3. Press a key.

| Key | Action |
|---|---|
| `q` | Quit |
| `h` | Print the help page |
| `v` | Print version number |
| `s` | Display the saved history |
| `a` | Show alerts |
| `f` | Enter filter mode |

### Filter syntax

| Keyword | Description |
|---|---|
| `proto` | Filter by osi protocol <ipv4/tcp/udp> |
| `port` | Match source OR destination port |
| `ip` | Match source OR destination IPV4 |
| `mac` | Match source OR destination MAC |
| `in/out` | Tell the filter to look for only source OR destiantion |


```
proto tcp
port 443
port 53 in
ip 192.168.1.10 out
mac aa:bb:cc:dd:ee:ff in
```

## Project layout

```
include/                  headers (protocols, NIC, helpers)
src/main.cpp              capture loop and terminal interface
src/nic/                  platform capture modules (nic_linux.cpp, nic_windows.cpp, adapters)
src/nic_factory.cpp       picks the NIC implementation for the current OS
src/protocol_parsing/     Ethernet / IPv4 / TCP / UDP parsers
src/keyboard.cpp          non-blocking keyboard input (termios / conio)
src/fcn_helper.cpp        printing and conversion helpers
src/filer/                files for the filter function
src/analysis              alerts and detectors
```

## Design choices

- **NIC abstraction:** a `NIC` interface (`open`, `read`, `close`) with one implementation per OS, created by `NIC_Factory`. The rest of the program never touches platform code.
- **Linux:** a raw `AF_PACKET` socket gives complete Ethernet frames with no library.
- **Windows:** Windows sockets do not expose Ethernet frames, so Npcap is used **only to open the interface and read raw frames**. All parsing is done by this project.

## Roadmap

- [x] Terminal filter / search
- [ ] Analysis engine (traffic spikes, unauthorized foreign IPs, protocol/port mismatches, beaconing)
- [ ] Logging to file
- [ ] Bonus: ARP, ICMP, IPv6, DNS, HTTP, sniffer detection, TUI

## Versioning

[Semantic Versioning](https://semver.org). Releases are Git tags (`v0.0.1`, ...). See [CHANGELOG.md](CHANGELOG.md). A command cheat sheet for maintainers is in [Memo.md](Memo.md).

## Troubleshooting

| Problem | Fix |
|---|---|
| `socket: Operation not permitted` | Run with `sudo` or use `setcap` (see Privileges). |
| `Interface not found` | Check the name in the interface list printed at startup. |
| Windows: `wpcap.dll` / `Packet.dll` not found | Install Npcap in WinPcap-compatible mode. |
| Windows: `pcap_open_live failed` | Run as Administrator; use the exact `\Device\NPF_{...}` name. |
| CMake: `Npcap SDK not found` | Extract the SDK to `./npcap-sdk` or pass `-DNPCAP_SDK_DIR`. |
| `std::format` not found | Use GCC 13+, Clang 17+ or MSVC 2022. |

## License

See `LICENSE`.