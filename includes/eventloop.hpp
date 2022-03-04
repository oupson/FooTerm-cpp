//
// Created by oupson on 01/03/2022.
//

#ifndef FOOTERM_EVENTLOOP_HPP
#define FOOTERM_EVENTLOOP_HPP

#include <vector>
#include <sys/poll.h>
#include <unistd.h>
#include <libssh2.h>
#include <libssh2_sftp.h>

class FooTermWindow;

struct EventLoopEntry {
    enum {
        DESCRIPTOR,
        CHANNEL
    } entryType;

    union {
        LIBSSH2_CHANNEL *channel;
        LIBSSH2_SFTP *sftp;
    };

    union {
        int fdout;
    };
};

class EventLoop {
private:
    std::vector<pollfd> pfds;
    std::vector<EventLoopEntry> outs;
    bool isClosed{};
    FooTermWindow* window;

public:
    explicit EventLoop(FooTermWindow* window);

    void registerFd(int fdin, EventLoopEntry entry);

    [[noreturn]] void run();

    static void start(EventLoop *self) {
        self->run();
    }
};

#endif //FOOTERM_EVENTLOOP_HPP
