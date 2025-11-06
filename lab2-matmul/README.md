## Std threads

```
$ c++ -O2 -std=c++20 -D USE_STDTHREAD main.cpp -o a.out
$ ./a.out
```

## Posix threads

```
$ c++ -O2 -std=c++20 -D USE_PTHREAD main.cpp -o a.out
$ ./a.out
```

## Win threads

```
$ x86_64-w64-mingw32-g++ -static -O2 -std=c++20 -D USE_WINTHREAD main.cpp -o a.exe
$ wine64 ./a.exe
```

### Cross-compilation dependencies

- Ubuntu
  ```
  $ sudo apt install g++-mingw-w64-x86-64 gcc-mingw-w64-x86-64 wine
  ```
- NixOS
  ```
  $ nix-shell -p pkgsCross.mingwW64.stdenv.cc wine
  ```
