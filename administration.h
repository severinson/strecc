#ifndef _administration_h
#define _administration_h

#define cancelcode "00000"

//Will undo the last transaction for user
int undoTransaction(struct User *user);

//Add a new user
int addUser();

//Change the login of the user
int changeUserLogin(struct User *user);

//Add a custom amount to user balance
int addBalance(struct User *user);

//Add a new item
int addItem();

//Change the barcode of an item
int changeItemBarcode(struct Item *item);

//Remove a user
int removeUser();

//Remove an item
int removeItem();

#endif
