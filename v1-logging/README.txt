libtslog - Thread-safe C++ logging library

Conteúdo:
- tslog.h
- tslog.cpp
- main_logger_test.cpp
- Makefile

Como compilar e testar (Linux):

    make
    ./test_logger 16 500 test_log.txt

OU, no terminal:
    g++ -std=c++17 -O2 -pthread -Wall -Wextra -c tslog.cpp
    g++ -std=c++17 -O2 -pthread -Wall -Wextra -c main_logger_test.cpp
    g++ -std=c++17 -O2 -pthread -Wall -Wextra -o test_logger tslog.o main_logger_test.o
    ./test_logger

Isso cria o arquivo test_log.txt com mensagens de múltiplas threads.
