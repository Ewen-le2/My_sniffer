#include "header.hpp"
#include "fcn_helper.hpp"
#include "nic.hpp"
#include <fcntl.h>
#include <iomanip>
#include <memory>
#include <netinet/in.h>
#include <sys/poll.h>

void printHex(const unsigned char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

int main(void)
{
    uint8_t buffer [65536]; //buffer only for reading raw packet
    std::unique_ptr<NIC> nic;

    #ifdef _WIN32
        nic = std::make_unique<NIC_Windows>();
    #else
        nic = std::make_unique<NIC_Linux>();
    #endif

    // Make stdin non-blocking
    int stdinflags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, stdinflags | O_NONBLOCK);
    char c;

    if(!nic->open("all")) {
        std::cerr << "Failed to open NIC\n";
        return 1;
    }

    struct pollfd fds[2];
    fds[0].fd = nic->getfd();
    fds[0].events = POLLIN;

    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;

    std::cout << "Press q to exit: " << std::endl;
    while (true) {
        int ret = poll(fds, 2, -1);
        if (ret > 0) {
            // Keyboard
            if (fds[1].revents & POLLIN) {
                if (read(STDIN_FILENO, &c, 1) > 0 && c == 'q')
                    break;
            }
            // Packet
            if (fds[0].revents & POLLIN) {
                ssize_t bytes = nic->read(buffer, sizeof(buffer));
                if (bytes >= 64) {
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
                            std::cout << "Unknown 0x" << std::hex << std::setw(4) << std::setfill('0') << htons(hdr->ethertype) << std::endl;
                            break;
                    }
                }
            }
        }

    }
    return 0;
}