#ifndef _db_h
#define _db_h

#define MAX_DATA 80

#define itemfile "items.csv"
#define userfile "users.csv"
#define transactionfile "transactions.csv"
#define administrationfile "administration.csv"
#define logfile "strecclog.csv"

#define userIdColumn 0
#define userNameColumn 1
#define userBarcodeColumn 2
#define userBalanceColumn 3
#define userIsAdminColumn 4
#define userFileColumns 4

#define itemIdColumn 0
#define itemNameColumn 1
#define itemBarcodeColumn 2
#define itemPriceColumn 3
#define itemFileColumns 3

#define transactionTimestampColumn 0
#define transactionItemColumn 1
#define transactionItemIdColumn 2
#define transactionUserColumn 3
#define transactionUserIdColumn 4
#define transactionItemPriceColumn 5
#define transactionFileColumns 5

#define administrationIdColumn 0
#define administrationFunctionColumn 1
#define administrationFileColumns 1

#define logFileColumns 1

struct User
{
  char *userId;
  char *name;
  char *barcode;
  int balance;
  int isAdmin;
};

struct Item
{
  char *itemId;
  char *name;
  char *barcode;
  int price;
};

struct Transaction
{
  char *timestamp;
  struct Item item;
  struct User user;
};

//Get item from database file
struct Item *getItem(char *barcode);

//Get user from database file
struct User *getUser(char *barcode);
#endif
