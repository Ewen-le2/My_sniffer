#include "keyboard.hpp"

#ifdef _WIN32
    #include <conio.h>

    Keyboard::Keyboard() {}

    Keyboard::~Keyboard() {}

    bool Keyboard::read(char& c) {
        if (!_kbhit())
            return false;

        c = static_cast<char>(_getch());
        return true;
    }
#else
    #include <termios.h>
    #include <unistd.h>
    #include <poll.h>

    struct Keyboard::TermiosState {
        termios original{};
    };

    Keyboard::Keyboard() : _state(new TermiosState{})
    {
        if (tcgetattr(STDIN_FILENO, &_state->original) != 0) {
            delete _state;
            _state = nullptr;
            return;
        }

        termios raw = _state->original;

        // Instant, don't need enter
        raw.c_lflag &= ~ICANON;

        // Disable echo
        raw.c_lflag &= ~ECHO;

        // Don't make read() wait for a character
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;

        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }

    Keyboard::~Keyboard()
    {
        if (_state) {
            tcsetattr(
                STDIN_FILENO,
                TCSANOW,
                &_state->original
            );

            delete _state;
            _state = nullptr;
        }
    }

    bool Keyboard::read(char& c)
    {
        if (!_state)
            return false;

        struct pollfd fd{};

        fd.fd = STDIN_FILENO;
        fd.events = POLLIN;

        int result = poll(&fd, 1, 0);

        if (result <= 0) return false;

        if (!(fd.revents & POLLIN)) return false;

        return ::read(STDIN_FILENO, &c, 1) == 1;
    }

#endif
