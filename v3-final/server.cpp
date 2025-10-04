#include "tslog.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <set>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

const int PORT = 5555;

std::set<int> clients;
std::mutex clients_mtx;
tslog::TSLogger logger("server.log", true);

void broadcast(const std::string &msg, int sender_fd) {
    std::lock_guard<std::mutex> lock(clients_mtx);
    for (int fd : clients) {
        if (fd != sender_fd) {
        send(fd, msg.c_str(), msg.size(), 0);
        }
    }
}

void handle_client(int client_fd) {
    char buffer[1024];
    logger.info("Novo cliente conectado: fd=" + std::to_string(client_fd));

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_fd, buffer, sizeof(buffer)-1, 0);
        if (bytes <= 0) break;
        std::string msg = std::string(buffer, bytes);
        logger.info("Mensagem recebida de fd=" + std::to_string(client_fd) + ": " + msg);
        broadcast(msg, client_fd);
    }

    close(client_fd);
    {
        std::lock_guard<std::mutex> lock(clients_mtx);
        clients.erase(client_fd);
    }
        logger.warn("Cliente desconectado: fd=" + std::to_string(client_fd));
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);

    logger.info("Servidor iniciado na porta " + std::to_string(PORT));

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        
        {
            std::lock_guard<std::mutex> lock(clients_mtx);
            clients.insert(client_fd);
        }

        std::thread(handle_client, client_fd).detach();
    }


    close(server_fd);
    logger.shutdown();
    return 0;
}