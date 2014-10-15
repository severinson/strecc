#include <stdlib.h>
#include <stdio.h>

#include "dbg.h"
#include "db.h"
#include "logging.h"
#include "ccsv.h"

//Get item details from database file from a provided barcode.
//The item is stored on the heap and a pointer to it is returned.
struct Item *getItem(char *barcode)
{
  struct Item *item = NULL;
  char *currentItemPriceString = NULL;
  char *currentItemIdString = NULL;
  char *itemBarcode = NULL;

  int currentItemRow = -1;
  currentItemRow = find(itemfile, barcode, itemBarcodeColumn);
  if(currentItemRow == -1){
    printf("Finns ingen vara med den koden.\n");
    goto error;
  }

  char *currentItemName = getCell(itemfile, currentItemRow, itemNameColumn);
  check(currentItemName != NULL, "Lyckades inte hämta varans namn.");

  currentItemPriceString = getCell(itemfile, currentItemRow, itemPriceColumn);
  check(currentItemPriceString != NULL, "Lyckades inte hämta varans pris.");

  currentItemIdString = getCell(itemfile, currentItemRow, itemIdColumn);
  check(currentItemIdString != NULL, "Lyckades inte hämta varans ID.");

  int currentItemPrice = atoi(currentItemPriceString);

  item = malloc(sizeof(struct Item));
  check(item != NULL, "Lyckades inte allokera minne.");

  itemBarcode = malloc(MAX_DATA * sizeof(char));
  check(itemBarcode != NULL, "Lyckades inte allokera minne.");
  strcpy(itemBarcode, barcode);
  
  item->itemId = currentItemIdString;
  item->name = currentItemName;
  item->barcode = itemBarcode;
  item->price = currentItemPrice;

  free(currentItemPriceString);
  return item;

 error:
  if(item) free(item);
  if(itemBarcode) free(itemBarcode);
  if(currentItemPriceString) free(currentItemPriceString);
  return NULL;
}

//Get user details from database file from a provided barcode
//The user it stored on the heap and a pointer is returned
struct User *getUser(char *barcode)
{
  //Allocate memory for a user
  struct User *user = NULL;
  char *currentUserBalanceString = NULL;
  char *userBarcode = NULL;
  char *userIsAdminString = NULL;

  //Attempt to find the user in database
  int currentUserRow = -1;
  currentUserRow = find(userfile, barcode, userBarcodeColumn);
  if(currentUserRow == -1){
    printf("Finns ingen användare med den koden.\n");
    goto error;
  }

  char *currentUserId = getCell(userfile, currentUserRow, userIdColumn);
  check(currentUserId != NULL, "Lyckades inte hämta användarens ID.");

  char *currentUserName = getCell(userfile, currentUserRow, userNameColumn);
  check(currentUserName != NULL, "Lyckades inte hämta användarens namn.");

  currentUserBalanceString = getCell(userfile, currentUserRow, userBalanceColumn);
  check(currentUserBalanceString != NULL, "Lyckades inte hämta användarens saldo.");

  userIsAdminString = getCell(userfile, currentUserRow, userIsAdminColumn);
  check(userIsAdminString != NULL, "Lyckades inte hämta användarens adminstatus.");

  int currentUserBalance = atoi(currentUserBalanceString);
  int currentUserAdmin = atoi(userIsAdminString);
  
  user = malloc(sizeof(struct User));
  check(user != NULL, "Lyckades inte allokera minne.");

  userBarcode = malloc(MAX_DATA * sizeof(char));
  check(userBarcode != NULL, "Lyckades inte allokera minne.");
  strcpy(userBarcode, barcode);

  user->userId = currentUserId;
  user->name = currentUserName;
  user->barcode = userBarcode;
  user->balance = currentUserBalance;
  user->isAdmin = currentUserAdmin;

  //Free the temporary string used for getting the balance
  free(currentUserBalanceString);
  free(userIsAdminString);

  return user;

 error:
  if(user) free(user);
  if(userBarcode) free(userBarcode);
  if(currentUserBalanceString) free(currentUserBalanceString);
  if(userIsAdminString) free(userIsAdminString);
  return NULL;
}
