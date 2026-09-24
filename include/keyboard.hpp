#pragma once

class Keyboard {
    public:
        Keyboard();
        ~Keyboard();

        bool read(char& c);
    private:
#ifdef _WIN32
    // Nothing required here
#else
    struct TermiosState;
    TermiosState* _state;
#endif
};
