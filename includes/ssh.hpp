//
// Created by oupson on 01/03/2022.
//

#ifndef FOOTERM_SSH_HPP
#define FOOTERM_SSH_HPP

#include <libssh2.h>
#include <libssh2_sftp.h>
#include <utility>
#include <vector>
#include <string>

class Session {
private:
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channel;
    int sock;
    LIBSSH2_SFTP* sftpSession;

    static int openSocket(const char *addr, int port);

    static bool authWithPublicKey(LIBSSH2_AGENT *agent, const char *username);

    static bool authWithPassword(LIBSSH2_SESSION *session, const char *username, const char *password);

public:
    explicit Session();

    ~Session();

    void openConnection(const char *addr, int port, const char *username, const char *password);

    void disconnect();

    bool isConnected();

    [[nodiscard]] int getSock() const {
        return this->sock;
    };

    LIBSSH2_SESSION *getSession() {
        return this->session;
    }

    LIBSSH2_CHANNEL *getChannel() {
        return this->channel;
    }

    LIBSSH2_SFTP* getSftp() {
        return this->sftpSession;
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
