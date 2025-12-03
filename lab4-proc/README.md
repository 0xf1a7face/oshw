## Unix

```
c++ sleep.cpp -o sleep
c++ kill.cpp -o kill
c++ user_unix.cpp -o user
./user
```

## Win

```
x86_64-w64-mingw32-g++ -static sleep.cpp -o sleep.exe
x86_64-w64-mingw32-g++ -static kill.cpp -o kill.exe
x86_64-w64-mingw32-g++ -static user_win.cpp -o user.exe
wine64 ./user.exe
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
