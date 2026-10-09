#pragma once
#include <cstdint>
#include <memory>
#include <string>

#ifdef _WIN32
    #include "pcap.h"
#endif


class NIC {
    public:
        virtual bool open(const std::string& name) = 0;
        virtual std::ptrdiff_t read(uint8_t* buffer, size_t len) = 0;
        virtual void close() = 0;
        virtual ~NIC() = default;
};

class NIC_Factory {
    public:
        NIC_Factory();
        ~NIC_Factory();
        void CreateNIC(std::unique_ptr<NIC> &nic);
};

#ifdef __linux__

    class NIC_Linux : public NIC {
    public:
        NIC_Linux();
        ~NIC_Linux();

        bool open(const std::string& name) override;
        std::ptrdiff_t read(uint8_t* buffer, size_t len) override;
        void close() override;

    private:
        int _fd;
    };
#endif

#ifdef _WIN32
    class NIC_Windows : public NIC {
    public:
        NIC_Windows();
        ~NIC_Windows();

        bool open(const std::string& name) override;
        std::ptrdiff_t read(uint8_t* buffer, size_t len) override;
        void close() override;

    private:
        pcap_t* pcap = nullptr;
    };
#endif