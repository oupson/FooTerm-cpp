//
// Created by oupson on 01/03/2022.
//

#include "ssh.hpp"

#include <sys/socket.h>
#include <cstdio>
#include <netdb.h>
#include <stdexcept>
#include <fmt/core.h>

Session::Session() {
    this->session = nullptr;
    this->channel = nullptr;
    this->sock = -1;
}


Session::~Session() {
    if (this->isConnected()) {
        this->disconnect();
    }
}

int Session::openSocket(const char *addr, int port) {
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_flags |= AI_CANONNAME;
    hints.ai_protocol = 0;

    struct addrinfo *result;
    struct sockaddr *sin;
    unsigned int socklen;

    char service[256];
    sprintf(service, "%d", port);

    int s = getaddrinfo(addr, service, &hints, &result);
    if (s) {
        throw SessionConnectException(fmt::format("failed to resolve {}: {}", addr, gai_strerror(s)));
    }

    if (result) {
        sin = result->ai_addr;
        socklen = result->ai_addrlen;
    } else {
        // TODO
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw SessionConnectException(fmt::format("failed to create socket: {}", strerror(errno)));
    }


    if (connect(sock, sin, socklen) != 0) {
        throw SessionConnectException(fmt::format("failed to connect: {}", strerror(errno)));
    }

    return sock;
}

void Session::openConnection(const char *addr, int port, const char *username, const char *password) {
    char *error;
    int errorLen;

    this->sock = Session::openSocket(addr, port);
    this->session = libssh2_session_init();

    libssh2_session_set_blocking(this->session, 0);

    int rc;
    while ((rc = libssh2_session_handshake(this->session, sock)) == LIBSSH2_ERROR_EAGAIN) {}

    if (rc) {
        libssh2_session_last_error(session, &error, &errorLen, true);

        throw SessionConnectException(fmt::format("failure establishing SSH session: {}", error));
    }


    char *userauthlist;
    while ((userauthlist = libssh2_userauth_list(session, username, strlen(username))) == nullptr) {
        if (libssh2_session_last_errno(session) != LIBSSH2_ERROR_EAGAIN)
            break;
    }


    bool usePublicKey = true, usePassword = true;


#if DEBUG
    fprintf(stderr, "Authentication methods: %s\n", userauthlist);
#endif
    if (strstr(userauthlist, "publickey") == nullptr) {
#if DEBUG
        fprintf(stderr, "\"publickey\" authentication is not supported\n");
#endif
        usePublicKey = false;
    }

    if (strstr(userauthlist, "password") == nullptr || password == nullptr) {
#if DEBUG
        fprintf(stderr, "\"password\" authentication is not supported\n");
#endif
        usePassword = false;
    }


    /* Connect to the ssh-agent */
    LIBSSH2_AGENT *agent = libssh2_agent_init(session);

    if (!agent) {
        libssh2_session_last_error(session, &error, &errorLen, true);
        throw SessionConnectException(fmt::format("failure initializing ssh-agent support: {}", error));
    }

    if (libssh2_agent_connect(agent)) {
        libssh2_session_last_error(session, &error, &errorLen, true);
        throw SessionConnectException(fmt::format("failure connecting to ssh-agent: {}", error));
    }

    bool isConnected = false;

    if (usePublicKey)
        isConnected = Session::authWithPublicKey(agent, username);

    if (!isConnected && usePassword)
        isConnected = Session::authWithPassword(session, username, password);

    if (!isConnected) {
        libssh2_session_last_error(session, &error, &errorLen, true);
        throw SessionConnectException(fmt::format("Failed to authenticate: {}", error));
    }

    do {
        this->channel = libssh2_channel_open_session(session);
    } while (this->channel == nullptr && libssh2_session_last_errno(this->session) == LIBSSH2_ERROR_EAGAIN);

    if (!channel) {
        libssh2_session_last_error(session, &error, &errorLen, true);
        throw SessionConnectException(fmt::format("unable to open a session: {}", error));
    }

    do {
        rc = libssh2_channel_setenv(this->channel, "TERM", "xterm-256color");
    } while (rc == LIBSSH2_ERROR_EAGAIN);

    do {
        rc = libssh2_channel_request_pty(this->channel, "xterm-256color");
    } while (rc == LIBSSH2_ERROR_EAGAIN);

    do {
        rc = libssh2_channel_shell(this->channel);
    } while (rc == LIBSSH2_ERROR_EAGAIN);
}

void Session::disconnect() {
    libssh2_channel_free(this->channel);
    libssh2_session_free(this->session);

    this->channel = nullptr;
    this->session = nullptr;
    this->sock = -1;
}

bool Session::isConnected() {
    return this->channel != nullptr;
}

bool Session::authWithPublicKey(LIBSSH2_AGENT *agent, const char *username) {
    int rc;
    struct libssh2_agent_publickey *identity, *prev_identity = nullptr;

    while ((rc = libssh2_agent_list_identities(agent)) == LIBSSH2_ERROR_EAGAIN);

    if (rc) {
        fprintf(stderr, "Failure requesting identities to ssh-agent\n");
        return false;
    }


    while (true) {
        do {
            rc = libssh2_agent_get_identity(agent, &identity, prev_identity);
        } while (rc == LIBSSH2_ERROR_EAGAIN);

        if (rc == 1)
            break;

        if (rc < 0) {
            fprintf(stderr,
                    "Failure obtaining identity from ssh-agent support\n");
            return false;
        }

        do {
            rc = libssh2_agent_userauth(agent, username, identity);
        } while (rc == LIBSSH2_ERROR_EAGAIN);

        if (rc) {
#if DEBUG
            fprintf(stderr, "\tAuthentication with username %s and "
                            "public key %s failed!\n",
                    username, identity->comment);
#endif
        } else {
#if DEBUG
            fprintf(stderr, "\tAuthentication with username %s and "
                            "public key %s succeeded!\n",
                    username, identity->comment);
#endif
            break;
        }
        prev_identity = identity;
    }

    return rc == 0;
}

bool Session::authWithPassword(LIBSSH2_SESSION *session, const char *username, const char *password) {
    int rc;

    do {
        rc = libssh2_userauth_password(session, username, password);
    } while (rc == LIBSSH2_ERROR_EAGAIN);

    return rc == 0;
}
