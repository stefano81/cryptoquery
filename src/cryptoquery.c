#include "cryptoquery.h"
#include <unistd.h>
#include <string.h>

unsigned int verbose;

char * ec_param;

void usage() {
  fprintf(stderr, "Usage:");
}

int parse_options(int argc, char *argv[]) {
  char * opts = "f:p:";
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
      usage();
      ret = CQ_ERROR;
      break;
    }
  }

  return ret;
}

unsigned int get_command() {
  return CQ_QUIT;
}

void quit() {
  close_database();
  printf("Bye!\n");
}

int main(int argc, char *argv[]) {
  if (CQ_OK != parse_options(argc, argv))
    return 1;

  int command;
  do {
    command = get_command();
    switch (command) {
    case CQ_ADD:
      //add_query();
      break;
    case CQ_SELECT:
      //select_query();
      break;
    case CQ_QUIT:
      quit();
    default:
      fprintf(stderr, "Unkown command\n");
      break;
    }
  } while (CQ_QUIT != command);


  return 0;
}
