#include <stdlib.h>

#include "dbg.h"
#include "db.h"
#include "administration.h"
#include "logging.h"
#include "ccsv.h"

//Will undo the last transaction for user
int undoTransaction(struct User *user)
{
  int i = 0;
  int rc = 0;
  int transactionRow = -1;
  int userRow = -1;
  int price = 0;

  char *priceString = NULL;
  char *itemName = NULL;
  char *balanceString = malloc(MAX_DATA * sizeof(char));

  //Get user row
  userRow = find(userfile, user->name, userNameColumn);
  check(userRow != -1, "Hittade inte användaren i användarfilen.");

  //Get last transaction in log
  printf("Ångra köp.\n");
  while(i != -1){
    i = findNext(transactionfile, user->userId, transactionUserColumn);
    if(i != -1) transactionRow = i;
  }

  if(transactionRow == -1){
    printf("Du har inga fler transaktioner att ångra.\n");
    goto error;
  }
 
  //Get transaction details
  itemName = getCell(transactionfile, transactionRow, transactionItemColumn);
  check(itemName != NULL, "Lyckades inte hämta varans namn.");

  priceString = getCell(transactionfile, transactionRow, transactionItemPriceColumn);
  check(priceString != NULL, "Lyckades inte hämta varans pris.");

  price = atoi(priceString);

  //Delete row in file
  rc = removeRow(transactionfile, transactionRow);
  check(rc != -1, "Lyckades inte ta bort transaktionen i loggen.");

  //Update user balance
  user->balance += price;
  sprintf(balanceString, "%d", user->balance);
  rc = editCell(userfile, userRow, userBalanceColumn, balanceString);
  printf("[%s] ångrat.\n[SALDO]:%d\n", itemName, user->balance);

  //Log
  info("[UNDO]:%s (%s):%s", user->name, user->userId, itemName);
 
  free(itemName);
  free(priceString);
  free(balanceString);

  return 0;

 error:
  if(priceString) free(priceString);
  if(itemName) free(itemName);
  if(balanceString) free(balanceString);
  return -1;
}

//Add a new user
int addUser(struct User *user)
{
  int rc = 0;
  int userId = 0;
  int stringLength = 0;

  char *ch = NULL;
  char *userName = malloc(MAX_DATA * sizeof(char));
  char *userLogin = malloc(MAX_DATA *sizeof(char));
  char *userIdString = malloc(MAX_DATA *sizeof(char));

  //Check that current user is admin
  if(user->isAdmin != 1){
    printf("Bara admin kan lägga till nya användare. Den här händelsen har rapporterats.\n");
    goto error;
  }

  printf("Lägg till ny användare.\n");

  //Collect username
  printf("Ange användarnamn på tangentbordet.\n");
  ch = fgets(userName, MAX_DATA, stdin);
  check(ch != NULL, "Lyckades inte läsa användarnamn.");

  //Remove newline
  stringLength = strlen(userName) - 1;
  if(userName[stringLength] == '\n') userName[stringLength] = '\0';

  rc = strncmp(userName, cancelcode, MAX_DATA);
  if(rc == 0){
    printf("Avbryt\n");
    goto error;
  }

  //Check that username doesn't match existing user
  rc = find(userfile, userName, userNameColumn);
  if(rc != -1){
    printf("Användarnamnet är upptaget.\n");
    goto error;
  }

  //Collect user login
  printf("Ange användarlogin genom att lägga ditt kårkort mot läsaren.\n");
  ch = fgets(userLogin, MAX_DATA, stdin);
  check(ch != NULL, "Lyckades inte läsa login.");

  //Remove newline
  stringLength = strlen(userLogin) - 1;
  if(userLogin[stringLength] == '\n') userLogin[stringLength] = '\0';

  //Check that login doesn't match an existing user
  rc = find(userfile, userLogin, userBarcodeColumn);
  if(rc != -1){
    printf("Användarlogin är upptaget.\n");
    goto error;
  }

  //Check that login doesn't match an item
  rc = find(itemfile, userLogin, itemBarcodeColumn);
  if(rc != -1){
    printf("Du får inte använda en varas streckkod.\n");
    goto error;
  }

  //Check that login doesn't match an admin function
  rc = find(administrationfile, userLogin, administrationIdColumn);
  if(rc != -1){
    printf("Du får inte använda en administrationsfunktion.\n");
    goto error;
  }

  //Generate unique userID
  sprintf(userIdString, "%d", userId);
  while(find(userfile, userIdString, userIdColumn) != -1){
    userId++;
    sprintf(userIdString, "%d", userId);
  }
  
  //Append user to file
  char *values[] = {userIdString, userName, userLogin, "0", "0"};
  rc = appendRow(userfile, values);
  check(rc != -1, "Lyckades inte lägga till användaren i användarfilen.");

  info("[USERADD] %s (%s) added user %s (%s)", user->name, user->userId, userName, userIdString);
  printf("Användaren har lagts till.\n");

  free(userName);
  free(userLogin);
  free(userIdString);

  return 0;

 error:
  if(userName) free(userName);
  if(userLogin) free(userLogin);
  if(userIdString) free(userIdString);
  return -1;
}

//Change the login of the user
int changeUserLogin(struct User *user)
{
  int rc = 0;
  int userRow = 0;
  int stringLength = 0;

  char *ch = NULL;
  char *userLogin = malloc(MAX_DATA *sizeof(char));

  //Find user row
  userRow = find(userfile, user->name, userNameColumn);
  check(userRow != -1, "Hittade inte användaren i användarfilen.");

  //Collect new user login
  printf("Byt login.\n");

  printf("Ange användarlogin genom att lägga ditt kårkort mot läsaren.\n");
  ch = fgets(userLogin, MAX_DATA, stdin);
  check(ch != NULL, "Lyckades inte läsa login.");

  //Remove newline
  stringLength = strlen(userLogin) - 1;
  if(userLogin[stringLength] == '\n') userLogin[stringLength] = '\0';

  //Check that login doesn't match an existing user
  rc = find(userfile, userLogin, userBarcodeColumn);
  if(rc != -1){
    printf("Användarlogin är upptaget.\n");
    goto error;
  }

  //Check that login doesn't match an item
  rc = find(itemfile, userLogin, itemBarcodeColumn);
  if(rc != -1){
    printf("Du får inte använda en varas streckkod.\n");
    goto error;
  }

  //Check that login doesn't match an admin function
  rc = find(administrationfile, userLogin, administrationIdColumn);
  if(rc != -1){
    printf("Du får inte använda en administrationsfunktion.\n");
    goto error;
  }
  
  //Update the user file
  rc = editCell(userfile, userRow, userBarcodeColumn, userLogin);
  check(rc != -1, "Lyckades inte uppdatera login.");

  free(userLogin);

  return 0;

 error:
  if(userLogin) free(userLogin);
  return -1;
}

//Add a custom amount to user balance
int addBalance(struct User *user)
{
  int rc = 0;
  int amount = 0;
  int userRow = 0;
  int stringLength = 0;

  char *ch = NULL;
  char *amountString = malloc(MAX_DATA * sizeof(char));

  //Find user row
  userRow = find(userfile, user->name, userNameColumn);
  check(userRow != -1, "Hittade inte användaren i användarfilen.");

  //Collect new user login
  printf("Lägg till saldo.\n");

  printf("Ange summa på tangentbordet.\n");
  ch = fgets(amountString, MAX_DATA, stdin);
  check(ch != NULL, "Lyckades inte läsa summa.");

  //Remove newline
  stringLength = strlen(amountString) - 1;
  if(amountString[stringLength] == '\n') amountString[stringLength] = '\0';

  //Convert to int and sanity check
  amount = atoi(amountString);
  if(amount > 500){
    printf("Du kan max lägga till 500kr åt gången.\n");
    goto error;
  }

  //Update user in memory
  user->balance += amount;

  //Convert balance to string
  sprintf(amountString, "%d", user->balance);
  
  //Log
  rc = info("[BALANCE]:%s (%s):%s", user->name, user->userId, amountString);
  
  //Update the user file
  rc = editCell(userfile, userRow, userBalanceColumn, amountString);
  check(rc != -1, "Lyckades inte uppdatera balans.");

  printf("Ditt saldo är nu %d.\n", user->balance);

  free(amountString);
  return 0;

 error:
  if(amountString) free(amountString);
  return -1;
}

//Add a new item
int addItem(struct User *user)
{
  int rc = 0;
  int itemId = 0;
  int stringLength = 0;

  char *ch = NULL;
  char *itemName = malloc(MAX_DATA * sizeof(char));
  char *itemBarcode = malloc(MAX_DATA *sizeof(char));
  char *itemIdString = malloc(MAX_DATA *sizeof(char));
  char *itemPriceString = malloc(MAX_DATA *sizeof(char));

  printf("Lägg till ny vara.\n");

  //Collect itemname
  printf("Ange varans namn på tangentbordet.\n");
  ch = fgets(itemName, MAX_DATA, stdin);
  check(ch != NULL, "Lyckades inte läsa varans namn.");

  //Remove newline
  stringLength = strlen(itemName) - 1;
  if(itemName[stringLength] == '\n') itemName[stringLength] = '\0';

  //Check that itemname doesn't match existing item
  rc = find(itemfile, itemName, itemNameColumn);
  if(rc != -1){
    printf("Namnet är upptaget.\n");
    goto error;
  }

  //Collect item barcode
  printf("Ange streckkod genom att skanna varans kod. Skanna en ledig kod på pappret om varan saknar kod.\n");
  ch = fgets(itemBarcode, MAX_DATA, stdin);
  check(ch != NULL, "Lyckades inte läsa streckkod.");

  //Remove newline
  stringLength = strlen(itemBarcode) - 1;
  if(itemBarcode[stringLength] == '\n') itemBarcode[stringLength] = '\0';

  //Check that barcode doesn't match an existing item
  rc = find(itemfile, itemBarcode, itemBarcodeColumn);
  if(rc != -1){
    printf("Streckkoden är upptagen.\n");
    goto error;
  }

  //Check that barcode doesn't match a user
  rc = find(userfile, itemBarcode, userBarcodeColumn);
  if(rc != -1){
    printf("Du får inte använda en användares login.\n");
    goto error;
  }

  //Check that barcode doesn't match an admin function
  rc = find(administrationfile, itemBarcode, administrationIdColumn);
  if(rc != -1){
    printf("Du får inte använda en administrationsfunktion.\n");
    goto error;
  }

  //Collect item barcode
  printf("Ange varans pris.\n");
  ch = fgets(itemPriceString, MAX_DATA, stdin);
  check(ch != NULL, "Lyckades inte läsa priset.");

  //Remove newline
  stringLength = strlen(itemPriceString) - 1;
  if(itemPriceString[stringLength] == '\n') itemPriceString[stringLength] = '\0';

  //Generate unique itemID
  sprintf(itemIdString, "%d", itemId);
  while(find(itemfile, itemIdString, itemIdColumn) != -1){
    itemId++;
    sprintf(itemIdString, "%d", itemId);
  }
  
  //Append user to file
  char *values[] = {itemIdString, itemName, itemBarcode, itemPriceString};
  rc = appendRow(itemfile, values);
  check(rc != -1, "Lyckades inte lägga till varan i varufilen.");

  info("[ITEMADD] %s (%s) added item %s (%s)", user->name, user->userId, itemName, itemIdString);
  printf("Varan har lagts till.\n");

  free(itemName);
  free(itemBarcode);
  free(itemIdString);
  free(itemPriceString);

  return 0;

 error:
  if(itemName) free(itemName);
  if(itemBarcode) free(itemBarcode);
  if(itemIdString) free(itemIdString);
  if(itemPriceString) free(itemPriceString);

  return -1;
}

//Change the barcode of an item
int changeItemBarcode(struct Item *item)
{
  //Collect info
  //Log change
  //Update item barcode
}

//Remove a user
int removeUser()
{
  //Collect username
  //Log user removal
  //Remove user row
}

//Remove an item
int removeItem()
{
  //Collect item barcode
  //Log item removal
  //Delete item row
}
