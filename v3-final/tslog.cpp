#include "tslog.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <semaphore.h>   // POSIX semáforos

namespace tslog {

struct TSLogger::Impl {
    std::ofstream out;
    std::queue<std::string> q;
    std::mutex m;
    std::condition_variable cv;

    sem_t slots;  // controla espaço livre na fila
    sem_t items;  // controla quantidade de mensagens disponíveis

    std::thread worker;
    bool running{false};
    bool file_ok{false};
    size_t max_queue{100}; // limite da fila (pode ser ajustado)

    Impl(const std::string &path, bool append) {
        std::ios_base::openmode mode = std::ios::out;
        if (append) mode |= std::ios::app;
        out.open(path, mode);
        file_ok = out.is_open();

        sem_init(&slots, 0, max_queue); // todos os slots livres no início
        sem_init(&items, 0, 0);         // nenhum item disponível no início

        running = true;
        worker = std::thread(&Impl::run, this);
    }

    ~Impl() {
        shutdown();
        if (worker.joinable()) worker.join();
        if (out.is_open()) out.close();
        sem_destroy(&slots);
        sem_destroy(&items);
    }

    static std::string now_string() {
        auto t = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(t);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch()) % 1000;
        std::ostringstream oss;
        std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm, &tt);
#else
        localtime_r(&tt, &tm);
#endif
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "." 
            << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    static std::string level_to_string(Level l) {
        switch (l) {
            case Level::DEBUG: return "DEBUG";
            case Level::INFO:  return "INFO";
            case Level::WARN:  return "WARN";
            case Level::ERROR: return "ERROR";
        }
        return "UNK";
    }

    void enqueue(Level l, const std::string &msg) {
        // espera por slot livre
        sem_wait(&slots);

        std::ostringstream oss;
        oss << now_string() << " [" << level_to_string(l) << "] " << msg << "\n";

        {
            std::lock_guard<std::mutex> lk(m);
            q.push(oss.str());
        }

        // sinaliza que há um item disponível
        sem_post(&items);
    }

    void run() {
        while (running) {
            // aguarda item disponível
            sem_wait(&items);

            std::string msg;
            {
                std::lock_guard<std::mutex> lk(m);
                if (!q.empty()) {
                    msg = q.front();
                    q.pop();
                }
            }

            // libera um slot (já que consumiu um item)
            sem_post(&slots);

            if (!msg.empty() && file_ok) {
                out << msg;
            }
        }

        // flush final
        if (file_ok) out.flush();
    }

    void flush_blocking() {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&]{ return q.empty(); }); // espera até fila ficar vazia
        if (file_ok) out.flush();
    }

    void shutdown() {
        running = false;
        // garante que o worker saia do sem_wait
        sem_post(&items);
    }
};

TSLogger::TSLogger(const std::string &path, bool append)
    : pimpl(std::make_unique<Impl>(path, append)) {}

TSLogger::~TSLogger() {
    if (pimpl) pimpl->shutdown();
}

void TSLogger::log(Level level, const std::string &msg) {
    pimpl->enqueue(level, msg);
}

void TSLogger::debug(const std::string &msg) { log(Level::DEBUG, msg); }
void TSLogger::info(const std::string &msg)  { log(Level::INFO,  msg); }
void TSLogger::warn(const std::string &msg)  { log(Level::WARN,  msg); }
void TSLogger::error(const std::string &msg) { log(Level::ERROR, msg); }

void TSLogger::flush() {
    pimpl->flush_blocking();
}

void TSLogger::shutdown() {
    if (pimpl) pimpl->shutdown();
}

} // namespace tslog
