//
// Created by oupson on 01/03/2022.
//

#include "eventloop.hpp"
#include <libssh2.h>
#include <iostream>

EventLoop::EventLoop() = default;

void EventLoop::registerFd(int fdin, EventLoopEntry entry) {
    pollfd fd = {0};
    fd.fd = fdin;
    fd.events = POLLIN;
    this->pfds.emplace_back(fd);
    this->outs.emplace_back(entry);
}

#define BUFFER_SIZE (256)

void EventLoop::run() {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    int res;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (!this->isClosed) {
        res = poll(this->pfds.data(), this->pfds.size(), 1000);

        if (res > 0) {
            int i = 0;

            while (i < this->pfds.size()) {
                pollfd &pfd = this->pfds[i];

                if (pfd.revents & POLLIN) {
                    do {
                        if (this->outs[i].entryType == EventLoopEntry::CHANNEL) {
                            bytesRead = libssh2_channel_read(this->outs[i].channel, buffer, BUFFER_SIZE);

                            if (bytesRead > 0) {
                                write(this->outs[i].out, buffer, bytesRead);
                            } else if (bytesRead < 0 && bytesRead != LIBSSH2_ERROR_EAGAIN) {
                                std::cout << bytesRead << std::endl;
                            }

                            if (libssh2_channel_eof(this->outs[i].channel)) {
                                this->outs.erase(this->outs.begin() + i);
                                this->pfds.erase(this->pfds.begin() + i);
                                continue;
                            }
                        } else {
                            bytesRead = read(pfd.fd, buffer, BUFFER_SIZE);


                            if (bytesRead > 0) {
                                libssh2_channel_write(this->outs[i].channel, buffer, bytesRead);
                            }
                        }
                    } while (bytesRead > 0);
                }
                i++;
            }

        }
    }
#pragma clang diagnostic pop
}