#include <iostream>
#include <string>

#include "SocketLibFunction.h"
#include "WrapTCPService.h"
#include "Connector.h"

using namespace brynet;
using namespace brynet::net;

int main(int argc, char **argv)
{
    if (argc != 6)
    {
        fprintf(stderr, "Usage: <host> <port> <net work thread num> <session num> <packet size> \n");
        exit(-1);
    }

    std::string tmp(atoi(argv[5]), 'a');

    auto server = std::make_shared<WrapTcpService>();
    server->startWorkThread(atoi(argv[3]));

    auto connector = AsyncConnector::Create();
    connector->startThread([server, tmp](sock fd, const std::any& ud) {
        std::cout << "connect success" << std::endl;
        server->addSession(fd, [tmp](const TCPSession::PTR& session) {
            session->setDataCallback([](const TCPSession::PTR& session, const char* buffer, size_t len) {
                session->send(buffer, len);
                return len;
            });
            session->send(tmp.c_str(), tmp.size());
        }, false, 1024 * 1024);
    }, [](const std::any&) {
        std::cout << "connect failed" << std::endl;
    });

    for (auto i = 0; i < atoi(argv[4]); i++)
    {
        connector->asyncConnect(argv[1], atoi(argv[2]), 10000, nullptr);
    }

    std::cin.get();
}