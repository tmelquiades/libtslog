#include "tslog.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace tslog {

struct TSLogger::Impl {
    std::ofstream out;
    std::mutex m;
    std::condition_variable cv;
    std::queue<std::string> q;
    std::thread worker;
    bool running{false};
    bool file_ok{false};
    Level min_level{Level::DEBUG};

    Impl(const std::string &path, bool append) {
        std::ios_base::openmode mode = std::ios::out;
        if (append) mode |= std::ios::app;
        out.open(path, mode);
        file_ok = out.is_open();
        running = true;
        worker = std::thread(&Impl::run, this);
    }

    ~Impl() {
        // garante shutdown se o cliente esquecer
        {
            std::unique_lock<std::mutex> lk(m);
            running = false;
            cv.notify_all();
        }
        if (worker.joinable()) worker.join();
        if (out.is_open()) out.close();
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
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3) << ms.count();
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
        std::ostringstream oss;
        oss << now_string() << " [" << level_to_string(l) << "] " << msg << "\n";
        {
            std::lock_guard<std::mutex> lk(m);
            q.push(oss.str());
        }
        cv.notify_one();
    }

    void run() {
        while (true) {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [&]{ return !q.empty() || !running; });
            while (!q.empty()) {
                auto s = q.front(); q.pop();
                lk.unlock();
                if (file_ok) {
                    out << s;
                }
                lk.lock();
            }
            if (!running && q.empty()) break;
        }
        // final flush
        if (file_ok) out.flush();
    }

    void flush_blocking() {
        // espera até que a fila esteja vazia
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&]{ return q.empty(); });
        if (file_ok) out.flush();
    }
};

TSLogger::TSLogger(const std::string &path, bool append)
    : pimpl(std::make_unique<Impl>(path, append)) {}

TSLogger::~TSLogger() {
    // pimpl será destruído automaticamente
}

void TSLogger::log(Level level, const std::string &msg) {
    if (!pimpl) return;
    pimpl->enqueue(level, msg);
}

void TSLogger::debug(const std::string &msg) { log(Level::DEBUG, msg); }
void TSLogger::info(const std::string &msg)  { log(Level::INFO,  msg); }
void TSLogger::warn(const std::string &msg)  { log(Level::WARN,  msg); }
void TSLogger::error(const std::string &msg) { log(Level::ERROR, msg); }

void TSLogger::flush() {
    if (!pimpl) return;
    pimpl->flush_blocking();
}

void TSLogger::shutdown() {
    if (!pimpl) return;
    {
        std::lock_guard<std::mutex> lk(pimpl->m);
        pimpl->running = false;
    }
    pimpl->cv.notify_all();
    if (pimpl->worker.joinable()) pimpl->worker.join();
    if (pimpl->out.is_open()) pimpl->out.close();
}

}
