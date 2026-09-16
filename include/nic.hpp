#pragma once
#include <cstdint>
#include <string>

class NIC {
public:
    virtual bool open(const std::string& name) = 0;
    virtual std::ptrdiff_t read(uint8_t* buffer, size_t len) = 0;
    virtual void close() = 0;
    virtual ~NIC() = default;
    virtual int getfd() = 0;
};

class NIC_Linux : public NIC {
public:
    NIC_Linux();
    ~NIC_Linux();

    bool open(const std::string& name) override;
    std::ptrdiff_t read(uint8_t* buffer, size_t len) override;
    void close() override;
    int getfd() override;

private:
    int _fd;
};

class NIC_Windows : public NIC {
public:
    NIC_Windows();
    ~NIC_Windows();

    bool open(const std::string& name) override;
    std::ptrdiff_t read(uint8_t* buffer, size_t len) override;
    void close() override;
    int getfd() override;

private:
    int _fd; // check Npcap (discord)
    void* pcap = nullptr;
};
