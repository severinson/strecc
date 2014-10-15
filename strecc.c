#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "db.h"
#include "dbg.h"
#include "ccsv.h"
#include "strecc.h"
#include "logging.h"
#include "administration.h"

//Initialize the strecc system
int initialize()
{
  int rc = 0;

  //Check that all required files are present and use correct CSV syntax
  rc = isValidCsv(itemfile);
  check(rc == 0, "Produktfilen items.csv finns inte eller har felaktig syntax.");
  rc = getColumns(itemfile);
  check(rc == itemFileColumns, "Produktfilen har felaktig syntax.")
  printf("items.csv ser helt ok ut!\n");

  rc = isValidCsv(userfile);
  check(rc == 0, "Användarfilen users.csv finns inte eller har felaktig syntax.");
  rc = getColumns(userfile);
  check(rc == userFileColumns, "Användarenfilen har felaktig syntax.");
  printf("users.csv ser helt ok ut!\n");

  rc = isValidCsv(transactionfile);
  check(rc == 0, "Transaktionsfilen transactions.csv finns inte eller har felaktig syntax.");
  rc = getColumns(transactionfile);
  check(rc == transactionFileColumns, "Transaktionsfilen har felaktig syntax,");
  printf("transactions.csv ser helt ok ut!\n");

  rc = isValidCsv(logfile);
  check(rc == 0, "Logfilen strecclog.csv finns inte eller har felaktig syntax.");
  rc = getColumns(logfile);
  check(rc == logFileColumns, "Logfilen har felaktig syntax.");
  printf("strecclog.csv ser inte helt ok ut!\n");

  return 0;

 error:
  return -1;
}

//Wait for a user to login
struct User *login()
{
  //Print log-in message
  clearTerminal(12);
  printf("Logga in...\n");

  //Wait for user input
  char barcode[MAX_DATA] = "";
  fgets(barcode, MAX_DATA, stdin);
  
  int barcodeLength = 0;

  //Remove newline
  barcodeLength = strlen(barcode) - 1;
  if(barcode[barcodeLength] == '\n') barcode[barcodeLength] = '\0';

  //Attempt to find user in database. If user is found, continue.
  //Otherwise try again.
  return getUser(barcode);
}

//Collects items the user is buying
int purchase(struct User *user, char *barcode)
{
  struct Item *item = NULL;

  int rc = 0;

  //Get item with barcode
  item = getItem(barcode);
  check(item != NULL, "Kunde inte hitta en vara med det ID.");

  if(user->balance - item->price < -100 && item->price > 0){
    printf("Ditt saldo är %dkr. Max kredit 100kr. Stoppa i mer pengar.\n", user->balance);
    return -1;    
  }

  //Log transaction
  rc = addTransaction(user, item);
  check(rc != -1, "Kunde inte logga transaktionen.");
  
  //Adjust user balance
  rc = updateUserBalance(user, item);
  check(rc != -1, "Kunde inte uppdatera ditt saldo.");

  printf("%s för %dkr\n", item->name, item->price);
  printf("Saldo: %dkr\n", user->balance);

  free(item->name);
  free(item->barcode);
  free(item);
  return 0;

 error:
  if(item) free(item);
  return -1;
}

//Check which administrative function that was scanned and perform it.
int administrate(char *str, struct User *user)
{
  int rc = 0;

  if(strncmp(str, "adduser", MAX_DATA) == 0) rc = addUser(user);
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

//Cleanup user
int cleanupUser(struct User *user)
{
  if(user->userId) free(user->userId);
  if(user->name) free(user->name);
  if(user->barcode) free(user->barcode);
  if(user) free(user);
  user = NULL;  
  return 0;
}

int main(int argc, char *argv[])
{
  struct User *user = NULL;
  char barcode[MAX_DATA] = "";
  int barcodeLength = 0;
  int rc = 0;

  int adminFuncNum = 0;
  char *str = "";

  rc = initialize();
  check(rc != -1, "Kunde inte starta systemet. Se felmeddelande.");
  printf("strecc v1.1 startat!\n");

  rc = info("[STARTUP]");
  check(rc != -1, "Loggning funkar inte.");

  while(1){
    //Wait for login
    while(user == NULL) user = login();
  loggedIn:
    rc = clearTerminal(11);
    printf("Välkommen %s! Ditt saldo är %dkr.\n", user->name, user->balance);
    clearTerminal(1);
    info("[LOGIN]:%s (%s)", user->name, user->userId);


    while(1){
    waitForBarcode:
      clearTerminal(11);
      printf("Skanna vara eller funktion...\n");

      //Get barcode
      fgets(barcode, MAX_DATA, stdin);
  
      //Remove newline
      barcodeLength = strlen(barcode) - 1;
      if(barcode[barcodeLength] == '\n') barcode[barcodeLength] = '\0'; 

      //Check if the barcode is an administrative function
      adminFuncNum = find(administrationfile, barcode, administrationIdColumn);
      if(adminFuncNum != -1){
        //Perform administrative function
        str = getCell(administrationfile, adminFuncNum, administrationFunctionColumn);
        if(strncmp(str, "logout", MAX_DATA) == 0){
          free(str);
          printf("Hejdå %s!\n", user->name);
          info("[LOGOUT]:%s (%s)", user->name, user->userId);
          goto cleanup;
        }

        else if(strncmp(str, "shutdown", MAX_DATA) == 0){
          free(str);
          printf("Hejdå %s!\n", user->name);
          goto end;
        }

        else{
          rc = administrate(str, user);
          free(str);
          goto waitForBarcode;
        }          
      }
      
      //If not, check if it's an item
      else if(find(itemfile, barcode, itemBarcodeColumn) != -1){
        //Make a purchase
        rc = purchase(user, barcode);
      }

      //If not check if it's a user. Log out if it is.
      else if(find(userfile, barcode, userBarcodeColumn) != -1){
        printf("Hejdå %s!\n", user->name);
        info("[LOGOUT]:%s (%s)", user->name, user->userId);
        goto cleanup;
      }
      
      //Otherwise print an error message
      else printf("Finns ingen vara eller funktion med det ID.\n");
    }

  cleanup:
    //Cleanup user
    rc = cleanupUser(user);
    user = NULL;
    
  }
  
 end:
  //Cleanup user
  rc = cleanupUser(user);
  user = NULL;

  printf("strecc avslutat.\n");
  info("[SHUTDOWN]");
  return 0;

 error:
  return -1;
}

/*
- Undo ger inte tillbaks pengarna.
- Man borde inte få ha samma login som en varas streckkod eller som en
- adminfunktion.
- Man borde inte få lägga till en ny vara med en streckkod som hör till en användare eller adminfunktion.

- Kunna byta loginId
- Inga köp om under -100 i saldo.
- Undo tar samma användares transaktion
- Kunna lägga in en fri summa. Istället för att strecka en hundring t.ex.
- Random info på skärmen. Köra en till fil med statistik.
- Om man skannar en produkt när den inte är inloggad, visa info för produkten. Pris, statistik osv.
- Töm termineln vid logout.
*/
