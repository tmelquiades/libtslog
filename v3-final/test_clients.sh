NUM_CLIENTS=${1:-5}

for i in $(seq 1 $NUM_CLIENTS); do
        (echo "Cliente $i conectado"; sleep 2; echo "Mensagem do cliente $i"; sleep 1) | ./client &
done

wait