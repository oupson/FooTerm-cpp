//
// Created by oupson on 01/03/2022.
//

#ifndef FOOTERM_EVENTLOOP_HPP
#define FOOTERM_EVENTLOOP_HPP

#include <vector>
#include <sys/poll.h>
#include <unistd.h>
#include <libssh2.h>


struct EventLoopEntry {
    enum {
        DESCRIPTOR,
        CHANNEL
    } entryType;

    LIBSSH2_CHANNEL *channel;
    int out;
};

class EventLoop {
private:
    std::vector<pollfd> pfds;
    std::vector<EventLoopEntry> outs;
    bool isClosed{};
public:
    EventLoop();

    void registerFd(int fdin, EventLoopEntry entry);

    void run();

    static void start(EventLoop *self) {
        self->run();
    }
};

#endif //FOOTERM_EVENTLOOP_HPP
