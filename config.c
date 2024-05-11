//Risoluzione
#define SIZEX 1024
#define SIZEY 768

//Bar size
#define VERTICAL_BAR_SIZE 16
#define VERTICAL_BAR_DOWN 24
#define HORIZONTAL_BAR_SIZE 150

//Font default
#define FONT_HEIGHT  8
#define FONT_WIDTH   8

//Costante finestre
#define NUMWINDOWS 12
#define LENGHTITLE 50
#define QUEUELENGHT 1024

//Variabile di cambiamento
#define CHANGESVAR 8

//Mouse
#define MOUSECLICKTIME 100
#define DOUBLECLICKTIME 300
#define LIMITFORCLICK 50

//Toolbar
#define TOOLALT  26
#define NUMTOOL 15

//Buckup Screen
#define BUCKUPSCR 128

//Dichiarazione variabili
int winx = SIZEX - HORIZONTAL_BAR_SIZE, winy=HORIZONTAL_BAR_SIZE-VERTICAL_BAR_SIZE-VERTICAL_BAR_DOWN;
char changes[CHANGESVAR]; //Controlla se ci sono stati cambiamenti
char *buck[BUCKUPSCR], freebuck[BUCKUPSCR];
char textTool[NUMTOOL][4][12], freeTextTool[NUMTOOL];

//Variabili generali
unsigned int mouseX, mouseY, ultAggTime;
struct timeb tp;

//Gestione finestre
char windowsBool[NUMWINDOWS], windowsTitle[NUMWINDOWS][LENGHTITLE], visibleWin=0;
char pixelWin[NUMWINDOWS][SIZEX-HORIZONTAL_BAR_SIZE][SIZEY-(VERTICAL_BAR_SIZE*2)]; //No malloc :)

//Structure queue
struct drawStructText{
      char enable;
      char screen;
      int x;
      int y;
      int col;
      int bg;
      char text[1024];
};
struct drawStructText drawQueueText[QUEUELENGHT];

struct drawStructLine{
      char enable;
      char screen;
      int x1;
      int y1;
      int x2;
      int y2;
      int col;
};
struct drawStructLine drawQueueLine[QUEUELENGHT];

struct drawStructPoint{
      char enable;
      char screen;
      int x;
      int y;
      int col;
};
struct drawStructPoint drawQueuePoint[QUEUELENGHT*2];

//Mouse Manager
int mousePrecClick[2] = {0,0};

