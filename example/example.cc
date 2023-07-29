#include <zhnetlib/TcpServer.h>

class EchoServer {
public:
    EchoServer(EventLoop *loop, const InetAddress &addr, const std::string &name)
            : server_(loop, addr, name),
              loop_(loop) {
        // 注册回调函数
        server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2,
                                             std::placeholders::_3));
        server_.setThreadNum(16);
    }

    void start() {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr &conn) {
        if (conn->connected()) {
            std::cout << "Connection Up : %s", conn->peerAddress().toIpPort().c_str();
        } else {
            std::cout << "Connection Down : %s", conn->peerAddress().toIpPort().c_str();
        }
    }

    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time) {
        std::string msg = buf->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown();
    }

    EventLoop *loop_;
    TcpServer server_;
};

int main() {
    EventLoop loop;
    InetAddress addr(8000);
    EchoServer server(&loop, addr, "EchoServer-01");
    server.start();
    loop.loop();

    return 0;
}