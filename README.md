# LRU-DB-Queries-Cache

Least Recently Used Database Queries Cache for multi-threaded environment

## About

<img align="left" src="assets/ita.png" alt="alt text" width="60px">

Project developed by [Igor Bragaia](https://github.com/igorbragaia) and [Luiz Felipe Schiaveto](https://github.com/luizfschiaveto) for CES-33 - Operational Systems college course

**Professor [Cecília de Azevedo Castro César](http://www.comp.ita.br/~cecilia/index.html)**

In applications that have database which supports high amount of queries, it is common repeated ones often. An example is the main webpage from an website that displays some information retrieved from database got through an async fetch everytime webpage loads. As queries get heavy computationally, it is reasonable consider a queries caching which can return responses highly quicker than querying database in fact.

In order to realize this query caching project, team choosed to implement a LRU - Least Recently Used cache, which basically consists of a data structure that supports two operations, they are

* **get(key)** returns value cached for given key. It will be a SQL query string
* **put(key, value)** caches a SQL query string and its respective response

Note that LRU Cache has a maximum data to be cached in order to avoid stack overflow so no queried elements are dropped once cache gets full. Furthermore, this cache is implemented using linked lists and hash tables in order to accomplish an amortized look up and update time complexities.

Finally, this project has been implemented in C++, interacting with Postgres database deployed in production environment through Heroku. There is also a ReactJS+NodeJS simple web app that allows database data viewing and editing, it is deployed in production environment through Heroku and is available at [https://goo.gl/oeRbiQ](https://goo.gl/oeRbiQ).

## Running project

* Code: GitHub repository [here](https://github.com/igorbragaia/LRU-DB-Queries-Cache)

* Postgres setup: make sure you set all environment variables as example following and queries are reasonable for this database
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

* Run C++ script:
```bash
$ ./compile_and_run.sh queries.cpp
```
