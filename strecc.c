#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "db.h"
#include "dbg.h"
#include "ccsv.h"
#include "strecc.h"
#include "logging.h"
#include "administration.h"
#include "init.h"
#include "ui.h"

struct User *user;
int logout_clock;


//Login user with corresponding login
int login(char *userlogin)
{
  check(user == NULL, "Kan inte logga in. En användare är redan inloggad.");

  user = getUser(userlogin);
  check(user != NULL, "Lyckades inte logga in.");
  setUi("top", "Välkommen %s!\nSaldo: %dKr", user->name, user->balance);
  setUi("bottom", "Skanna vara eller funktion...");
  logout_clock = 10;

  return 0;

 error:
  return -1;
}

int logout()
{
  check(user != NULL, "Kan inte logga ut. Ingen användare är inloggad.");
  setUi("top", "Hejdå %s!", user->name);
  setUi("bottom", "Logga in...");
  refreshUi();

  if(user->userId) free(user->userId);
  if(user->name) free(user->name);
  if(user->barcode) free(user->barcode);
  if(user) free(user);
  user = NULL;

  return 0;

error:
return -1;
}

int purchase(char *barcode)
{
  int rc = 0;
  struct Item *item = NULL;

  //get item
  item = getItem(barcode);
  check(item != NULL, "Kunde inte hitta varan.");

  //Check that user has sufficient balance
  if(user->balance - item->price < -50 && item->price > 0){
    setUi("top", "Ditt saldo är %dkr. Max kredit 50kr. Stoppa i mer pengar.\n", user->balance);
    goto error;  
  }

  //Adjust user balance
  rc = updateUserBalance(user, item);
  check(rc != -1, "Kunde inte uppdatera ditt saldo.");

  //Log transaction
  rc = addTransaction(user, item);
  check(rc != -1, "Kunde inte logga transaktionen.");

  setUi("top", "%s för %dKr\nSaldo: %dKr", item->name, item->price, user->balance);
  setUi("bottom", "Skanna vara eller funktion...");

  //Cleanup
  free(item->name);
  free(item->barcode);
  free(item);
  return 0;

 error:
  if(item->name) free(item->name);
  if(item->barcode) free(item->barcode);
  if(item) free(item);
  return -1;
}

int displayItem(char *barcode)
{
  struct Item *item = NULL;

  //Get item from db
  item = getItem(barcode);
  check(item != NULL, "Kunde inte hitta varan.");

  //Display item details
  setUi("top", "%s för %dKr", item->name, item->price);
  setUi("bottom", "Logga in...");
  
  //Cleanup
  free(item->name);
  free(item->barcode);
  free(item);
  return 0;

 error:
  if(item->name) free(item->name);
  if(item->barcode) free(item->barcode);
  if(item) free(item);
  return -1;
}

//Check which administrative function that was scanned and perform it.
int administrate(char *str)
{

  if(strncmp(str, "logout", MAX_DATA) == 0) rc = logout();
  else if(strncmp(str, "shutdown", MAX_DATA) == 0) exit(0);
  else if(strncmp(str, "adduser", MAX_DATA) == 0) rc = addUser(user);
  else if(strncmp(str, "updatelogin", MAX_DATA) == 0) rc = changeUserLogin(user);
  else if(strncmp(str, "addbalance", MAX_DATA) == 0) rc = addBalance(user);
  else if(strncmp(str, "undotransaction", MAX_DATA) == 0) rc = undoTransaction(user);
  else if(strncmp(str, "additem", MAX_DATA) == 0) rc = addItem(user);
  else if(strncmp(str, "removeitem", MAX_DATA) == 0) printf("Funktionen finns inte ännu.\n");
  else if(strncmp(str, "updateitemprice", MAX_DATA) == 0) printf("Funktionen finns inte ännu.\n");

  return 0;

}

void *logout_timer(void *param);

void *logout_timer(void *param)
{
  while(1){
    while(logout_clock > 0){
      sleep(6);
      logout_clock--;
    }
    if(user) logout();
    while(!user) sleep(1);
  }
}

int main(int argc, char *argv[])
{
  user = NULL;
  char barcode[MAX_DATA] = "";
  char *str;

  int barcodeLength = 0;
  int adminFuncNum = -1;
  int rc = 0;

  //Attempt to initialize the system
  rc = initialize();
  check(rc != -1, "Kunde inte starta strecc. Se felmeddelande.");

  //Start logout timer
  logout_clock = 10;
  pthread_t tid;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(&tid, &attr, logout_timer, "");

  setUi("top", "strecc v1.4.3 startat!");
  setUi("bottom", "Logga in...");
  refreshUi();

  while(1){
    if(user) logout_clock = 10; //Reset logout timer
    fgets(barcode, MAX_DATA, stdin);

    //Remove newline
    barcodeLength = strlen(barcode) - 1;
    if(barcode[barcodeLength] == '\n') barcode[barcodeLength] = '\0'; 

    if(user == NULL){
      if(isUser(barcode) != -1) login(barcode);
      else if(isItem(barcode) != -1) displayItem(barcode);
      else setUi("top", "Ingen användare eller vara med den koden.");
    }

    else if(isUser(barcode) != -1) logout();

    else if(isAdminfunc(barcode) != -1){
      adminFuncNum = find(administrationfile, barcode, administrationIdColumn);
      str = getCell(administrationfile, adminFuncNum, administrationFunctionColumn);
      rc = administrate(str);
      free(str);
    }

    else if(isItem(barcode) != -1) purchase(barcode);

    else setUi("top", "wat?");

    if(user) setUi("bottom", "Skanna vara eller funktion...");
    else setUi("bottom", "Logga in...");
    refreshUi();
  }

 error:
  return -1;
}




