//Conversione e rapporto
int x (int xprop)
{
    return (int)(((float)xprop/1024.f)*SIZEX);
}

int y (int yprop)
{
    return (int)(((float)yprop/768.f)*SIZEY);
}

int numProp (float num, float x1, float x2)
{
    return (num/x1) *x2;
}

int mousePropX(int val)
{
    return numProp(val,32767, SIZEX);
}

int mousePropY(int val)
{
    return numProp(val,32767, SIZEY);
}
