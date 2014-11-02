#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "db.h"
#include "ui.h"
#include "dbg.h"


struct Ui *ui;

//Initialize the ui
int initUi()
{
  ui = malloc(sizeof(struct Ui));
  check(ui != NULL, "Lyckades inte allokera minne för UI.");
  
  ui->top = NULL;
  ui->toplength = 0;
  ui->middle = NULL;
  ui->middlelength = 0;
  ui->bottom = NULL;
  ui->bottomlength = 0;

  return 0;

 error:
  return -1;
}

//Set top, middle or bottom message
int setUi(char *location, char *str, ...)
{
  va_list arg;
  int done;

  if(strncmp(location, "top", MAX_DATA) == 0){
    if(ui->top){
      free(ui->top);
      ui->top = NULL;
    }
    
    ui->top = malloc(sizeof(char) * MAX_DATA);
    va_start(arg, str);
    done = vsnprintf(ui->top, MAX_DATA, str, arg);

    ui->toplength = stringRows(ui->top);
  }

  else if(strncmp(location, "bottom", MAX_DATA) == 0){
    if(ui->bottom){
      if(ui->bottom) free(ui->bottom);
      ui->bottom = NULL;
    }
    
    ui->bottom = malloc(sizeof(char) * MAX_DATA);
    va_start(arg, str);
    done = vsnprintf(ui->bottom, MAX_DATA, str, arg);

    ui->bottomlength = stringRows(ui->bottom);
  }

  return done;
}

int stringRows(char *str)
{
  int rows = 1;
  int columns = 0;
  int i = 0;

  for(i = 0;i < MAX_DATA && str[i] != '\0';i++){
    if(str[i] == '\n'){
      rows++;
      columns = 0;
    }
    else{
      columns++;
      if(columns > terminalColumns){
        rows++; 
        columns = 0;
      }
    }
  }
  return rows;
}

//Refresh the user interface
int refreshUi()
{
  int i = 0;
  int rows = terminalRows - (ui->toplength + ui->bottomlength);
  printf("%s", ui->top);
  
  for(i = 0;i < rows;i++) printf("\n");

  printf("%s\n", ui->bottom); 
}
