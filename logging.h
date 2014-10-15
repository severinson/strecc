#ifndef _logging_h
#define _logging_h

#define terminalRows 14

//Update user balance with amount
int updateUserBalance(struct User *user, struct Item *item);

//Add a transaction
int addTransaction(struct User *user, struct Item *item);

//Generate a timestamp
char *generateTimestamp();

//Undo previous transactions. Up to 5 minutes past.
int undoTransaction();

//Clear terminal
int clearTerminal(int n);
#endif
