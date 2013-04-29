#include "query.h"

int main(int argc, char ** argv) {
  unsigned char c = -1;

  printf("%d\n", c);

  const unsigned int l = 6;

  setup_output * gp = setup(l, argv[1]);//"a.param");

  unsigned char x[] = {'1', '2', '3', '4', '5', '6'};

  printf("sizeof x%d\n", sizeof(x)/sizeof(*x));

  message m1;
  
  cipher * ct = enc(&m1, x, gp->info, gp->mpk);

  int y[] = {'1', '2', '3', '4', '5', '6'};
  public_key * key = keygen(y, gp->info, gp->msk);

  message *m2 = dec(ct, key, gp->info);

  printf("Result: %s\n", check(&m1, m2) ? "true" : "false");

  return 0;
}
