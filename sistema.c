#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

void setNonCanonicalMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);  // Obtém a configuração atual do terminal
    t.c_lflag &= ~ICANON;         // Desativa o modo canônico
    t.c_lflag &= ~ECHO;           // Desativa a exibição do caractere digitado
    tcsetattr(STDIN_FILENO, TCSANOW, &t); // Aplica a nova configuração imediatamente
}

void resetTerminalMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);    // Obtém a configuração atual do terminal
    t.c_lflag |= ICANON;            // Ativa o modo canônico
    t.c_lflag |= ECHO;              // Ativa a exibição do caractere digitado
    tcsetattr(STDIN_FILENO, TCSANOW, &t); // Aplica a nova configuração imediatamente
}

unsigned int strToInt(const char *str){
  unsigned int hash = 0;
  while (*str) {
    hash = (hash << 5) + hash + (unsigned char)(*str);
    str++;
  }
  return hash;
}

char CountId(FILE *file){
  char line[MAX_LINE_LENGTH];
  char *last_occurrence = NULL;

  while (fgets(line, sizeof(line), file)) {
    if(strstr(line, "id:")) {
      free(last_occurrence);
      last_occurrence = strdup(line);
    }
  }
  return last_occurrence[3];
  fclose(file);
}

void PrintTitle(){
  char* text = " _____     _           _    _____         _             ";

  struct winsize ws;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  int term_width = ws.ws_col;

  int text_width = strlen(text);
  int padding = (term_width - text_width) / 2;

  if(padding < 0){
    padding = 0;
  }

  for (int i = 0; i < padding; i++) {
    putchar(' ');
  }

  printf("%s\n", text);

  for (int i = 0; i < padding; i++) {
    putchar(' ');
  }

  printf("|   __|___| |_ ___ ___| |  |   __|_ _ ___| |_ ___ _____ \n");

  for (int i = 0; i < padding; i++) {
    putchar(' ');
  }

  printf("|__   |  _|   | . | . | |  |__   | | |_ -|  _| -_|     |\n");

  for (int i = 0; i < padding; i++) {
    putchar(' ');
  }

  printf("|_____|___|_|_|___|___|_|  |_____|_  |___|_| |___|_|_|_|\n");

  for (int i = 0; i < padding; i++) {
    putchar(' ');
  }

  printf("                                 |___|                  \n");
}

int Selection(char *strings[], int count){
  setNonCanonicalMode();

  int selec = 0, selec_max = count;
  char key, seq[3];

  while (1) {
    
    system("clear");
    PrintTitle();

    for (int i = 0; i < count; i++) {
      if(i == selec){
        printf(">");
      }
      printf("%s\n", strings[i]);
    }

    key = getchar();

    if(key == '\033'){
      seq[0] = key;
      seq[1] = getchar(); // [
      seq[2] = getchar(); // A ou B
      
      if(seq[1] == '['){
        if(seq[2] == 'A'){
          if(selec == 0){
            selec = selec_max -1;
          }else {
            selec--;
          }
        }else if(seq[2] == 'B'){
          if(selec == selec_max - 1){
            selec = 0;
          }else{
            selec++;
          }
        }
      }
    }else if(key == '\n' || key == '\r'){
      resetTerminalMode();
      return selec;
    }
  }
}

void SignIn(){


  system("clear");
  PrintTitle();

  char Name[50], Email[50], Password[50];
  int type, id;

  const char *filename = "accounts.json";
  FILE *file;

  file = fopen(filename, "r");

  if(file){
    char charId;
    int id;

    charId = CountId(file);
    id = charId - '0';
    printf("%c\n", charId);
    printf("%d\n", id);
    printf("Name: \n>");
    scanf("%s", Name);

  }else{
    id = 0;

    printf("Name: \n>");
    scanf("%s", Name);

    printf("Email: \n>");
    scanf("%s", Email);

    printf("Password: \n>");
    scanf("%s", Password);

    char jsonData[250];
    snprintf(jsonData, sizeof(jsonData), "id:%d{\nName:%s;\nEmail:%s;\nPassword:%s;\n};\n", id, Name, Email, Password);
    file = fopen(filename, "w");
    fprintf(file, "%s", jsonData);
    fclose(file);
  }

}

struct Logged {
  int isLogged, type, id;
  char Name[30], Email[30], Password[30];
};

int main(){

  struct Logged logged;
  logged.isLogged = 0;
  int option;

  while (1) {
    if (logged.isLogged) {
      printf("Logged!\n");
    } else {
      char *options[] = {"LogIn", "SignIn", "Exit"};
      int count = sizeof(options) / sizeof(options[0]);
      option = Selection(options, count);

      if(option == 0){ //LogIn
        break;
      }else if(option == 1){ //SignIn
        SignIn();
      }else{ //Exit
        break;
      }
    }
  }
  
  return 0;
}
