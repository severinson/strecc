#define terminalRows 15
#define terminalColumns 40

struct Ui
{
  char *top;
  int toplength;
  char *middle;
  int middlelength;
  char *bottom;
  int bottomlength;
};

//Initialize the ui
int initUi();

//Set top, middle or bottom message
int setUi(char *location, char *str, ...);

//Refresh the user interface
int refreshUi();
