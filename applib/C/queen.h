#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define AREAMEMORY 256

//Struct Area
struct areaStruct{
    char enable;
    char name[32];
    int x1;
    int y1;
    int x2;
    int y2;
};
struct areaStruct areaList[AREAMEMORY];

//Predichiarazione
void onEvent(char typeEvent, int arg[3]);

//Vars
pthread_t threads;
unsigned char bufferRet[1024], idCon=0, ref=0, sendTitle[1024];

void splitChar(int numb, unsigned char splitNumb[3]) //Dividere int in due char
{
    splitNumb[0] = (unsigned char)(numb / 256);
    splitNumb[1] = (unsigned char)(numb % 256);
}

void stampa(char toSend[1024], int lenght) //Stampa forzata stringa (pel debug)
{
    int i;
    for(i=0;i<lenght;i++) printf(" %c",toSend[i]);
    printf("\n");
}

void sendByte(char toSend[1024], char isRet, int lenght)
{
	int SPORT = 24000;			// Porta TCP Server
	char IP[12] = "127.0.0.1"; 	// Indirizzo IP Server

	int server, err, byte;
	struct sockaddr_in server_addr;

	//	Creazione del socket
	server = socket(AF_INET, SOCK_STREAM, 0);

	//	Struttura relativa alle informazioni sul Server
	server_addr.sin_family		= AF_INET;
	server_addr.sin_port		= htons((u_short)SPORT);
	server_addr.sin_addr.s_addr	= inet_addr(IP);

	//	La funzione connect() restituisce un codice di errore
	err = connect (server, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(err == 0)
	{
		send (server,toSend,lenght,0);
		if(isRet==1)recv (server, bufferRet, 1024, 0);
	}
	else
	{
	    ref = -1;
		perror (bufferRet);
	}

	//	Chiusura socket
	close(server);
}

void *server(void *arg) {
    //Socket
    int SPORT = 24000+idCon;	// Porta TCP Server
	int server, client;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr), byte;
	unsigned char buffer[7];

	//	Struttura con le informazioni sul server per effettuare il BIND
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons((u_short) SPORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	server = socket (AF_INET, SOCK_STREAM, 0);
	if (server < 0)
	{
		printf ("\n\n");
		perror (buffer);
		printf ("\n\n");
	}


	//	Il BIND associa al socket una porta
	bind (server,(struct sockaddr*)&server_addr,sizeof(server_addr));
	listen(server,1);

	//	L'applicazione rimane in attesa di connessione
	while(1)
	{
        client = accept (server,(struct sockaddr*) &client_addr, &client_len);
        if (client >= 0)
        {
            memset(buffer,'\0',7);
            byte = recv(client,buffer,7,0);

            if(buffer[0]==1)
            {
                int argt[3] = {(int)buffer[1],0,0};
                onEvent(1,argt);
            }

            if(buffer[0]==2)
            {
                unsigned char xc1 = buffer[2], xc2 = buffer[3], yc1 = buffer[4], yc2 = buffer[5];
                int xp = (xc1) * 256 + (xc2);
                int yp = (yc1) * 256 + (yc2);

                int argt[3] = {xp,yp,buffer[1]};
                onEvent(2,argt);
            }
        }
	}
}


int initQueen()
{
    ref=0;
    sendByte("01\0",1,3);
    idCon = atoi(bufferRet);

    pthread_create(&threads,NULL,server,(void*) 0); //Crea thraead

    return ref;
}

void closeQueen()
{
    memset(sendTitle,'\0',1024);
    snprintf(sendTitle,4,"02%c\0",idCon);
    sendByte(sendTitle,0,4);

    //pthread_join (threads, NULL); //Chiusura thread
}

void quSetTitle(char title[])
{
    memset(sendTitle,'\0',1024);
    snprintf(sendTitle,1024,"03%c%s\0",idCon,title);
    sendByte(sendTitle,0,4+strlen(title));
}

//Strumenti di disegno
int quHereYouAre()
{
    ref=0;
    sendByte("04\0",1,3);
    if(ref==0)return bufferRet[0];
    else return -1;
}

void qrWrite(int x, int y, char text[], char col, char bg)
{
    char xp[2], yp[2];
    splitChar(x, xp);
    splitChar(y, yp);

    memset(sendTitle,'\0',1024);
    snprintf(sendTitle,1024,"05%c%c%c%c%c%c%c%s\0",idCon,xp[0],xp[1],yp[0],yp[1],col,bg,text);
    sendByte(sendTitle,0,11+strlen(text));
}

void qrLine(int x1, int y1, int x2, int y2, char col)
{
    char xp1[2], yp1[2], xp2[2], yp2[2];
    splitChar(x1, xp1);
    splitChar(y1, yp1);
    splitChar(x2, xp2);
    splitChar(y2, yp2);

    memset(sendTitle,'\0',1024);
    snprintf(sendTitle,1024,"06%c%c%c%c%c%c%c%c%c%c\0",idCon,xp1[0],xp1[1],yp1[0],yp1[1],xp2[0],xp2[1],yp2[0],yp2[1],col);
    sendByte(sendTitle,0,13);
}

void qrPoint(int x, int y, char col)
{
    char xp[2], yp[2];
    splitChar(x, xp);
    splitChar(y, yp);

    memset(sendTitle,'\0',1024);
    snprintf(sendTitle,1024,"07%c%c%c%c%c%c\0",idCon,xp[0],xp[1],yp[0],yp[1],col);
    sendByte(sendTitle,0,9);
}

//Gestione area
void areaAdd(char name[32], int x1, int y1, int x2, int y2)
{
    int i;
    for(i=0;i<AREAMEMORY;i++) if(areaList[i].enable!=1)break;
    if(i<AREAMEMORY)
    {
        areaList[i].enable = 1;
        areaList[i].x1 = x1;
        areaList[i].x2 = x2;
        areaList[i].y1 = y1;
        areaList[i].y2 = y2;
        strcpy(areaList[i].name,name);
    }
}

void areaDel(char name[32])
{
    int i;
    for(i=0;i<AREAMEMORY;i++)if(areaList[i].enable == 1 && strcmp(name, areaList[i].name)) areaList[i].enable = 0;
}

int areaGet(int x, int y)
{
    int i;
    for(i=0;i<AREAMEMORY;i++)if(areaList[i].enable == 1)
    {
        if((x >= areaList[i].x1 && x <= areaList[i].x2) && (y >= areaList[i].y1 && y <= areaList[i].y2)) return i;
    }

    return -1;
}

//Creazione rettangolo
void drawEmptyRect(int x1, int y1, int x2, int y2, char col)
{
    qrLine(x1,y1,x1,y2,col);
    qrLine(x1,y1,x2,y1,col);
    qrLine(x2,y2,x1,y2,col);
    qrLine(x2,y2,x2,y1,col);
}

void drawRect(int x1, int y1, int x2, int y2, char col)
{
    int i;
    for(i=y1;i<=y2;i++) qrLine(x1,i,x2,i,col);
}
