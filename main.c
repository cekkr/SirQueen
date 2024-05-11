#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vga.h>
#include <vgagl.h>
#include <pthread.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vgamouse.h>

//Funzioni SirQueen
#include "config.c"
#include "resolution.c"
#include "font.c"
#include "mouse.c"
#include "fun.c"

//Thread application
pthread_t threads;

void update()
{
    ftime(&tp);

    if(ultAggTime != tp.millitm && tp.millitm % 40)
    {
        mouse_update(); //Aggiornamento posizione mouse

        //Aggiornamento mouse
        mouseX = mousePropX(mouse_getx());
        mouseY = mousePropY(mouse_gety());
        displayMouse(mouseX,mouseY); //Disegno mouse

        vga_setcolor(4); //Impostazione colore

        ultAggTime = tp.millitm;

        if(changes[0]==1)updateBarApp(); //Aggiornamento barra applicazioni
    }

    if(changes[3]==1) pastDrawLine();
    if(changes[4]==1) pastDrawPoint();
    if(changes[2]==1) pastDrawText();
    if(changes[5]>0) changeScreenTo((changes[5]-1));
    if(changes[6]>0) Buckup();
    if(changes[7]>0) Restore();

}

int main(void)
{
    int r = fork();

	if (r == -1) {
		/* errore */
		printf("Errore nella fork!\n");

	} else if(r == 0) {
	    //execlp("applib/hello", "applib/hello", "-a", NULL);
	    execlp("mono", "mono", "applib/Hello.exe", NULL);
	}
	else
	{
        vga_init();
        vga_setmode(G1024x768x256);

        vga_setmousesupport(1);

        //Inizializzazione finestre
        memset(windowsBool,0,NUMWINDOWS);
        memset(changes,0,CHANGESVAR);
        memset(freeTextTool,0,NUMTOOL);
        memset(freebuck,0,BUCKUPSCR);

        //Impostazione range mouse
        mouse_setyrange(numProp(VERTICAL_BAR_SIZE+8,SIZEX,32767), numProp(SIZEX-VERTICAL_BAR_DOWN-20,SIZEX,32767));
        mouse_setxrange(0,32500);

        //Thread TCP
        pthread_create(&threads,NULL,tcpExc,(void*) 0);

        //Avvio esecuzione
        firstDraw();

        unsigned char ch = 0, cmouse = 0, pch[5]={0,0,0,0,0}, runfast = 1;
        unsigned int ultMouseClick;
        while(runfast)
        {
            if(ch==97) vga_setpage(4);
            update();

            ch = vga_getkey();
            cmouse = mouse_getbutton();

            if(ch > 0)
            {
                char sendTitle[20];

                char argt[2] = {1,ch}, loltext[10];
                sendByte(argt);

                shifPch(pch);
                pch[4] = ch;

                if(pch[1]==27&&pch[2]==91&&pch[3]==91&&pch[4]==65)if(windowsBool[0]==1) changeScreenTo(0);
                if(pch[1]==27&&pch[2]==91&&pch[3]==91&&pch[4]==66)if(windowsBool[1]==1) changeScreenTo(1);
                if(pch[1]==27&&pch[2]==91&&pch[3]==91&&pch[4]==67)if(windowsBool[2]==1) changeScreenTo(2);
                if(pch[1]==27&&pch[2]==91&&pch[3]==91&&pch[4]==68)if(windowsBool[3]==1) changeScreenTo(3);
                if(pch[1]==27&&pch[2]==91&&pch[3]==91&&pch[4]==69)if(windowsBool[4]==1) changeScreenTo(4);
                if(pch[0]==27&&pch[1]==91&&pch[2]==49&&pch[3]==55&&pch[4]==126)if(windowsBool[5]==1) changeScreenTo(5);
                if(pch[0]==27&&pch[1]==91&&pch[2]==49&&pch[3]==56&&pch[4]==126)if(windowsBool[6]==1) changeScreenTo(6);
                if(pch[0]==27&&pch[1]==91&&pch[2]==49&&pch[3]==57&&pch[4]==126)if(windowsBool[7]==1) changeScreenTo(7);
                if(pch[0]==27&&pch[1]==91&&pch[2]==50&&pch[3]==48&&pch[4]==126)if(windowsBool[8]==1) changeScreenTo(8);
                if(pch[0]==27&&pch[1]==91&&pch[2]==50&&pch[3]==49&&pch[4]==126)if(windowsBool[9]==1) changeScreenTo(9);
                if(pch[0]==27&&pch[1]==91&&pch[2]==50&&pch[3]==51&&pch[4]==126)if(windowsBool[10]==1) changeScreenTo(10);
                if(pch[0]==27&&pch[1]==91&&pch[2]==50&&pch[3]==52&&pch[4]==126)if(windowsBool[11]==1) changeScreenTo(11);
                printf("%d.%d.%d.%d\n",pch[0],pch[1],pch[2],pch[3]);
            }
            else if(pch[4]==27) runfast = 0;

            if(cmouse==MOUSE_LEFTBUTTON && (tp.millitm-ultMouseClick)>LIMITFORCLICK)
            {
                ultMouseClick = tp.millitm-ultMouseClick;
                char typeClick = 0; //Click normale
                if(ultMouseClick<MOUSECLICKTIME) typeClick = 1; //Click a tempo prolungato
                if(ultMouseClick>MOUSECLICKTIME&&ultMouseClick<DOUBLECLICKTIME) typeClick = 2; //Doppio click

                if(mouseX<(SIZEX-HORIZONTAL_BAR_SIZE) && mouseY>VERTICAL_BAR_SIZE && mouseY<(SIZEY-VERTICAL_BAR_SIZE))
                {
                    mousePrecClick[0] = mouseX;
                    mousePrecClick[1] = mouseY;

                    char xp[2], yp[2];
                    splitChar(mouseX, xp);
                    splitChar((mouseY-VERTICAL_BAR_SIZE), yp);

                    char argt[7] = {2,cmouse,xp[0],xp[1],yp[0],yp[1],typeClick};
                    sendByte(argt);
                }

                ultMouseClick = tp.millitm;
            }
        }

        execlp("killall", "killall", "mono", NULL);
        vga_setmode(TEXT);
	}

    return EXIT_SUCCESS;
}

