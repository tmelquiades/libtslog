# Chat Multiusuário em C++ (Cliente/Servidor TCP com Logging Concorrente)

## 📌 Visão Geral

Este projeto implementa um **sistema de chat multiusuário** baseado em **TCP**, onde:

* O **servidor** aceita múltiplos clientes simultâneos (cada cliente é tratado por uma thread).
* As mensagens enviadas por um cliente são retransmitidas (broadcast) para os demais.
* O sistema utiliza a biblioteca **libtslog** para logging concorrente, garantindo segurança em acessos a arquivos de log.
* Há scripts auxiliares para simular múltiplos clientes conectando e enviando mensagens.

## 🗂 Estrutura do Projeto

```
├── tslog.h / tslog.cpp   # Biblioteca de logging thread-safe
├── server.cpp            # Servidor TCP multiusuário
├── client.cpp            # Cliente CLI
├── Makefile              # Automação da compilação
├── test_clients.sh       # Script de simulação de múltiplos clientes
└── README.md             # Documentação do projeto
```

## ⚙️ Requisitos

* **C++17 ou superior**
* Compilador com suporte a **POSIX sockets** e **threads** (ex.: g++, clang++)
* Ambiente Unix/Linux (para sockets TCP e `pthread`)

## 🚀 Compilação

Você pode compilar manualmente ou via `Makefile`.

### Usando Makefile

```bash
sed -i 's/^    /\t/' Makefile
make
```

### Manualmente

```bash
g++ -std=c++17 -O2 -pthread server.cpp tslog.cpp -o server
g++ -std=c++17 -O2 -pthread client.cpp -o client
```

## ▶️ Execução

### 1. Iniciar o servidor

```bash
./server
```

O servidor escutará na porta **5555** e registrará eventos em `server.log`.

### 2. Conectar um cliente

Em outro terminal:

```bash
./client
```

Digite mensagens — elas serão enviadas ao servidor e retransmitidas para todos os clientes conectados.

### 3. Testar múltiplos clientes automaticamente

```bash
bash test_clients.sh 5
```

Isso cria 5 clientes que enviam mensagens automaticamente.

## 🛠 Funcionalidades

* Conexão TCP concorrente
* Threads por cliente
* Broadcast de mensagens
* Logging thread-safe
* Script de simulação

## 📊 Logging

Todos os eventos são registrados em `server.log`, incluindo:

* Conexões de clientes
* Mensagens recebidas
* Desconexões

---
