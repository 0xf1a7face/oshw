#include <cstdlib>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

void run(const char *cmd, void (*callback)(const PROCESS_INFORMATION *)) {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  if (!CreateProcess(NULL, (char*)cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    printf("CreateProcess failed '%d'.\n", GetLastError());
    return;
  }

  if (callback) {
    (*callback)(&pi);
  }

  WaitForSingleObject(pi.hProcess, INFINITE);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

void callback_set_env(const PROCESS_INFORMATION *pi) {
  SetEnvironmentVariable("PROC_TO_KILL", "sleep.exe");
  run("kill.exe", NULL);
  SetEnvironmentVariable("PROC_TO_KILL", NULL);
}

void callback_name(const PROCESS_INFORMATION *pi) {
  run("kill.exe --name sleep.exe", NULL);
}

void callback_pid(const PROCESS_INFORMATION *pi) {
  char a[1024];
  snprintf(a, sizeof(a), "kill.exe --id %i", pi->dwProcessId);
  run(a, NULL);
}

int main() {
  printf("Killing by env\n");
  run("sleep.exe", callback_set_env);
  printf("Killing by name\n");
  run("sleep.exe", callback_name);
  printf("Killing by pid\n");
  run("sleep.exe", callback_pid);

  return 0;
}
