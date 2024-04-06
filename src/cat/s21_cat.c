#include "s21_cat.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

void number(FILE *file, char fp[], flag fg);
void dollar(FILE *file, char fp[]);
void null(FILE *file, char fp[]);
char symbol(char c);
void makeflag(int argc, char *argv[], flag *fg);
void tab(FILE *file, char fp[], flag fg);
void print(FILE *file, char fp[]);

int main(int argc, char *argv[]) {
  FILE *file = NULL;
  flag fg = {0, 0, 0, 0, 0, 0};
  makeflag(argc, argv, &fg);
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      if (fg.b == 1 || fg.n == 1) {
        number(file, argv[i], fg);
      }
      if (fg.s == 1) {
        null(file, argv[i]);
      }
      if (fg.e == 1) {
        dollar(file, argv[i]);
      }
      if (fg.t == 1) {
        tab(file, argv[i], fg);
      }
      if (fg.v == 1) {
        print(file, argv[i]);
      }
    }
  }
}
void print(FILE *file, char fp[]) {
  file = fopen(fp, "r");
  if (!file) {
    fprintf(stderr, "no such file or dir %s\n", fp);
  } else {
    char ch;
    while ((ch = fgetc(file)) != EOF) {
      printf("%c", ch);
    }
    fclose(file);
  }
}
void number(FILE *file, char fp[], flag fg) {
  file = fopen(fp, "r");
  if (!file) {
    fprintf(stderr, "no such file or dir %s\n", fp);
  } else {
    char str[512];
    char *estr;
    int i = 0;
    while ((estr = fgets(str, sizeof(str), file)) != 0) {
      if (fg.b == 1) {
        if (estr[0] != '\n') {
          i++;
          printf("%6d\t", i);
        }
        printf("%s", str);
      } else if (fg.n == 1) {
        i++;
        printf("%6d\t%s", i, str);
      }
    }
    fclose(file);
  }
}
void dollar(FILE *file, char fp[]) {
  file = fopen(fp, "r");
  if (!file) {
    fprintf(stderr, "no such file or dir %s\n", fp);
  } else {
    char ch;
    while ((ch = fgetc(file)) != EOF) {
      if (ch == '\n') {
        printf("$");
      } else
        printf("%c", ch);
    }
    fclose(file);
  }
}
void null(FILE *file, char fp[]) {
  file = fopen(fp, "r");
  if (!file) {
    fprintf(stderr, "no such file or dir %s\n", fp);
  } else {
    char c;
    char prev = '\n';
    while ((c = fgetc(file)) != EOF) {
      int fl = 0;
      while (c == '\n' && prev == '\n') {
        fl = 1;
        prev = c;
        c = fgetc(file);
      }
      if (fl) printf("\n");
      prev = c;
      printf("%c", c);
    }
    fclose(file);
  }
}
void tab(FILE *file, char fp[], flag fg) {
  file = fopen(fp, "r");
  if (!file) {
    fprintf(stderr, "no such file or dir %s\n", fp);
  } else {
    char c;
    while ((c = fgetc(file)) != EOF) {
      if (c == '\t' && fg.t == 1) {
        printf("^I");
      } else
        printf("%c", fg.v == 1 ? symbol(c) : c);
    }
    fclose(file);
  }
}
char symbol(char c) {
  if (c == '\n' || c == '\t')
    ;
  else if (c < 32 && c >= 0) {
    c = c + 64;
    printf("^");
  } else if (c == 127) {
    c = c - 64;
    printf("^");
  }
  return c;
}
void makeflag(int argc, char *argv[], flag *fg) {
  const char *short_opt = "+bentvsET";
  int result;
  const struct option long_opt[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                    {"number", no_argument, NULL, 'n'},
                                    {"squeeze-blank", no_argument, NULL, 's'},
                                    {NULL, 0, NULL, 0}};
  while ((result = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
    switch (result) {
      case 'b':
        fg->b = 1;
        break;
      case 'e':
        fg->e = 1;
        fg->v = 1;
        break;
      case 'n':
        fg->n = 1;
        break;
      case 't':
        fg->t = 1;
        break;
      case 's':
        fg->s = 1;
        break;
      case 'E':
        fg->e = 1;
        break;
      case 'T':
        fg->t = 1;
        break;
      case 'v':
        fg->v = 1;
        break;
    }
    if (fg->b) {
      fg->n = 0;
    }
  }
};