// C-Shell Project
// By Edward Yuen | edwardyuensf@gmail.com
// Finished April 2019

// C-Shell that supports redirections, piping, and executing background processes
// First parses the input entered in command line
// Then runs the commands using fork/exec depending on input
// Opens/Closes/Dups file descriptors depending on commands and input

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 512
#define MAX_TOKEN_SIZE 32
#define SPACE_DELIMITER " "
#define PIPE_DELIMITER "|"
#define OUTPUT_REDIRECT_DELIMITER ">"
#define INPUT_REDIRECT_DELIMITER "<"

int piped;
int func_pipe_fd[2];
int outputRedirectedGlobal;
int inputRedirectedGlobal;
char *outputRedirectedFileName;
char *inputRedirectedFileName;
int noPrompt;

void display_prompt(){
  printf("shell: ");
}

void func(int sig){
  wait(NULL);
}

int count_chars(const char* string, char ch){
  int count = 0;
  int i;
  int length = strlen(string);

  for (i = 0; i < length; i++){
    if (string[i] == ch)
      count++;
  }

  return count;
}

char **tokenize(char *input_arr, int *position, char* delim){
  char **tokens = malloc(MAX_TOKEN_SIZE*sizeof(char*));
  char *token = strtok(input_arr, delim);
  int token_length = 0;
  
  while(token != NULL){
    tokens[token_length] = token;
    token_length++;
    token = strtok(NULL, delim);
  }
  *position = token_length;

  free(token);
  return tokens;
}

int forkAndExecute(int in, int out, char **command, int runInBackground, int outputRedirected, int inputRedirected){
  pid_t pid;
  int status;

  pid = fork();
  if (pid != 0){
    if (piped == 0 && runInBackground == 0)
      wait(&status);
    else if(piped == 1 && runInBackground == 0){

    }else if(piped == 1 && runInBackground == 1){

    }else{
      waitpid(-1, &status, WNOHANG);
    }
  }else{
    //if current fork&exec's input coming from a pipe, change child's stdin child descriptor to the pipe'd input
    if (in != 0){
      dup2(in, 0);
      close(in);
    }
    //if current fork&exec's output is to a pipe, change child's stdout child descriptor to the pipe'd output
    if (out != 1){
      dup2(out, 1);
      close(out);
    }

    if (outputRedirected == 1){
      int fd = open(outputRedirectedFileName, O_RDWR | O_CREAT, 0666);
      dup2(fd, 1);
      close(fd);
    }

    if (inputRedirected == 1){
      int fd1 = open(inputRedirectedFileName, O_RDONLY, 0666);
      dup2(fd1, 0);
      close(fd1);
    }

    if (execvp(command[0], command) == -1){
      perror("ERROR: ");
    }
  }

  return pid;
}

int fork_pipes(int n, char **commands, int runInBackground){
  int in;
  int status;
  int numTokens = 0, *position = &numTokens;
  in = 0;

  for (int i = 0; i < n; i++){
    pipe(func_pipe_fd);
    char **tokenizedCommands = tokenize(commands[i], position, SPACE_DELIMITER);
    if (i == n-1){
      if (outputRedirectedGlobal == 1){
        forkAndExecute(in, 1, tokenizedCommands, runInBackground, 1, 0);
      }else{
        forkAndExecute(in, 1, tokenizedCommands, runInBackground, 0, 0);
      }
      close(func_pipe_fd[0]);
      close(func_pipe_fd[1]);
    } else if(i == 0){
        if (inputRedirectedGlobal == 1){
          forkAndExecute(in, func_pipe_fd[1], tokenizedCommands, runInBackground, 1, 0);
        }else{
          forkAndExecute(in, func_pipe_fd[1], tokenizedCommands, runInBackground, 0, 0);
        }
        close(func_pipe_fd[1]);
    }
    else {
      forkAndExecute(in, func_pipe_fd[1], tokenizedCommands, runInBackground, 0, 0);
      close(func_pipe_fd[1]);
      close(in);
    }
    in = func_pipe_fd[0];
  }

  if(runInBackground){
    return 0;
  }

  for (int i = 0; i < n; i++){
    if (runInBackground){
      waitpid(-1, &status, WNOHANG);
    }else{
      wait(&status);
    }
  }
  
  return 0;
}

int main(int argc, char** argv){
  char input_arr[MAX_INPUT_SIZE];
  char tmp_input_arr[MAX_INPUT_SIZE];
  char tmp_input_arr2[MAX_INPUT_SIZE];

  if (argc == 2)
    noPrompt = 1;
  else{
    noPrompt = 0;
    }

  char *code;
  int status;
  
  while(1){
    waitpid(-1, &status, WNOHANG);
    int numTokens = 0, *position = &numTokens;
    int runInBackground;
    runInBackground = 0;

    if (noPrompt == 0){
      display_prompt();
    }

    code = fgets(input_arr, MAX_INPUT_SIZE, stdin);
    if ((count_chars(input_arr, '&') > 1) || (count_chars(input_arr, '>') > 1) || count_chars(input_arr, '<') > 1){
      printf("ERROR: Multiple &, >, or < characters\n");
      continue;
    }
    if (strstr(input_arr, "||"))
      continue;

    if (code == NULL){
      printf("\n");
      return 0;
    }

    // If the input is "enter key", restart the shell
    if (!strcmp("\n", input_arr))
      continue;
    
    // Strips "\n" character to pass into exec()
    input_arr[strcspn(input_arr, "\n")] = 0;
    if (strstr(input_arr, "&")){
      input_arr[strcspn(input_arr, "&")] = 0;
      runInBackground = 1;
    }
    else
    {
      runInBackground = 0;
    }
    
    strcpy(tmp_input_arr, input_arr);
    strcpy(tmp_input_arr2, input_arr);

    
        
    char **outputRedirectTokens;
    if (strstr(tmp_input_arr, ">")){
      input_arr[strcspn(input_arr, ">")] = 0;
      tmp_input_arr2[strcspn(tmp_input_arr2, ">")] = 0;
      outputRedirectTokens = tokenize(tmp_input_arr, position, OUTPUT_REDIRECT_DELIMITER);
      numTokens = 0;
      char **tempFilename = tokenize(outputRedirectTokens[1], position, SPACE_DELIMITER);
      outputRedirectedFileName = tempFilename[0];
      outputRedirectedGlobal = 1;
      numTokens = 0;
    }else{
      outputRedirectedGlobal = 0;
    }

    char **tempPipedTokens;

    if (strstr(tmp_input_arr2, "<")){
      if(strstr(tmp_input_arr2, "|")){
        tmp_input_arr2[strcspn(tmp_input_arr2, "|")] = 0;
      }
      tempPipedTokens = tokenize(tmp_input_arr2, position, INPUT_REDIRECT_DELIMITER);
      inputRedirectedFileName = tokenize(tempPipedTokens[numTokens-1], position, SPACE_DELIMITER)[0];
      inputRedirectedGlobal = 1;
      numTokens = 0;

    }else{
      inputRedirectedGlobal = 0;
    }
    
    char **tokens;
    if(strstr(input_arr, PIPE_DELIMITER)){
      tokens = tokenize(input_arr, position, PIPE_DELIMITER);
      piped = 1;
      tokens[0][strcspn(tokens[0], "<")] = 0;
    }
    else{
      if (inputRedirectedGlobal == 1)
        input_arr[strcspn(input_arr, "<")] = 0;
      tokens = tokenize(input_arr, position, SPACE_DELIMITER);
      piped = 0;
    }

    if (piped)
      fork_pipes(numTokens, tokens, runInBackground);
    else{
      if (outputRedirectedGlobal == 1 && inputRedirectedGlobal == 1)
        forkAndExecute(0, 1, tokens, runInBackground, 1, 1);
      else if(outputRedirectedGlobal == 1 && inputRedirectedGlobal == 0)
        forkAndExecute(0, 1, tokens, runInBackground, 1, 0);
      else if(outputRedirectedGlobal == 0 && inputRedirectedGlobal == 1)
        forkAndExecute(0, 1, tokens, runInBackground, 0, 1);
      else
        forkAndExecute(0, 1, tokens, runInBackground, 0, 0);
    }
    
  }
  return 0;
}
