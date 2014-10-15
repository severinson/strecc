#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbg.h"
#include "ccsv.h"

int listItems(char *filename)
{
  int fileRows = getRows(filename);
  int fileColumns = getColumns(filename);

  int curRow = 0;
  int curCol = 0;

  char *str = "";

  for(curRow = 0;curRow < fileRows;curRow++){
    for(curCol = 0;curCol <= fileColumns;curCol++){
      str = getCell(filename, curRow, curCol);
      check(str != NULL, "Failed to list file items.");

      printf("%s,", str);
      free(str);
    }
    printf("\n");
  }

  return 0;

 error:
  if(str) free(str);
  return -1;
}

//Get the number of rows
int getRows(char *filename)
{
  FILE *file = NULL;
  int ch = 0;
  int i = 0;

  file = fopen(filename, "r");
  check(file, "Could not open file.");

  while(!feof(file)){
  ch = fgetc(file);
  if(ch == '\n') i++;
  }

  fclose(file);
  return i; 

 error:
  if(file) fclose(file);
  return -1;
}

//Get the number of columns
int getColumns(char *filename)
{
  FILE *file = NULL;
  int ch = 0;
  int i = 0;

  file = fopen(filename, "r");
  check(file, "Could not open file.");

  while(ch != '\n' && !feof(file)){
    ch = fgetc(file);
    if(ch == ',') i++;
  }

  fclose(file);
  return i;

 error:
  if(file) fclose(file);
  return -1;
}

//return the value of the indicated cell
char *getCell(char *filename, int row, int col)
{
  //Check that a nonnegative row and column was submited.
  check(row >= 0 && col >= 0, "Row and column must be nonnegative.");

  int fileRows = getRows(filename);
  int fileColumns = getColumns(filename);

  //Check that the submited row and column is within document limits.
  check(row <= fileRows, "Row is out of bounds.");
  check(col <= fileColumns, "Column if out of bounds.");

  int curRow = 0;
  int curCol  = 0;
  int ch = 0;

  char *key = NULL;

  char str[MAX_DATA] = "";
  int i = 0;

  FILE *file = NULL;
  file = fopen(filename, "r");
  check(file, "Could not open file.");
  clearerr(file);

  //Step to the indicated row.
  while(curRow < row){
    ch = fgetc(file);
    check(!feof(file), "Row is out of bounds.");

    if(ch == '\n') curRow++;
  }

  //Step to the indicated column
  while(curCol < col){
    ch = fgetc(file);
    check(ch != '\n', "Column is out of bounds.");

    if(ch == ',') curCol++;
  }

  //Step past the current ","
  ch = fgetc(file);

  //Store the data in that cell in str
  while(ch != ',' && ch != '\n'){
    str[i] = ch;
    i++;

    ch = fgetc(file);
  }
  str[i] = '\0';

  int strlength = strlen(str);

  key = malloc(MAX_DATA * sizeof(char));
  strcpy(key, str);

  fclose(file);

  return key;

 error:
  if(file) fclose(file);
  if(key) free(key);
  return NULL;

}

//Find the first row for which column has value string
//Find is depracted in favor of findNext. Find currently acts as a wrapper.
int find(char* filename, char *searchword, int col)
{
  int row = findNext(filename, searchword, col);
  findNext(NULL, NULL, 0);
  return row;
  /*
  int row = -1;
  int curRow = 0;

  int fileRows = getRows(filename);
  int fileColumns = getColumns(filename);
  check(col <= fileColumns, "Column must be within file limits when using find.");

  char *str = NULL;

  for(curRow = 0;curRow < fileRows;curRow++){
    str = getCell(filename, curRow, col);
    check(str != NULL, "Error in csv file format.");

    if(strncmp(str, searchword, MAX_DATA) == 0){
      row = curRow;
      free(str);
      return row;
    }
    free(str);
  }

  return row;

 error:
  if(str) free(str);
  return -1;
  */
}

int findNext(char *filename, char *searchword, int col)
{
  int row = -1;
  char *ch = NULL;
  static int curRow = 0;
  static char *file = NULL;

  if(file == NULL){
    file = malloc(MAX_DATA * sizeof(char));
    check(file != NULL, "Lyckades inte allokera minne.");
    ch = strncpy(file, "", MAX_DATA);
    check(ch != NULL, "Lyckades inte kopiera filnamn.");
  }

  if(filename == NULL){
    curRow = 0;
    return 0;
  }

  if(strncmp(file, filename, MAX_DATA) != 0){
    curRow = 0;
    ch = strncpy(file, filename, MAX_DATA);
    check(ch != NULL, "Lyckades inte kopiera filnamn.");
  }
  else curRow++;

  int fileRows = getRows(filename);
  int fileColumns = getColumns(filename);
  check(col <= fileColumns, "Column must be within file limits when using findNext.");

  char *str = NULL;

  //Step to next occurence
  for(curRow;curRow < fileRows;curRow++){
    str = getCell(filename, curRow, col);
    check(str != NULL, "Error in csv file format.");

    if(strncmp(str, searchword, MAX_DATA) == 0){
      row = curRow;
      free(str);
      return row;
    }
    free(str);
  }

  return row;

 error:
  if(str) free(str);
  return -1;
}

//Append a row to the file
int appendRow(char *filename, char *values[])
{
  int fileRows = getRows(filename);
  int fileColumns = getColumns(filename);

  int curCol = 0;
  int i = 0;
  char *str = "";

  FILE *file = NULL;
  file = fopen(filename, "a");
  check(file != NULL, "Failed to open file %s.", filename);

  for(curCol = 0;curCol <= fileColumns;curCol++){
    str = values[curCol];
    fputs(str, file);

    if(curCol == fileColumns) fputs("\n", file);
    else fputs(",", file);
  }

  fflush(file);
  fclose(file);
  return 0;

 error:
  if(file) fclose(file);
  return -1;
}

//Add header to an empty file
int addHeader(char *filename, char *values[], int numValues)
{
  FILE *file = NULL;
  file = fopen(filename, "w");
  check(file != NULL, "Failed to open file %s.", filename);
  
  char *str = "";
  int i = 0;
  for(i = 0;i < numValues;i++){
    str = values[i];
    fputs(str, file);
    if(i != numValues - 1) fputs(",", file);
  }
  fputs("\n", file);

  fclose(file);
  return 0;
 error:
  if(file) fclose(file);
  return -1;
}

//Remove a given row from file
int removeRow(char *filename, int rowToRemove)
{
  //Create a temporary file
  FILE *tmp = NULL;
  tmp = fopen("tmp.csv", "w");
  check(tmp != NULL, "Failed to create a temporary file.");

  //Get the number of rows and columns in the file
  int fileRows = getRows(filename);
  int fileColumns = getColumns(filename);

  int curRow = 0;
  int curCol = 0;

  check(rowToRemove <= fileRows, "Row must be whithin file limits when deleting.");
  check(rowToRemove >= 0, "Row number must be nonnagative when deleting.")

  char *str = "";
  int rc;

  //Copy all rows except the row that is to be deleted to the tmp file
  for(curRow = 0;curRow < fileRows;curRow++){

    if(curRow != rowToRemove){
      for(curCol = 0;curCol <= fileColumns;curCol++){
        str = getCell(filename, curRow, curCol);
        fputs(str, tmp);
        free(str);

        if(curCol == fileColumns) fputs("\n", tmp);
        else fputs(",", tmp);
      }
    }
  }

  rc = rename("tmp.csv", filename);
  check(rc == 0, "Failed to replace %s with the tmp file.", filename);

  fflush(tmp);
  fclose(tmp);

  return 0;

 error:
  if(tmp) fclose(tmp);
  if(str) free(str);
  return -1;
}

int removeFile(char *filename)
{
  
  int rc = remove(filename);
  check(rc != -1, "File %s not found", filename);

  return 0;
 error:
  return -1;
  
}

//Edit the value of a cell
int editCell(char *filename, int row, int col, char *value)
{
  //Create a temporary file
  FILE *tmp = NULL;
  tmp = fopen("tmp.csv", "w");
  check(tmp != NULL, "Failed to create a temporary file.");

  //Get the number of rows and columns in the file
  int fileRows = getRows(filename);
  int fileColumns = getColumns(filename);

  int curRow = 0;
  int curCol = 0;

  check(row <= fileRows, "Row must be whithin file limits when editing.");
  check(row >= 0, "Row number must be nonnagative when editing.")

  char *str = "";
  int rc;

  //Copy all cells but replace the designated cell
  for(curRow = 0;curRow < fileRows;curRow++){
    for(curCol = 0;curCol <= fileColumns;curCol++){

      if(curRow == row && curCol == col){
        str = value;
        fputs(str, tmp);
      }

      else{
        str = getCell(filename, curRow, curCol);
        fputs(str, tmp);
        free(str);
      }

      if(curCol == fileColumns) fputs("\n", tmp);
      else fputs(",", tmp);
    }
  }

  rc = rename("tmp.csv", filename);
  check(rc == 0, "Failed to replace %s with the tmp file.", filename);

  fflush(tmp);
  fclose(tmp);

  return 0;

 error:
  if(tmp) fclose(tmp);
  if(str) free(str);
  return -1;
}

//Check that the indicated csv file has valid syntax
//Valid syntax means every row has the same number of columns
int isValidCsv(char *filename)
{
  FILE *file = NULL;
  file = fopen(filename, "r");
  check(file != NULL, "Filen %s finns inte.", filename);

  double rows = 0;
  double columns = 0;
  double quote = getColumns(filename);

  char ch = 0;

  while(!feof(file)){
    ch = fgetc(file);
    if(ch == ',') columns++;
    else if(ch == '\n'){
      rows++;
      check(columns == quote, "%s isn't a valid CSV file. File must exist and have the same number of columns at every row.", filename);
      columns = 0;
    }
  }

  return 0;

 error:
  if(file) fclose(file);
  return -1;
}
