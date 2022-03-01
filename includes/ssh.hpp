//
// Created by oupson on 01/03/2022.
//

#ifndef FOOTERM_SSH_HPP
#define FOOTERM_SSH_HPP

#include <libssh2.h>
#include <utility>
#include <vector>
#include <string>

class Session {
private:
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channel;
    int sock;

    static int openSocket(const char *addr, int port);

    static bool authWithPublicKey(LIBSSH2_AGENT *agent, const char *username);

    static bool authWithPassword(LIBSSH2_SESSION *session, const char *username, const char *password);

public:
    explicit Session();

    ~Session();

    void openConnection(const char *addr, int port, const char *username, const char *password);

    void disconnect();

    bool isConnected();

    int getSock() {
        return this->sock;
    };

    LIBSSH2_CHANNEL *getChannel() {
        return this->channel;
    }
};

class SessionConnectException : public std::exception {
public:
    explicit SessionConnectException(std::string msg) : m_msg(std::move(msg)) {
    }

    [[nodiscard]] const char *what() const noexcept override {
        return m_msg.c_str();
    }

    const std::string m_msg;
};


#endif //FOOTERM_SSH_HPP
