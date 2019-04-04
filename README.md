# LRU-DB-Queries-Cache
Least Recently Used Database Queries Cache for multi-threaded environment

* Postgres setup: make sure you set all environment variables as example following
```bash
$ echo "export USER_NODECRUD=\"xxx\"" >> ~/.bashrc
$ echo "export HOST_NODECRUD=\"xxx\"" >> ~/.bashrc
$ echo "export DATABASE_NODECRUD=\"xxx\"" >> ~/.bashrc
$ echo "export PASSWORD_NODECRUD=\"xxx\"" >> ~/.bashrc
```

* C++ libraries setup:
```bash
$ ./setup.sh
```

* Run c++ script:
```bash
$ ./compile_and_run.sh helloworld.cpp
```

# About
Project developed by [Igor Bragaia](https://github.com/igorbragaia) and [Luiz Felipe Schiaveto](https://github.com/luizfschiaveto) for CES-33 - Operational Systems college course

**Professor [Cecília de Azevedo Castro César](http://www.comp.ita.br/~cecilia/index.html)**

Em aplicações que possuem bancos de dados que suportam grande quantidade de queries, é comum consultas repetidas serem feitas constantemente. Um exemplo disso é a página de entrada de um website que exibe em sua principal página uma série de dados do banco de dados. A medida que essas consultas tornem-se dispendiosas computacionalmente, é razoável considerar um cache de queries, de forma que a concorrência de consultas não será diminuída - pois as consultas ao cache ainda se fazem necessárias - mas passarão a ter um tempo de look up constante e muito menor que o tempo da query em si.
Para a realização do projeto de queries cache, optou-se por um cache do tipo LRU - Least Recently Used, que consiste basicamente de uma estrutura de dados que suporta duas operações:

* **get(key)** retorna o valor armazenado para dada key. No contexto de queries cache, a chave será uma string de query SQL.
* **put(key, value)** adiciona uma query SQL (key) e seu respectivo response (value) ao cache.

Nota-se que o LRU Cache possui uma quantidade máximas de keys a serem armazenadas e a medida que o cache atinge sua capacidade, os itens menos consultados são descartados. Além disso, o cache é implementado por meio de listas ligadas e hash tables, de forma que a complexidade de tempo para look up e updates é aproximadamente constante.
Por fim, o projeto foi implementado em linguagem C++, interagindo com um banco de dados PostgreSQL em produção deployado por meio do Heroku. O banco de dados pode ser acessado interativamente em https://goo.gl/oeRbiQ , página web desenvolvida em ReactJS e NodeJS para facilitar debugging e avaliação do professor. As consultas multi-threaded, por sua vez, são otimizadas por meio do cache das consultas feitas mais frequentemente ao banco de dados. No presente projeto apresentam-se gráficos comparativos entre o desempenho com e sem o uso do cache para consultas sucessivas e repetidas ao banco de dados remoto demonstrando a validade do protótipo desenvolvido.
