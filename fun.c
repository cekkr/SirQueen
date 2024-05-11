 //Predichiarazione
 void changeScreenTo(char display);

 struct timeb {
      time_t   time;
      unsigned short millitm;
      short    timezone;
      short    dstflag;
 };

int getDrawQueueText() //Ricerca di una row disponibile in coda nel testo
{
    int i;
    for(i=0;i<QUEUELENGHT;i++) if(drawQueueText[i].enable!=1) return i;
    return 0;
}

int getDrawQueueLine() //Ricerca di una row disponibile in coda nel testo
{
    int i;
    for(i=0;i<QUEUELENGHT;i++) if(drawQueueLine[i].enable!=1) return i;
    return 0;
}

int getDrawQueuePoint() //Ricerca di una row disponibile in coda nel testo
{
    int i;
    for(i=0;i<QUEUELENGHT*2;i++) if(drawQueuePoint[i].enable!=1) return i;
    return 0;
}

void rect(int x1, int y1, int x2, int y2, int col)
{
    int xp, yp;
    vga_setcolor(col);
    for(yp=0;yp<(y2-y1);yp++)for(xp=0;xp<(x2-x1);xp++)
    {
        vga_drawpixel(xp+x1,yp+y1);
    }
}

void splitChar(int numb, unsigned char splitNumb[2]) //Dividere int in due char
{
    splitNumb[0] = (unsigned char)(numb / 128);
    splitNumb[1] = (unsigned char)(numb % 128);
}

void charCancel(int xp, int yp, int len, char col) //Cancella tot di scrittura
{
    rect(xp,yp,xp+(len*8),yp+8,col);
}

void shifPch(unsigned char pch[5]) //Shift ricezione tastiera
{
    pch[0] = pch[1];
    pch[1] = pch[2];
    pch[2] = pch[3];
    pch[3] = pch[4];
}

void clearScreen ()
{
    int x, y;

    vga_setcolor(0);
    for(y=0;y<SIZEY;y++)for(x=0;x<SIZEX;x++)
    {
        vga_drawpixel(x,y);
    }
}

float distanza(float x1, float y1, float x2, float y2) //Calcola distanza fra due punti
{
    return sqrt(((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2)));
}

void firstDraw()
{
    vga_setcolor(4); //Impostazione colore

    //Posizionamento limiti barre
    //Linea superiore
    vga_drawline(x(0), y(VERTICAL_BAR_SIZE), x(SIZEX), y(VERTICAL_BAR_SIZE));
    //Linea sinistra
    //vga_drawline(x(HORIZONTAL_BAR_SIZE), y(VERTICAL_BAR_SIZE), x(HORIZONTAL_BAR_SIZE), y(SIZEY-VERTICAL_BAR_SIZE));
    //Linea destra
    vga_drawline(x(SIZEX-HORIZONTAL_BAR_SIZE), y(VERTICAL_BAR_SIZE), x(SIZEX-HORIZONTAL_BAR_SIZE), y(SIZEY-VERTICAL_BAR_DOWN));

    //Linea inferiore
    vga_setcolor(1);
    vga_drawline(x(0), y(SIZEY-VERTICAL_BAR_DOWN), x(SIZEX), y(SIZEY-VERTICAL_BAR_DOWN));


    //APPLICAZIONI
    //Divisione barra inferiore
    vga_setcolor(1);
    vga_drawline(x(0), y(SIZEY-(VERTICAL_BAR_DOWN/2)), x(SIZEX), y(SIZEY-(VERTICAL_BAR_DOWN/2)));

    //Disegno barra applicazioni
    int i;
    for(i=1;i<6;i++)
    {
        vga_drawline(x(i*(SIZEX/6)), y(SIZEY-VERTICAL_BAR_DOWN), x(i*(SIZEX/6)), y(SIZEY));
    }

    //Disegno applicazioni
    for(i=0;i<12;i++)
    {
        char numberBar[3], numberBarFi[3] = "0\0";
        snprintf(numberBar, 3,"%d", (i+1));
        if(i<9) strcat(numberBarFi,numberBar);
        else strcpy(numberBarFi,numberBar);

        writeText(x(((SIZEX/6)*(i%6))+2), y((SIZEY-VERTICAL_BAR_DOWN)+((i/6)*(VERTICAL_BAR_DOWN/2))+2),numberBarFi,15,0);
    }

    //Disegno toolbar
    int alt = 26, spazio = SIZEY - VERTICAL_BAR_DOWN+VERTICAL_BAR_SIZE;
    vga_setcolor(7);
    for(i=1;i<NUMTOOL;i++)
    {
        vga_drawline(x(SIZEX-HORIZONTAL_BAR_SIZE+25), y(VERTICAL_BAR_SIZE+TOOLALT*i), x(SIZEX-25), y(VERTICAL_BAR_SIZE+TOOLALT*i));
    }

    vga_drawline(x(SIZEX-HORIZONTAL_BAR_SIZE), y(VERTICAL_BAR_SIZE+TOOLALT*(i+1)), x(SIZEX), y(VERTICAL_BAR_SIZE+TOOLALT*(i+1)));

}


void *tcpExc(void *arg) {
    //Socket
    int SPORT = 24000;	// Porta TCP Server
	int server, client;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr), byte;
	unsigned char buffer[1024], buffersend[1024];

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
            memset(buffer,'\0',1024);
            byte = recv(client,buffer,1024,0);

            int iB = 0, i;
            while(1)
            {
                if(buffer[iB]=='0' && buffer[iB+1]=='1') //Richiesta spazio nel windows manager
                {
                    char prod = 0;

                    for(i=0;i<NUMWINDOWS;i++) if(windowsBool[i]==0)
                    {
                        windowsBool[i] = 1;

                        snprintf(buffersend,1024,"%d",(i+1));
                        send (client, buffersend, strlen(buffersend), 0);
                        prod = 1;

                        changes[5] = i+1;
                        break;
                    }

                    if(prod==0) send (client, "-1", 2, 0);
                    break;

                } else
                if(buffer[iB]=='0' && buffer[iB+1]=='2') //Eliminazione spazio
                {
                    windowsBool[buffer[iB+2]-1] = 0;

                    memset(windowsTitle[buffer[iB+2]-1],'\0',LENGHTITLE);

                    changeScreenTo(0);

                    break;
                } else
                if(buffer[iB]=='0' && buffer[iB+1]=='3') //Settaggio del titolo finestra
                {
                    for(i=0;i<LENGHTITLE && buffer[iB+2+i]!='\0';i++)
                    {
                        windowsTitle[buffer[iB+2]-1][i] = buffer[iB+3+i];
                    }

                    changes[0]=1;
                    break;
                } else
                if(buffer[iB]=='0' && buffer[iB+1]=='4') //Ottieni finestra attiva
                {
                     snprintf(buffersend,1024,"%c",(visibleWin+1));
                     send (client, buffersend, 2, 0);
                     break;
                }
                if(buffer[iB]=='0' && buffer[iB+1]=='5') //Scrivi testo
                {
                    unsigned char xc1 = buffer[iB+3], xc2 = buffer[iB+4], yc1 = buffer[iB+5], yc2 = buffer[iB+6];
                    int xp = (xc1) * 128 + (xc2);
                    int yp = (yc1) * 128 + (yc2);
                    unsigned char col = buffer[iB+7], cbg = buffer[iB+8], towrite[1024], idSes = buffer[iB+2]-1;

                    for(i=0;i<byte;i++)
                    {
                        towrite[i] = buffer[iB+9+i];
                        if(towrite[i]=='\0') break;
                    }

                    int indStruct = getDrawQueueText();
                    drawQueueText[indStruct].screen = idSes;
                    drawQueueText[indStruct].x = xp;
                    drawQueueText[indStruct].y = yp + VERTICAL_BAR_SIZE;
                    drawQueueText[indStruct].col = col;
                    drawQueueText[indStruct].bg = cbg;
                    strcpy(drawQueueText[indStruct].text, towrite);
                    drawQueueText[indStruct].enable = 1;

                    changes[2] = 1;

                    break;
                } else
                if(buffer[iB]=='0' && buffer[iB+1]=='6') //Disegna linea
                {
                    unsigned char xc1 = buffer[iB+3], xc2 = buffer[iB+4], yc1 = buffer[iB+5], yc2 = buffer[iB+6];
                    unsigned char xc12 = buffer[iB+7], xc22 = buffer[iB+8], yc12 = buffer[iB+9], yc22 = buffer[iB+10];
                    unsigned char idSes = buffer[iB+2]-1, col = buffer[iB+11];
                    int xp1 = (xc1) * 128 + (xc2);
                    int yp1 = (yc1) * 128 + (yc2);
                    int xp2 = (xc12) * 128 + (xc22);
                    int yp2 = (yc12) * 128 + (yc22);

                    int indStruct = getDrawQueueLine();

                    drawQueueLine[indStruct].screen = idSes;
                    drawQueueLine[indStruct].x1 = xp1;
                    drawQueueLine[indStruct].x2 = xp2;
                    drawQueueLine[indStruct].y1 = yp1;
                    drawQueueLine[indStruct].y2 = yp2;
                    drawQueueLine[indStruct].col = col;
                    drawQueueLine[indStruct].enable = 1;

                    changes[3] = 1;

                    break;
                } else
                if(buffer[iB]=='0' && buffer[iB+1]=='7') //Disegna punto
                {
                    unsigned char xc1 = buffer[iB+3], xc2 = buffer[iB+4], yc1 = buffer[iB+5], yc2 = buffer[iB+6];
                    int xp = (xc1) * 128 + (xc2);
                    int yp = (yc1) * 128 + (yc2);

                    unsigned char col = buffer[iB+7], idSes = buffer[iB+2]-1;

                    int indStruct = getDrawQueuePoint();
                    drawQueuePoint[indStruct].screen = idSes;
                    drawQueuePoint[indStruct].x = xp;
                    drawQueuePoint[indStruct].y = yp;
                    drawQueuePoint[indStruct].col = col;
                    drawQueuePoint[indStruct].enable = 1;

                    changes[4] = 1;

                    break;
                } else
                if(buffer[iB]=='0' && buffer[iB+1]=='8') //Ottieni mouse
                {
                    char xp[2], yp[2];
                    splitChar(mouseX,xp);
                    splitChar(mouseY,yp);

                    snprintf(buffersend,1024,"%c%c%c%c\0",xp[0],xp[1],yp[0],yp[1]);
                    send (client, buffersend, 5, 0);

                    break;
                } else
                if(buffer[iB]=='0' && buffer[iB+1]=='9') //Ottenere tool box
                {
                    char pres = 1;
                    for(i=0;i<NUMTOOL;i++) if(freeTextTool[i]==0)
                    {
                        freeTextTool[i] = 1;

                        snprintf(buffersend,10,"%d\0",i);
                        send (client, buffersend, strlen(buffersend), 0);

                        pres=0;
                        break;
                    }

                    if(pres==1)
                    {
                        snprintf(buffersend,10,"%d\0",-1);
                        send (client, buffersend, strlen(buffersend), 0);
                    }

                    break;
                } else
                if(buffer[iB]=='1' && buffer[iB+1]=='0') //Eliminare da toolbox
                {
                    char numTool = buffer[iB+2];
                    charCancel(SIZEX-HORIZONTAL_BAR_SIZE+1,1+VERTICAL_BAR_SIZE+TOOLALT*numTool,18,0);
                    charCancel(SIZEX-HORIZONTAL_BAR_SIZE+1,1+(VERTICAL_BAR_SIZE+TOOLALT*numTool)+8,18,0);
                    charCancel(SIZEX-HORIZONTAL_BAR_SIZE+1,1+(VERTICAL_BAR_SIZE+TOOLALT*numTool)+16,18,0);
                    freeTextTool[numTool] = 0;

                    break;
                } else
                if(buffer[iB]=='1' && buffer[iB+1]=='1') //Imposta testo toolbox
                {
                    char numTool = buffer[iB+2], line = buffer[iB+3], text[19];

                    text[18]='\0';
                    for(i=0;i<19;i++)
                    {
                        text[i] = buffer[iB+4+i];
                    }

                    charCancel(1+SIZEX-HORIZONTAL_BAR_SIZE,1+(VERTICAL_BAR_SIZE+TOOLALT*numTool)+(line*8),18,0);
                    int indStruct = getDrawQueueText();
                    drawQueueText[indStruct].screen = NUMWINDOWS;
                    drawQueueText[indStruct].x = (SIZEX-HORIZONTAL_BAR_SIZE)+2;
                    drawQueueText[indStruct].y = (VERTICAL_BAR_SIZE+(TOOLALT*numTool))+(line*8)+2;
                    drawQueueText[indStruct].col = 15;
                    drawQueueText[indStruct].bg = 0;
                    strcpy(drawQueueText[indStruct].text, text);
                    drawQueueText[indStruct].enable = 1;

                    changes[2] = 1;

                    break;
                } else
                if(buffer[iB]=='1' && buffer[iB+1]=='2') //Imposta testo barra superiore
                {
                    char text[129];
                    text[128] = '\0';

                    for(i=0;i<129;i++)
                    {
                        text[i] = buffer[iB+2+i];
                    }

                    charCancel(0,4,128,0);
                    int indStruct = getDrawQueueText();
                    drawQueueText[indStruct].screen = NUMWINDOWS;
                    drawQueueText[indStruct].x = 0;
                    drawQueueText[indStruct].y = 4;
                    drawQueueText[indStruct].col = 15;
                    drawQueueText[indStruct].bg = 0;
                    strcpy(drawQueueText[indStruct].text, text);
                    drawQueueText[indStruct].enable = 1;

                    changes[2] = 1;

                    break;
                } else
                if(buffer[iB]=='1' && buffer[iB+1]=='3') //Ottiene spazio di buckup
                {
                    char pres = 1;
                    for(i=0;i<BUCKUPSCR;i++)if(freebuck[i]==0)
                    {
                        changes[6]=(i+1);

                        snprintf(buffersend,10,"%d\0",i);
                        send (client, buffersend, strlen(buffersend), 0);

                        pres = 0;
                        break;
                    }

                    if(pres)
                    {
                        snprintf(buffersend,10,"%d\0",-1);
                        send (client, buffersend, strlen(buffersend), 0);
                    }

                    break;

                } else
                if(buffer[iB]=='1' && buffer[iB+1]=='4') //Restore e libera
                {
                    char idbuck = buffer[iB+2];

                    //Esecuzione restore
                    changes[7] = (idbuck+1);

                    free(buck[i]);
                    freebuck[idbuck] = 0;

                    break;
                }
                else break;

                if(buffer[iB]=='\0' || iB>1024) break;
            }

        }
        else
        {
            perror (buffer);
        }

        close (client);
	}

	//	Chisura socket

	close (server);
}

void updateBarApp()
{
    int i;
    for(i=0;i<NUMWINDOWS;i++)
    {
        int xp = x(((SIZEX/6)*(i%6))+24), yp = y((SIZEY-VERTICAL_BAR_DOWN)+((i/6)*(VERTICAL_BAR_DOWN/2))+2), cc=15, bc= 0;

        if(visibleWin==i)
        {
            bc=15;
            cc=0;
        }

        charCancel(xp,yp,18,bc);
        writeText(xp, yp,windowsTitle[i],cc,bc);
    }
    changes[0] = 0;
}

void pastDrawText()
{
    changes[2] = 0;
    returnMouseToBack();

    int i;
    for(i=0;i<QUEUELENGHT;i++)if(drawQueueText[i].enable == 1 && (drawQueueText[i].screen == visibleWin || drawQueueText[i].screen == NUMWINDOWS))
    {
        writeText(x(drawQueueText[i].x),y(drawQueueText[i].y),drawQueueText[i].text,drawQueueText[i].col,drawQueueText[i].bg);
        drawQueueText[i].enable = 0;
    }

    saveMouseOmbra(mouseX,mouseY);
    changes[1]=1;
}

void pastDrawLine()
{
    changes[3] = 0;
    returnMouseToBack();

    int i;
    for(i=0;i<QUEUELENGHT;i++)if(drawQueueLine[i].enable == 1 && drawQueueLine[i].screen == visibleWin)
    {
        vga_setcolor(drawQueueLine[i].col);
        vga_drawline(x(drawQueueLine[i].x1), y(drawQueueLine[i].y1+VERTICAL_BAR_SIZE), x(drawQueueLine[i].x2), y(drawQueueLine[i].y2+VERTICAL_BAR_SIZE));

        drawQueueLine[i].enable = 0;
    }

    saveMouseOmbra(mouseX,mouseY);
    changes[1]=1;
}

void pastDrawPoint()
{
    changes[4] = 0;
    returnMouseToBack();

    int i;
    for(i=0;i<QUEUELENGHT*2;i++)if(drawQueuePoint[i].enable == 1 && drawQueuePoint[i].screen == visibleWin)
    {
        vga_setcolor(drawQueuePoint[i].col);
        vga_drawpixel(x(drawQueuePoint[i].x),y(drawQueuePoint[i].y+VERTICAL_BAR_SIZE));

        drawQueuePoint[i].enable = 0;
    }

    saveMouseOmbra(mouseX,mouseY);
    changes[1]=1;
}

void sendByte(char toSend[6])
{
	int SPORT = 24000+(visibleWin+1);			// Porta TCP Server
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
		send (server,toSend,7,0);
	}
	else
	{
		//perror (bufferRet);
	}

	//	Chiusura socket
	close(server);
}


void restoreScreen()
{
    returnMouseToBack();

    int xp, yp;
    for(yp=0;yp<SIZEY-(VERTICAL_BAR_SIZE*2);yp++)for(xp=0;xp<SIZEX-HORIZONTAL_BAR_SIZE;xp++)
    {
        vga_setcolor(pixelWin[visibleWin][xp][yp]);
        vga_drawpixel(xp,yp+VERTICAL_BAR_SIZE);
    }

    saveMouseOmbra(mouseX,mouseY);
}

void buckupScreen()
{
    returnMouseToBack();

    int xp, yp;
    for(yp=0;yp<SIZEY-(VERTICAL_BAR_SIZE*2);yp++)for(xp=0;xp<SIZEX-HORIZONTAL_BAR_SIZE;xp++)
    {
        pixelWin[visibleWin][xp][yp] = vga_getpixel(xp,yp+VERTICAL_BAR_SIZE);
    }

    saveMouseOmbra(mouseX,mouseY);
}

void changeScreenTo(char display)
{
    if(windowsBool[display]==1)
    {
        buckupScreen();
        visibleWin = display;
        restoreScreen();
        changes[0]=1;
        firstDraw();

        changes[0] = 1;
        changes[2] = 1;
        changes[3] = 1;
        changes[4] = 1;
        changes[5] = 0;
    }
}

void Buckup()
{
    char *bck, i = changes[6]-1;
    buck[i] = (char *)malloc(winx*winy*sizeof(char));
    bck = buck[i];
    freebuck[i]=1;

    //Esecuzione buckup
    int xp, yp;
    for(yp=0;yp<winy;yp++)for(xp=0;xp<winx;xp++)
    {
        *bck = vga_getpixel(xp,yp+VERTICAL_BAR_SIZE);
        bck++;
    }

    changes[6]=0;
}

void Restore()
{
    char *bck,i = changes[7]-1;
    bck = buck[i];
    int xp, yp;
    for(yp=0;yp<winy;yp++)for(xp=0;xp<winx;xp++)
    {
        vga_setcolor(*bck);
        vga_drawpixel(xp,yp+VERTICAL_BAR_SIZE);
        bck++;
    }

    changes[7]=0;
}


