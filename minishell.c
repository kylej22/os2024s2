/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File			: minishell.c
   Compiler/System	: gcc/linux

********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#define NV 20           /* max number of command tokens */
#define NL 100          /* input buffer size */
#define MAX_PATH 0      /* max directory path */
char line[NL];          /* command input buffer */
char lastdir[MAX_PATH]; /* last directory */

/*
  shell prompt
 */

void prompt(void)
{
  // fprintf(stdout, "\n msh> ");
  fflush(stdout);
}

int exec_cd(char *arg)
{
  char curr[MAX_PATH];
  char path[MAX_PATH];

  if (getcwd(curr, sizeof(curr)))
  {
    *curr = '\0';
  }
  if (arg == NULL)
  {
    arg = getenv("HOME");
  }
  if (!strcmp(arg, "-"))
  {
    if (*lastdir == '\0')
    {
      fprintf(stderr, "No previous directory\n");
      return 1;
    }
    arg = lastdir;
  }
  else
  {
    if (*arg == '~')
    {
      if (arg[1] == '/' || arg[1] == '\0')
      {
        snprintf(path, sizeof(path), "%s%s", getenv("HOME"), arg + 1);
        arg = path;
      }
      else
      {
        fprintf(stderr, "syntax not supported: %s\n", arg);
        return 1;
      }
    }
  }
  if (chdir(arg))
  {
    fprintf(stderr, "chdir: %s: %s\n", strerror(errno), path);
    return 1;
  }
  strcpy(lastdir, curr);
  return 0;
}

int main(int argk, char *argv[], char *envp[])
/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */
{
  int frkRtnVal;       /* value returned by fork sys call */
  int wpid;            /* value returned by wait */
  char *v[NV];         /* array of pointers to command line tokens */
  char *sep = " \t\n"; /* command line token separators    */
  int bg_count = 0;               /* count background processes */
  int i;               /* index into command line token array */
  int background;      /* flag to indicate background execution */

  /* prompt for and process one command line at a time  */

  while (1)
  { /* do Forever */
    prompt();
    if (fgets(line, NL, stdin) == NULL)
    {
      if (feof(stdin))
      { /* non-zero on EOF  */
        /*
        fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
                feof(stdin), ferror(stdin));
        */
        exit(0);
      }
      else
      {
        perror("fgets");
        continue;
      }
    }
    fflush(stdin);

    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
      continue; /* to prompt */

    v[0] = strtok(line, sep);
    for (i = 1; i < NV; i++)
    {
      v[i] = strtok(NULL, sep);
      if (v[i] == NULL)
        break;
    }

    /* Check if the command is cd */
    if (strcmp(v[0], "cd") == 0)
    {
      if (exec_cd(v[1]))
      {
        fprintf(stderr, "cd failed\n");
      }
      continue;
    }

    /* Check if the last token is "&" to indicate background execution */
    background = 0;
    if (i > 1 && strcmp(v[i-1], "&") == 0) {
      background = 1;
      v[i-1] = NULL;
    }

    /* fork a child process to exec the command in v[0] */

    switch (frkRtnVal = fork())
    {
    case -1: /* fork returns error to parent process */
      perror("fork");
      break;
    case 0: /* code executed only by child process */
      execvp(v[0], v);
      perror("execvp");
      exit(1);
    default: /* code executed only by parent process */
      if (!background)
      {
        wpid = waitpid(frkRtnVal, NULL, 0);
        if (wpid == -1)
        {
          perror("waitpid");
        }
      }
      else
      {
        /*Increment background process count and display */
        bg_count++;
        fprintf(stdout, "[%d] %d\n", bg_count, frkRtnVal);
        fflush(stdout);
      }
      break;
    } /* switch */
  } /* while */
  return 0;
} /* main */

