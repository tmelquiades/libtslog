Chat Cliente/Servidor (C++17, TCP, Multi-threaded)

Conteúdo da v2:
- server.cpp (servidor TCP com broadcast e logging)
- client.cpp (cliente CLI simples)
- tslog.cpp/h (biblioteca de logging thread-safe, da v1)
- Makefile (compila servidor e cliente)
- test_clients.sh (simulação de múltiplos clientes)
+ arquivos da v1

Como compilar (se necessário, revisite o README da v1):
sed -i 's/^    /\t/' Makefile
make

Executar servidor:
./server   

Executar cliente manual:
./client

Executar teste com 5 clientes simulados:
bash test_clients.sh 5
