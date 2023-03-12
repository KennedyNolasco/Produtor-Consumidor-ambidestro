# Produtor-Consumidor-ambidestro
Código em C do problema clássico do produtor consumidor, mas com três tipos de threads:
- Produtor: Produz um elemento e coloca no buffer 1 se este não estiver cheio.
- Consumidor: Consome um elemento do buffer 2 se este não estiver vazio.
- Ambos: Consome um elemento do buffer 1 se este não estiver vazio e produz no buffer 2 se este não estiver cheio.

O programa acaba após todas as produções serem consumidas. 
