#include "cryptoquery.h"
#include "database.h"
#include "encryption.h"
#include "common.h"
#include "configuration.h"

#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

unsigned int verbose;
char * ec_param;

static void usage(char *name) {
  fprintf(stderr, "Usage: %s [-v] -f infile <command>", name);
}

static int parse_options(int argc, char *argv[]) {
  char * opts = "";
  unsigned int ret = CQ_OK;
  int ch;
  while (-1 != ((ch = getopt(argc, argv, opts)))) {
    switch (ch) {
    case 'p': // EC configuration
      ec_param = malloc(sizeof(strlen(optarg) + 1));
      strncpy(ec_param, optarg, strlen(optarg) + 1);
      break;
    case 'v': // verbose
      verbose = 1;
      break;
    case '?':
    case 'h':
    default:
      usage(argv[0]);
      ret = CQ_ERROR;
      break;
    }
  }

  return ret;
}

static unsigned int get_command() {
  char * line = readline("Enter command: ");
  unsigned int command = CQ_UNKOWN;

  if (NULL == line)
    command = CQ_QUIT;
  else if (strlen(line) == strlen("init") && 0 == strncmp(line, "init", strlen("init"))) {
    // init
    command = CQ_INIT;
  } else if (strlen(line) == strlen("keygen") && 0 == strncmp(line, "keygen", strlen("keygen"))) {
    // keygen
    command = CQ_KEYGEN;
  } else if (strlen(line) == strlen("select") && 0 == strncmp(line, "select", strlen("select"))) {
    // select
    command = CQ_SELECT;
  } else if (strlen(line) == strlen("insert") && 0 == strncmp(line, "insert", strlen("insert"))) {
    // insert
    command = CQ_INSERT;
  } else if (strlen(line) == strlen("quit") && 0 == strncmp(line, "quit", strlen("quit"))) {
    // quit
    command = CQ_QUIT;
  }

  free(line);

  return command;
}

static void quit() {
  save_configuration();
  close_database();
  printf("Bye!\n");
}

static void init_system() {
  
}

int main(int argc, char *argv[]) {
  if (CQ_OK != parse_options(argc, argv))
    return 1;

  int command;
  do {
    command = get_command();

    if (verbose) printf("command: %d\n", command);
    
    switch (command) {
    case CQ_INSERT:
      //insert_query();
      break;
    case CQ_SELECT:
      //select_query();
      break;
    case CQ_QUIT:
      quit();
    case CQ_INIT:
      init_system();
      break;
    default:
      fprintf(stderr, "Unkown command\n");
      break;
    }
  } while (CQ_QUIT != command);


  return 0;
}
