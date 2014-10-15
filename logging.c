#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "db.h"
#include "logging.h"
#include "dbg.h"
#include "ccsv.h"

//Update user balance with the cost of item
int updateUserBalance(struct User *user, struct Item *item)
{
  int currentUserRow = find(userfile, user->barcode, userBarcodeColumn);
  check(currentUserRow != -1, "Kunde inte hitta användaren i databas för att uppdatera balans.");
  
  char newBalance[MAX_DATA] = "";
  user->balance -= item->price;
  sprintf(newBalance, "%d", user->balance);

  int rc = editCell(userfile, currentUserRow, userBalanceColumn, newBalance);
  check(rc != -1, "Kunde inte uppdatera saldo.");

  return 0;

 error:
  return -1;
}

//Add a transaction to log
int addTransaction(struct User *user, struct Item *item){
  int rc = 0;

  //Generate timestamp
  char *timestamp = generateTimestamp();
  check(timestamp != NULL, "");

  char priceString[MAX_DATA] = "";
  sprintf(priceString, "%d", item->price);

  //Put values in an array
  char *values[] = {timestamp, item->name, item->itemId, user->name, user->userId, priceString};

  //Log transaction to file
  rc = appendRow(transactionfile, values);
  check(rc == 0, "Lyckades inte lägga till transaktion i loggfilen.");

  return 0;

 error:
  return -1;
}

int info(const char *str, ...)
{
  FILE *file = NULL;
  file = fopen(logfile, "a");
  check(file != NULL, "Kunde inte öppna loggfilen.");

  //Generate timestamp
  char *timestamp = generateTimestamp();
  check(timestamp != NULL, "");

  //Write the timestamp
  fprintf(file, "%s%c", timestamp, ',');

  //Do tha variable argument vodoo
  va_list arg;
  int done;

  va_start(arg, str);
  done = vfprintf(file, str, arg);
  va_end(arg);
  
  //Write newline
  fprintf(file, "%c", '\n');

  fclose(file);
  return done;

 error:
  if(file) fclose(file);
  return -1;
}

//Generate a timestamp from the current time
char *generateTimestamp()
{
  time_t ltime; /* calendar time */
  ltime=time(NULL); /* get current cal time */
  check(ltime != -1, "Lyckades inte generera tidsstämpel. Kontakta administratören.");

  char *timestamp = asctime(localtime(&ltime));

  //Remove newline
  int timestampLength = strlen(timestamp) - 1;
  if(timestamp[timestampLength] == '\n') timestamp[timestampLength] = '\0';

  return timestamp;

 error:
  return NULL;
}

int clearTerminal(int n)
{
  int i = 0;

  for(i = 0;i < n;i++) printf("\n");

  return 0;
}

//Log a message to the strecc log. Also log which user caused it.
int logAdminFunction(struct User *user, char *message)
{
  
}
