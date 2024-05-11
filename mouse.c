char mouse_default[] = {
2,2,2,2,2,2,2,2,
2,1,1,1,1,1,2,0,
2,1,1,1,1,2,0,0,
2,1,1,1,2,0,0,0,
2,1,1,2,1,2,0,0,
2,1,2,0,2,1,2,0,
2,2,0,0,0,2,1,2,
2,0,0,0,0,0,2,2,
};

char mouse_ombra[8][8], mouse_first = 0; //vga_getpixel
unsigned int mouse_ultx, mouse_ulty;

void returnMouseToBack()
{
    int px, py;
    for(py=0;py<8;py++)for(px=0;px<8;px++)
    {
        vga_setcolor(mouse_ombra[px][py]);
        vga_drawpixel(px+mouse_ultx,py+mouse_ulty);
    }
}

void saveMouseOmbra(int x, int y)
{
    int px, py;
    for(py=0;py<8;py++)for(px=0;px<8;px++) mouse_ombra[px][py] = vga_getpixel(px+x,py+y);
}

void displayMouse(int x, int y)
{
    vga_setcolor(15);

    int px, py;

    if(mouse_ultx!=x || mouse_ulty!=y || changes[1]==1)
    {
        if(mouse_first==1)returnMouseToBack();
        saveMouseOmbra(x,y);

        vga_setcolor(15);
        for(py=0;py<8;py++)for(px=0;px<8;px++)
        {
            if(mouse_default[(py*8)+px]==1)vga_drawpixel(x+px,y+py); //Bianco
            if(mouse_default[(py*8)+px]==2) //Nero
            {
                vga_setcolor(7);
                vga_drawpixel(x+px,y+py);
                vga_setcolor(15);
            }
        }

        mouse_first = 1;
        mouse_ultx = x;
        mouse_ulty = y;

        changes[1]=0;
    }


}

