#include <stdio.h>
#include <time.h>
#include <string.h>
#include "queen.h"

int writeFontArrive = 0, writeFontLine = 32;

void onEvent(char typeEvent, int arg[3])
{
    /*if(typeEvent==1)
    {
        if(arg[0]==12)
        {
            writeFontArrive -= 8;
            if(writeFontArrive<0) writeFontArrive = 0;
        }
        else
        {
            char arriv[2] = {arg[0],'\0'}, readInt[6];
            qrWrite(writeFontArrive,writeFontLine,arriv,15,0);

            writeFontArrive += 8;
        }

    }*/

    if(typeEvent==2)
    {
        /*int area = areaGet(arg[0],arg[1]);

        if(area!=-1)
        {
            if(strcmp(areaList[area].name,"click")==0) qrWrite(0,68,"Hai clicckato\0",15,0);
        }*/

        qrPoint(arg[0],arg[1],1);


    }

}

void sleepFun()
{
    time_t start;
    time_t current;

    time(&start);
    while(difftime(current,start) < 5.0)time(&current);
}

int main(int argc, char *argv[])
 {
    if(argc>1)
    {
        sleepFun();
    }

    initQueen();
    quSetTitle("SirQueen Start\0");

    //Presentazione
    qrWrite(0,0,"Benvenuto in SirQueen\0",0,15);
    qrWrite(0,16,"Cos'hai scritto:\0",15,0);

    //Aggiungiamo una nuova area
    qrWrite(0,60,"Ciao\0",14,0);
    areaAdd("click\0",0,60,32,68);
    drawRect(10,200,30,300,2);
    drawEmptyRect(10,200,30,300,3);

    //Disegnamo una linea
    qrLine(0,100,100,200,1);
    int i;
    for(i=0;i<10;i++) qrPoint(i,200,1);


    while(quHereYouAre()>=0) sleepFun();

    closeQueen();
    return 0;
 }
