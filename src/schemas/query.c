#include "query.h"

setup_output * setup(int l, char * path_to_param) {
  char param[1024];
  FILE * file = fopen(path_to_param, "r");

  printf("init setup");

  size_t count = fread(param, 1, 1024, file);

  if (!count)
    pbc_die("Input error!");

  setup_output * output = malloc(sizeof(setup_output));

  group_info * info = malloc(sizeof(group_info));

  pairing_init_set_buf(info->pairing, param, count);
  element_init_G1(info->g, info->pairing);
  element_random(info->g);

  output->info = info;

  msk * secret = malloc(sizeof(msk));
  mpk * public = malloc(sizeof(mpk));

  int i, x;

  secret->t = malloc(l * sizeof(element_t *));
  public->T = malloc(l * sizeof(element_t *));

  secret->l = public->l = l;

  for (i = 0; i < l; i++) {
    printf(".");

    secret->t[i] = malloc(255 * sizeof(element_t));
    public->T[i] = malloc(255 * sizeof(element_t));

    for (x = 0; x < 255; x++) {
      // compute t_{i,x}
      element_init_Zr(secret->t[i][x], info->pairing);
      element_random(secret->t[i][x]);

      // compute T_{i, x}
      element_init_G1(public->T[i][x], info->pairing);
      element_pow_zn(public->T[i][x], info->g, secret->t[i][x]);
    }
  }

  element_init_GT(public->omega, info->pairing);
  pairing_apply(public->omega, info->g, info->g, info->pairing);

  printf("done!\n");

  output->msk = secret;
  output->mpk = public;

  return output;
}

cipher * enc(message * m, unsigned char *x, group_info * info, mpk * public) {
  int i;
  short b = 0;
  printf("encrypting with x={");
  for (i = 0; i < public->l; i++) {
    if (b)
      printf(",");
    else
      b = 1;
    printf("%c", x[i]);
  }
  printf("}\n");
  cipher * ct = malloc(sizeof(*ct));

  ct->X = malloc(sizeof(element_t) * public->l);

  element_t s;

  element_init_Zr(s, info->pairing);
  element_random(s);

  printf("computing X_i");
  for (i = 0; i < public->l; i++) {
    printf(".");
    element_init_G1(ct->X[i], info->pairing);
    element_pow_zn(ct->X[i], public->T[i][x[i]], s); // X_i = T_{i, x_i}^s
    printf("%d %c", i, x[i]);
  }
  printf("done\n");

  element_init_same_as(ct->M, public->omega);

  element_init_same_as(m->m, public->omega);
  element_random(m->m);

  printf("computing M");

  element_t os;
  element_init_same_as(os, public->omega);
  element_pow_zn(os, public->omega, s);
  element_mul(ct->M, m->m, os);
  printf(".done\n");

  element_clear(os);
  element_clear(s);

  return ct;
}

public_key * keygen(int *y, group_info * info, msk * secret) {
  public_key * pk = malloc(sizeof(public_key));
  pk->Ssize = 0;
  int i;

  for (i = 0; i < secret->l; i++) {
    if (-1 != y[i]) {
      pk->Ssize++;
    }
  }
  pk->tk = malloc(sizeof(element_t) * pk->Ssize);
  pk->S = malloc(sizeof(unsigned int) * pk->Ssize);

  printf("|y[i] != *| = %d\n", pk->Ssize);

  element_t * a = malloc(sizeof(element_t) * pk->Ssize);

  int j;
  element_t tot;
  element_init_Zr(tot, info->pairing);
  element_set0(tot);
  for (i = j = 0; j < (pk->Ssize - 1 ) && i < secret->l; i++) {
    if (-1 != y[i]) {
      pk->S[j] = i;
      element_init_Zr(a[j], info->pairing);
      element_random(a[j]);
      element_add(tot, tot, a[j]);
      j++;
    }
  }

  pk->S[j] = i;  
  element_t t;
  element_init_Zr(t, info->pairing);
  element_init_Zr(a[j], info->pairing);
  element_set1(t);
  element_neg(t, t);
  element_sub(a[j], t, tot);

  // check if a[j]s sum up to 1
  element_set0(t); 
  for (j = 0; j < pk->Ssize; j++) { 
    element_add(t, t, a[j]);
  }

  printf("sum up to 1: %s\n", 0 == element_is1(t) ? "true":"false");


  for (j = 0; j < pk->Ssize; j++) {
    //printf("%d/%d\n", j, pk->Ssize);
    element_init_G1(pk->tk[j], info->pairing);
    i = pk->S[j];
    printf("%d %c\n", i, y[i]);
    element_div(t, a[j], secret->t[i][y[i]]); // t=a_i/t_{i, y_i}
    element_pow_zn(pk->tk[j], info->g, t); // g^t
  }

  element_clear(tot);
  element_clear(t);

  for (i = 0; i < pk->Ssize; i++) {
    element_clear(a[i]);
  }

  free(a);

  return pk;
}

message * dec(cipher *ct, public_key *key, group_info *info) {
  int i,j;
  element_t result;
  element_t exiyi;
  element_init_GT(result, info->pairing);
  element_init_GT(exiyi, info->pairing);
  element_set1(result);
  for (j = 0; j < key->Ssize; j++) {
    i = key->S[j];
    printf("%d %d\n", i, j);
    pairing_apply(exiyi, ct->X[i], key->tk[j], info->pairing);
    element_mul(result, result, exiyi);
    printf("%d : result == exiyi -> %s\n", j, 0 == element_cmp(result, exiyi)? "true" : "false");
  }
  element_clear(exiyi);

  message * m = malloc(sizeof(message));
  element_init_GT(m->m, info->pairing);
  element_div(m->m, ct->M, result);

  element_clear(result);

  return m;
}

int check(message *m1, message *m2) {
  int ret = 0;
  if (0 == element_cmp(m1->m, m2->m))
    ret = 1;
  else {
    element_t nm;
    element_init_same_as(nm, m2->m);
    element_neg(nm, m2->m);
    if (0 == element_cmp(m1->m, nm)) {
      ret = 1;
    } else {
      element_invert(nm, m2->m);
      if (0 == element_cmp(m1->m, nm)) {
	ret = 1;
      }
    }

    element_clear(nm);
  }

  return ret;
}
