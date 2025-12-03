#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void run(const char *const argv[], void (*callback)(pid_t)) {
  pid_t pid = fork();
  if (pid == 0) {
    execve(argv[0], (char *const *)argv, environ);
    printf("Error spawning '%s': %s\n", argv[0], strerror(errno));
    exit(1);
  }

  if (callback) {
    (*callback)(pid);
  }

  if (waitpid(pid, NULL, 0) < 0) {
    printf("Error waiting for '%s': %s\n", argv[0], strerror(errno));
    exit(1);
  }
}

void callback_set_env(pid_t pid) {
  setenv("PROC_TO_KILL", "sleep", 1);
  const char *const args[] = {"./kill", NULL};
  run(args, NULL);
  unsetenv("PROC_TO_KILL");
}

void callback_name(pid_t pid) {
  const char *const args[] = {"./kill", "--name", "sleep", NULL};
  run(args, NULL);
}

void callback_pid(pid_t pid) {
  char a[1024];
  snprintf(a, sizeof(a), "%i", pid);

  const char *const args[] = {"./kill", "--id", a, NULL};
  run(args, NULL);
}

int main() {
  const char *const args[] = {"./sleep", NULL};

  printf("Killing by env\n");
  run(args, callback_set_env);
  printf("Killing by name\n");
  run(args, callback_name);
  printf("Killing by pid\n");
  run(args, callback_pid);

  return 0;
}
