#include "header.hpp"
#include "fcn_helper.hpp"
#include "nic.hpp"
#include <fcntl.h>
#include <iomanip>
#include <memory>

#ifdef _WIN32

    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <conio.h>

#else
    #include <fcntl.h>
    #include <poll.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
#endif

void printHex(const unsigned char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

#ifdef  _WIN32
    bool keyboard_available() {
        return _kbhit() != 0;
    }

    bool read_keyboard(char& c) {
        if (!_kbhit())
            return false;

        c = static_cast<char>(_getch());
        return true;
    }
#else
    bool keyboard_available()
    {
        struct pollfd fd{};

        fd.fd = STDIN_FILENO;
        fd.events = POLLIN;

        int ret = poll(&fd, 1, 0);

        return ret > 0 && (fd.revents & POLLIN);
    }

    bool read_keyboard(char& c)
    {
        return read(STDIN_FILENO, &c, 1) > 0;
    }
#endif


int main(void)
{
    uint8_t buffer [65536]; //buffer only for reading raw packet
    std::unique_ptr<NIC> nic;

    #ifdef _WIN32
        nic = std::make_unique<NIC_Windows>();
    #else
        nic = std::make_unique<NIC_Linux>();
    #endif

    #ifndef _WIN32
        int stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);

        if (stdin_flags < 0) {
            perror("fcntl");
            return 1;
        }

        if (fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK) < 0) {
            perror("fcntl");
            return 1;
        }

    #endif

    if(!nic->open("\\Device\\NPF_{E73F39E4-CD1E-4C70-9266-05DBC49115DD}")) {
        std::cerr << "Failed to open NIC\n";
        return 1;
    }

    std::cout << "Press q to exit: " << std::endl;
    while (true) {
        if (keyboard_available()) {
            char c;
            if (read_keyboard(c)) {
                if (c == 'q' || c == 'Q') {
                    break;
                }
            }
        }
#ifdef _WIN32
        std::ptrdiff_t bytes = nic->read(buffer, sizeof(buffer));
        if (bytes > 0) {
#else
            struct pollfd fd{};
            fd.fd = nic->getfd();
            fd.events = POLLIN;
            int ret = poll(&fd, 1, 10);

            if (ret < 0) {
                perror("poll");
                break;
            }
            if (ret == 0) {
                continue;
            }
            if (!(fd.revents & POLLIN)) {
                continue;
            }
            std::ptrdiff_t bytes = nic->read(buffer, sizeof(buffer));

            if (bytes > 0) {
    #endif
                struct eth_header *hdr = reinterpret_cast<struct eth_header *>(buffer);
                switch (ntohs(hdr->ethertype)) {
                    case ETH_T_IPV4:
                        std::cout << "IPV4\n";
                        std::cout << *hdr << std::endl;
                        break;
                    case ETH_T_IPV6:
                        std::cout << "IPV6\n";
                        std::cout << *hdr << std::endl;
                        break;
                    case ETH_T_ARP:
                        std::cout << "ARP\n";
                        std::cout << *hdr << std::endl;
                        break;
                    default:
                        std::cout << *hdr << std::endl;
                        std::cout << "Unknown 0x" << std::hex << std::setw(4) << std::setfill('0') << ntohs(hdr->ethertype) << std::endl;
                        break;
                }
            }
        }
    return 0;
}