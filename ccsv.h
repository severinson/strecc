#ifndef _ccsv_h
#define _ccsv_h

//The largest string the system can use.
#define MAX_DATA 80

//List items in file
int listitems(char *filename);

//Get the number of rows in file
int getRows(char *filename);

//Get the number of columns of the first row in file
int getColumns(char *filename);

//Get the data in a given cell
char *getCell(char *filename, int row, int col);

//Find a row containing the search string. Return -1 if not found.
int find(char *filename, char *searchword, int col);

//Find the next occurence of search string. Return -1 if not found.
//Reset by calling with different file or with filename NULL
int findNext(char *filename, char *searchword, int col);

//Append a row to file with values[].
int appendRow(char *filename, char *values[]);

//Add a header to a file. File is created if it does not exists.
//Otherwise it is truncated to zero length.
int addHeader(char *filename, char *values[], int numValues);

//Delete a row
int removeRow(char *filename, int rowToRemove);

//Delete a file
int removeFile(char *filename);

//Edit a cell
int editCell(char *filename, int row, int col, char *value);

//Check that a file conforms to CCSV file syntax.
int isValidCsv(char *filename);

#endif
