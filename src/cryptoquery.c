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
char * db_location;

static void print_missing(int command) {
  fprintf(stderr, "Option -%c requires an argument", (char) command);
}

static void usage(char *name) {
  fprintf(stderr, "Usage: %s [-v] -f infile <command>", name);
}

static int parse_options(int argc, char *argv[]) {
  char * opts = "p:d:vh";
  unsigned int ret = CQ_OK;
  int ch, lch;
  verbose = 0;
  ec_param = db_location = NULL;

  while (-1 != ((ch = getopt(argc, argv, opts)))) {
    switch (ch) {
    case 'p': // EC configuration
      ec_param = optarg;
      break;
    case 'd': // database location
      db_location = optarg;
    case 'v': // verbose
      verbose = 1;
      break;
    case ':':
      print_missing(lch);
      break;
    case '?':
    case 'h':
    default:
      usage(argv[0]);
      ret = CQ_ERROR;
      break;
    }
    lch = ch;
  }

  if (NULL == ec_param || NULL == db_location)
    ret = CQ_ERROR;

  return ret;
}

static unsigned int get_command() {
  char * line = readline("Enter command: ");
  unsigned int command = CQ_OK;

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
  if (CQ_OK != parse_options(argc, argv)) {
    usage(argv[0]);
    return 1;
  }

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
