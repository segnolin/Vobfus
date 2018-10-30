#include <stdio.h>

long long sdbm(char s[])
{
  long long hash = 0;
  int i = 0;
  while (s[i] != '\x00')
  {
    hash = s[i] + (hash << 6) + (hash << 16) - hash;
    i++;
  }
  return hash;
}

int main(void)
{
  char s[] = "vobfus";

  printf("sdbm: %s --> %lld\n", s, sdbm(s));

  return 0;
}
