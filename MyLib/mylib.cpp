#include "mylib.h"

MyLib::MyLib()
{
}
int MyLib::windowing(int HU_value, int startValue, int windowWidth)
{
    int iGrauwert = 0;
    //Fensterung berechnen
    if (HU_value< startValue)
        iGrauwert = 0;
    if (HU_value> startValue+windowWidth)
        iGrauwert = 255;
    if (HU_value>= startValue  && HU_value<= startValue+windowWidth)
        iGrauwert = 255.0 / windowWidth * (HU_value - startValue);
    return iGrauwert;
}
