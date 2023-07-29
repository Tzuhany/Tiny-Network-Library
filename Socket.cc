#include "Socket.h"
#include "Logger.h"
#include "InetAddress.h"

#include <unistd.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/tcp.h>

Socket::~Socket() {
    close(sockfd_);
}

void Socket::bindAddress(const InetAddress &localAddr) const {
    if (0 != ::bind(sockfd_, (sockaddr *) localAddr.getSockAddr(), sizeof(sockaddr_in))) {
        LOG_FATAL("bind sockfd:%d fail \n", sockfd_);
    }
}

void Socket::listen() const {
    if (0 != ::listen(sockfd_, 1024)) {
        LOG_FATAL("listen sockfd:%d fail \n", sockfd_);
    }
}

int Socket::accept(InetAddress *peerAddr) const {
    sockaddr_in addr{};
    socklen_t len = sizeof addr;
    bzero(&addr, sizeof addr);
    int connfd = ::accept4(sockfd_, (sockaddr *) &addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0) {
        peerAddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::shutdownWrite() const {
    if (::shutdown(sockfd_, SHUT_WR) < 0) {
        LOG_ERROR("shutdownWrite error");
    }
}

void Socket::setTcpNoDelay(bool on) const {
    int optVal = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optVal, sizeof optVal);
}

void Socket::setReuseAddr(bool on) const {
    int optVal = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof optVal);
}

void Socket::setReusePort(bool on) const {
    int optVal = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optVal, sizeof optVal);
}

void Socket::setKeepAlive(bool on) const {
    int optVal = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optVal, sizeof optVal);
}