#include "cryptoquery.h"
#include <unistd.h>

unsigned int verbose;
unsigned int dbtype;

void usage() {
  fprintf(stderr, "Usage:");
}

unsigned int parse_options(int argc, char *argv[]) {
  char * opts = "e:vh";
  unsigned int ret = CQ_OK;
  int ch;
  while (-1 != ((ch = getopt(argc, argv, opts)))) {
    switch (ch) {
    case 'e':
      set_encryption_type(optarg);
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

void add_query() {

}

void select_query() {

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
      add_query();
      break;
    case CQ_SELECT:
      select_query();
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
