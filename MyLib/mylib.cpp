#include "mylib.h"
#include <QFile>
#include <cmath>
// Autor: Hossein Omid Beiki

MyLib::MyLib()
{
}

int MyLib::windowing(int HU_value, int startValue, int windowWidth, int& iGrauwert){
    //return: 0 if ok.-1 if HU_value is out of range. -2 if windowing parameters are out of range
    int error_stat;
    if (HU_value > 3072 || HU_value < -1024){
        error_stat = -1;
    }
    else if (windowWidth<1 || windowWidth>4096) {
        error_stat = -2;
    }
    else{
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

int MyLib::getSlice(const image3D& image, const Reconstruction& param, image2D& im2D){
    int error_stat = 0;
    //im2D = new image2D(512,512);
    //param.
    //try {
        for (int i = 0; i <= im2D.width; i++) {
            for (int j = 0; j < im2D.height; j++){
                // normalize param.xdir and param.ydir vectors
                double xdirVecLength = 1;
                double ydirVecLength = 1;
                double dx = (i-im2D.width/2)*param.xdir.x/xdirVecLength + (j-im2D.height/2)*param.ydir.x/ydirVecLength;
                double dy = (i-im2D.width/2)*param.xdir.y/xdirVecLength + (j-im2D.height/2)*param.ydir.y/ydirVecLength;
                double dz = (i-im2D.width/2)*param.xdir.z/xdirVecLength + (j-im2D.height/2)*param.ydir.z/ydirVecLength;

                int x = (int) dx;
                int y = (int) dy;
                int z = (int) dz;
                int iGrayVal;
                int iIndex = x+ y*image.height  + z*image.height*image.slices;

                if (x>=0 && x<image.width && y>=0 && y<image.height && z>=0 && z<image.slices){//&& iIndex>=0 && iIndex< image.width*image.height*image.slices){
                    int iHuVal = image.pImage[iIndex];
                    iGrayVal = iHuVal;
                    //int error_stat = MyLib::windowing(iHuVal, startValue, windowWidth, iGrayVal);
                }
                else{
                    iGrayVal = 0;
                }
                im2D.pImage[i+ j*image.height] = iGrayVal;
            }
        }

    //} catch () {

    //}
    return error_stat;
}

bool MyLib::calc3Dreflection(const image2D* im2D, image2D& im2DOut){
    for (int i = 1; i < im2D->width - 1 ; i++){
        for (int j = 1; j < im2D->height - 1 ; j++){
            int sx = 2;
            int sy = 2;
            int leftXNeighbor  = im2D->pImage[j * 512 + i-1];
            int rightXNeighbor = im2D->pImage[j * 512 + i+1];
            int leftYNeighbor  = im2D->pImage[(j-1) * 512 + i];
            int rightYNeighbor = im2D->pImage[(j+1) * 512 + i];
            int Tx = leftXNeighbor - rightXNeighbor;
            int Ty = leftYNeighbor - rightYNeighbor ;
            int iReflection = 255* (sx*sy) / std::pow(std::pow(sy*Tx,2) + std::pow(sx*Ty,2) + std::pow(sx*sy,2),0.5) ;
            im2DOut.pImage[i + j*im2D->width] = iReflection;
            //imageOut.setPixel(i,j,qRgb(iReflection, iReflection, iReflection));
        }
    }
}
