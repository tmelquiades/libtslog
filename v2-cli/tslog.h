#pragma once

#include <string>
#include <memory>
#include <fstream>

namespace tslog {

enum class Level { DEBUG, INFO, WARN, ERROR };

class TSLogger {
public:
    // Cria (ou inicializa) um logger que grava em `path`. Se append==true, abre em modo append.
    explicit TSLogger(const std::string &path, bool append = true);
    ~TSLogger();

    // Desabilita cópia/movimento simples (simplifica controle de instância)
    TSLogger(const TSLogger &) = delete;
    TSLogger &operator=(const TSLogger &) = delete;

    // API principal
    void log(Level level, const std::string &msg);

    // Conveniências
    void debug(const std::string &msg);
    void info(const std::string &msg);
    void warn(const std::string &msg);
    void error(const std::string &msg);

    // Força escrita no arquivo (bloqueante, garante que fila esteja vazia)
    void flush();

    // Fecha o logger, finaliza o worker e garante persistência
    void shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

}
