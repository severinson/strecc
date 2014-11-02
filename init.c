#include <stdio.h>

#include "db.h"
#include "dbg.h"

//Initialize the strecc system
//Check that all files are present and have proper syntax.
//Initialize the UI (not yet implemented)
int initialize()
{
  int rc = 0;

  //Check that all required files are present and use correct CSV syntax
  rc = isValidCsv(itemfile);
  check(rc == 0, "Produktfilen items.csv finns inte eller har felaktig syntax.");
  rc = getColumns(itemfile);
  check(rc == itemFileColumns, "Produktfilen har felaktig syntax.");
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
  printf("strecclog.csv ser helt ok ut!\n");

  //Initialize the user interface
  rc = initUi();
  check(rc == 0, "Lyckades inte initialisera användargränssnittet.");

  return 0;

 error:
  return -1;
}
