#include <cstdlib>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

pid_t get_pid(const char *name);

bool kill_pid(pid_t pid);

bool kill_name(char *name) {
  if (!name) {
    return false;
  }

  bool ok = false;
  while (1) {
    pid_t pid = get_pid(name);
    if (!pid) {
      break;
    }
    ok |= kill_pid(pid);
  }

  if (!ok) {
    printf("Couldn't find '%s'\n", name);
  }

  return ok;
}

int main(int argc, char *argv[]) {
  pid_t pid = 0;
  char *name = NULL;

  option long_options[] = {{"pid", required_argument, NULL, 'p'},
                           {"id", required_argument, NULL, 'p'},
                           {"name", required_argument, NULL, 'n'},
                           {NULL, 0, NULL, 0}};

  int opt;
  int option_index;
  while ((opt = getopt_long(argc, argv, "", long_options, &option_index)) !=
         -1) {
    switch (opt) {
    case 'p':
      pid = std::atol(optarg);
      break;
    case 'n':
      name = optarg;
      break;
    default:
      printf("Error parsing option '%s'\n", optarg);
      return 1;
    }
  }

  char *env = getenv("PROC_TO_KILL");
  bool ok = false;
  ok |= kill_name(env);
  ok |= kill_name(name);
  ok |= kill_pid(pid);

  if (!ok) {
    printf("Nothing killed\n");
    return 1;
  }

  return 0;
}

#ifdef _WIN32

#include <windows.h>

#include <tlhelp32.h>

pid_t get_pid(const char *name) {
  pid_t pid = 0;
  HANDLE procs = NULL;
  PROCESSENTRY32 pe32 = {.dwSize = sizeof(PROCESSENTRY32)};

  procs = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (procs == INVALID_HANDLE_VALUE) {
    goto out;
  }

  if (!Process32First(procs, &pe32)) {
    goto out;
  }

  do {
    if (_stricmp(pe32.szExeFile, name) == 0) {
      pid = pe32.th32ProcessID;
      goto out;
    }
  } while (Process32Next(procs, &pe32));

out:
  CloseHandle(procs);
  return pid;
}

bool kill_pid(pid_t pid) {
  HANDLE proc = NULL;
  bool res = false;

  if (!pid) {
    goto out;
  }

  proc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
  if (proc == NULL) {
    printf("Failed to open process %lu (error: %lu)\n", (unsigned long)pid,
           GetLastError());
    goto out;
  }

  if (!TerminateProcess(proc, 42)) {
    printf("Failed to terminate process %lu (error: %lu)\n", (unsigned long)pid,
           GetLastError());
    goto out;
  }

  res = true;

out:
  CloseHandle(proc);
  return res;
}

#endif

#ifdef __linux__

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <signal.h>

pid_t get_pid(const char *name) {
  for (const auto &entry : std::filesystem::directory_iterator("/proc")) {
    const auto num = entry.path().filename().string();
    if (!entry.is_directory() ||
        !std::all_of(num.begin(), num.end(), ::isdigit)) {
      continue;
    }

    std::ifstream in(entry.path() / "cmdline");
    std::ostringstream sstr;
    sstr << in.rdbuf();
    std::string str = sstr.str();

    const char *prog = strrchr(str.c_str(), '/');
    if (prog) {
      prog += 1;
    } else {
      prog = str.c_str();
    }

    if (strcmp(prog, name) == 0) {
      return strtoll(num.c_str(), NULL, 10);
    }
  }

  return 0;
}

bool kill_pid(pid_t pid) {
  if (!pid) {
    return false;
  }

  if (kill(pid, SIGKILL) != 0) {
    printf("Error killing '%i': %s\n", pid, strerror(errno));
    return false;
  }

  return true;
}

#endif
