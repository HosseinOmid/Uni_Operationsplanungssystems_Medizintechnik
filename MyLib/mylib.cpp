#include "mylib.h"

MyLib::MyLib()
{
}

int MyLib::windowing(int HU_value, int startValue, int windowWidth, int& iGrauwert)
{
    //return: 0 if ok.-1 if HU_value is out of range. -2 if windowing parameters are out of range
    int error_stat;
    if (HU_value>3072 || HU_value< -1024)
    {
        error_stat = -1;
    }
    else if (windowWidth<1 || windowWidth>4096) {
        error_stat = -2;
    }
    else
    {
        error_stat = 0;
        //Fensterung berechnen
        if (HU_value< startValue)
            iGrauwert = 0;
        if (HU_value> startValue+windowWidth)
            iGrauwert = 255;
        if (HU_value>= startValue  && HU_value<= startValue+windowWidth)
            iGrauwert = 255.0 / windowWidth * (HU_value - startValue);
    }
    return error_stat;
}
