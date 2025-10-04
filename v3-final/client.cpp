#include <iostream>
#include <thread>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

const int PORT = 5555;


void receive_messages(int sockfd) {
        char buffer[1024];

        while (true) {
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(sockfd, buffer, sizeof(buffer)-1, 0);
                if (bytes <= 0) break;
                std::cout << "[Broadcast] " << std::string(buffer, bytes) << std::endl;
        }     
}


int main() {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in servaddr{};
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);


        if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
                std::cerr << "Erro ao conectar ao servidor." << std::endl;
                return 1;
        }

        std::thread receiver(receive_messages, sockfd);
        std::string msg;
        while (std::getline(std::cin, msg)) {
                send(sockfd, msg.c_str(), msg.size(), 0);
        }

        close(sockfd);
        receiver.join();
        return 0;
}