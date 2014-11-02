#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "db.h"
#include "dbg.h"
#include "ccsv.h"
//#include "strecc.h"
#include "logging.h"
#include "administration.h"
#include "init.h"
#include "ui.h"

struct User *user;

//Login user with corresponding login
int login(char *userlogin)
{
  check(user == NULL, "Kan inte logga in. En användare är redan inloggad.");

  user = getUser(userlogin);
  check(user != NULL, "Lyckades inte logga in.");
  setUi("top", "Välkommen %s!", user->name);
  setUi("bottom", "Skanna vara eller funktion...");

  return 0;

 error:
  return -1;
}

int logout()
{
  check(user != NULL, "Kan inte logga ut. Ingen användare är inloggad.");
  setUi("top", "Hejdå %s!", user->name);
  setUi("bottom", "Logga in...");

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

  setUi("top", "%s för %dKr. Saldo: %d", item->name, item->price, user->balance);

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
  int rc = 0;

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

 error:
  return -1;
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

  setUi("top", "strecc startat!");
  setUi("bottom", "Logga in...");
  refreshUi();

  while(1){
    fgets(barcode, MAX_DATA, stdin);

    //Remove newline
    barcodeLength = strlen(barcode) - 1;
    if(barcode[barcodeLength] == '\n') barcode[barcodeLength] = '\0'; 

    if(user == NULL){
      if(isUser(barcode) != -1) login(barcode);
    }

    else if(isUser(barcode) != -1) logout();

    else if(isAdminfunc(barcode) != -1){
      adminFuncNum = find(administrationfile, barcode, administrationIdColumn);
      str = getCell(administrationfile, adminFuncNum, administrationFunctionColumn);
      rc = administrate(str);
      free(str);
    }

    else if(isItem(barcode) != -1) purchase(barcode);

    else setUi("top", "Felaktig input.");

    refreshUi();
  }

 error:
  return -1;
}




