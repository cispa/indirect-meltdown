#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

long current_nanos() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return time.tv_sec * 1000000 + time.tv_usec;
}

/* https://www.gnu.org/software/libc/manual/html_node/Noncanon-Example.html */
struct termios saved_attributes;

void reset_input_mode(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode(void) {
  struct termios tattr;
  char *name;

  // Make sure stdin is a terminal.
  if (!isatty(STDIN_FILENO)) {
    fprintf(stderr, "Not a terminal.\n");
    exit(EXIT_FAILURE);
  }

  // Save the terminal attributes so we can restore them later.
  tcgetattr(STDIN_FILENO, &saved_attributes);
  atexit(reset_input_mode);

  // Set the funny terminal modes.
  tcgetattr(STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON | ECHO);  // Clear ICANON and ECHO.
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

int main() {
    set_input_mode();
    for (int i=0; i<200; i++) {
        getchar();
        printf("%ld\n", current_nanos());
    }
    usleep(500);
    system("pkill probe_idt");
    system("pkill spy");
    return 0;
}
