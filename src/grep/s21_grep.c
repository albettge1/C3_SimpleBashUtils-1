#include "s21_grep.h"

#include <getopt.h>
#include <unistd.h>
#define no_arg 0

void open_command(int argc, char *argv[]);
void make_flags(int argc, char *argv[], flag *fg);
char *read_pattern(int argc, char **argv);
int parse_arguments(int argc, char **argv);
void get_file_name(FILE *file, regex_t *preg, flag *grep_flags, char *filename,
                   int file_count);

int main(int argc, char *argv[]) { open_command(argc, argv); }

void open_command(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr,
            "usage: grep [-abcdDEFGHhIiJLlMmnOopqRSsUVvwXxZz] [-A num] [-B "
            "num] [-C[num]]\n[-e pattern] [-f file] [--binary-files=value] "
            "[--color=when]\n[--context[=num]] [--directories=action] "
            "[--label] [--line-buffered]\n[--null] [pattern] [file ...]");
  } else {
    flag grep_flags = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char *var = argv[1];
    char *var1 = "";
    int a = REG_EXTENDED;
    for (int i = 1; argv[i][0] == '-'; i++) {
      var = argv[i + 1];
    }
    regex_t preg;
    int file_count = parse_arguments(argc, argv);
    var1 = read_pattern(argc, argv);
    make_flags(argc, argv, &grep_flags);
    if (grep_flags.i) a |= REG_ICASE;
    a |= REG_NEWLINE;
    if (grep_flags.e && grep_flags.f) {
      a |= REG_EXTENDED;
      var = var1;
    }
    if (regcomp(&preg, var, a)) {
      fprintf(stderr, "Failed to compile regex\n");
      free(var1);
      return;
    } else {
      for (int i = 1; i < argc; i++) {
        if (!access(argv[i], F_OK)) {
          FILE *file = fopen(argv[i], "r");
          if (file) {
            if (!(strcmp(argv[i - 1], "-e") == 0 ||
                  strcmp(argv[i - 1], "-f") == 0)) {
              get_file_name(file, &preg, &grep_flags, argv[i], file_count);
            }
            fclose(file);
          } else {
            if (argv[i][0] != '-' &&
                !(argv[i] == var || strcmp(argv[i - 1], "-e") == 0 ||
                  strcmp(argv[i - 1], "-f") == 0) &&
                !grep_flags.s)
              fprintf(stderr, "grep: %s: No such file or directory\n", argv[i]);
          }
        }
      }
      regfree(&preg);
    }
    free(var1);
  }
}

char *read_pattern(int argc, char **argv) {
  int append_flag = 0;
  char *var = malloc(4096);
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i - 1], "-e") == 0) {
      if (append_flag == 0) {
        strcpy(var, argv[i]);
        append_flag = 1;
      } else {
        strcat(var, "|");
        strcat(var, argv[i]);
      }
    }
  }
  for (int i = 1; i < argc; i++) {
    FILE *file = NULL;
    if (strcmp(argv[i - 1], "-f") == 0) {
      file = fopen(argv[i], "rb");
      if (!file) {
        fprintf(stderr, "no such file or directory");
      } else {
        size_t buffer_size;
        char *str = NULL;
        while (getline(&str, &buffer_size, file) > 0) {
          if (str[strlen(str) - 1] == '\n') str[strlen(str) - 1] = '\0';
          if (append_flag == 0) {
            strcpy(var, str);
            append_flag = 1;
          } else {
            strcat(var, "|");
            strcat(var, str);
          }
        }
        fclose(file);
        free(str);
      }
    }
  }
  return var;
}

void make_flags(int argc, char *argv[], flag *fg) {
  const char *short_options = "+eviclnhsf:o";
  int result = 0;
  while ((result = getopt_long(argc, argv, short_options, no_arg, NULL)) !=
         -1) {
    switch (result) {
      case 'e':
        fg->e = 1;
        break;
      case 'v':
        fg->v = 1;
        break;
      case 'i':
        fg->i = 1;
        break;
      case 'c':
        fg->c = 1;
        break;
      case 'l':
        fg->l = 1;
        break;
      case 'n':
        fg->n = 1;
        break;
      case 'h':
        fg->h = 1;
        break;
      case 's':
        fg->s = 1;
        break;
      case 'f':
        fg->f = 1;
        break;
      case 'o':
        fg->o = 1;
        break;
      default:
        break;
    }
  }
}
int parse_arguments(int argc, char **argv) {
  int file_count = 0;
  int flag = 1;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i - 1], "-e") == 0) flag = 0;
    if (argv[i][0] != '-' && strcmp(argv[i - 1], "-e")) file_count++;
  }
  return file_count - flag;
}
void get_file_name(FILE *file, regex_t *preg, flag *grep_flags, char *filename,
                   int file_count) {
  char *str = NULL;
  size_t blabla;
  regmatch_t match;
  int count = 0;
  int i = 1;
  while (getline(&str, &blabla, file) > 0) {
    if (strchr(str, '\n') == NULL) strcat(str, "\n");
    if ((!regexec(preg, str, 1, &match, 0) && !grep_flags->v) ||
        (regexec(preg, str, 1, &match, 0) && grep_flags->v)) {
      if (grep_flags->o) {
        printf("%.*s\n", (int)(match.rm_eo - match.rm_so), str + match.rm_so);
      } else {
        if (!grep_flags->c && !grep_flags->l) {
          if (file_count > 1 && !grep_flags->h) printf("%s:", filename);
          if (grep_flags->n) printf("%d:", i);
          printf("%s", str);
        }
      }
      count++;
    }
    i++;
  }
  if (grep_flags->l && count > 0) printf("%s\n", filename);
  if (grep_flags->c) {
    if (file_count > 1) {
      printf("%s:", filename);
    }
    printf("%d\n", count);
  }
  free(str);
}