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

* Run c++ script
```bash
$ ./compile_and_run.sh helloworld.cpp
```
