
#include "tslog.h"
#include <thread>
#include <vector>
#include <iostream>
#include <random>
#include <sstream>

int main(int argc, char **argv) {
    const int num_threads = (argc > 1) ? std::stoi(argv[1]) : 8;
    const int messages_per_thread = (argc > 2) ? std::stoi(argv[2]) : 100;
    const std::string logfile = (argc > 3) ? argv[3] : "test_log.txt";

    tslog::TSLogger logger(logfile, true);

    std::cout << "Iniciando teste: threads=" << num_threads << " msgs/thread=" << messages_per_thread << " -> " << logfile << "\n";

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([t, messages_per_thread, &logger]() {
            std::random_device rd; std::mt19937_64 gen(rd()); 
            std::uniform_int_distribution<int> dist(1, 10);
            for (int i = 0; i < messages_per_thread; ++i) {
                std::ostringstream oss;
                oss << "thread=" << t << " msg_index=" << i;
                logger.info(oss.str());
                // small sleep to create interleaving
                std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
            }
        });
    }

    for (auto &th : threads) if (th.joinable()) th.join();

    // garante que todas as mensagens sejam escritas
    logger.flush();
    logger.shutdown();

    std::cout << "Teste finalizado. Confira " << logfile << "\n";
    return 0;
}
