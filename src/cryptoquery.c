#include "cryptoquery.h"
#include <unistd.h>

unsigned int verbose;

char * config_file_path;

void usage() {
  fprintf(stderr, "Usage:");
}

void read_config(const char *config_file_path) {
  FILE * fd = fopen(config_file_path, "r");
  
}

int parse_options(int argc, char *argv[]) {
  char * opts = "f:";
  unsigned int ret = CQ_OK;
  int ch;
  while (-1 != ((ch = getopt(argc, argv, opts)))) {
    switch (ch) {
    case 'f': // configuration file
      read_config(optarg);
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
  verbose = 0;
  
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
