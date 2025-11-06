## Lab3 buffered channel

В `main.cpp` есть что-то вроде тестов: sanity-check и многопоточный fuzzing.
Реализация канала в `buffered_channel.h`.

```
$ c++ -O2 main.cpp
$ ./a.out
Sanity ok
Completed 100 fuzzing rounds chan_cap=20, threads=101
```
