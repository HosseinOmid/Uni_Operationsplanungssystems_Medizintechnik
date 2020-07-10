#include "mylib.h"
#include <QFile>
#include <cmath>
// Autor: Hossein Omid Beiki

MyLib::MyLib(){
}
int MyLib::windowing(int HU_value, int startValue, int windowWidth, int& iGrauwert){
    //return: 0 if ok.-1 if HU_value is out of range. -2 if windowing parameters are out of range
    int error_stat;
    if (HU_value > 3072 || HU_value < -1024){
        error_stat = -1;
    }
    else if (windowWidth < 1 || windowWidth > 4096){
        error_stat = -2;
    }
    else{
        error_stat = 0;
        //calculate windowing
        if (HU_value < startValue)
            iGrauwert = 0;
        if (HU_value > startValue+windowWidth)
            iGrauwert = 255;
        if (HU_value >= startValue  && HU_value <= startValue+windowWidth)
            iGrauwert = 255.0 / windowWidth * (HU_value - startValue);
    }
    return error_stat;
}
int MyLib::getSlice(const image3D& image, const Reconstruction& param, image2D& im2D){
    int error_stat = 0; // returns 0 if ok. -1 if input image is incorrect. -2 if output im2D is incorrect.
    if (false){ // if input image is incorrect
        error_stat = -1;
        return error_stat;
    }
    try {
        for (float i = 0.0; i < im2D.width; i++) {
            for (float j = 0.0; j < im2D.height; j++){
                // normalize the param.xdir and param.ydir vectors
                double xdirVecLength = pow(pow(param.xdir.x,2) + pow(param.xdir.y,2) + pow(param.xdir.z,2),0.5);
                double ydirVecLength = pow(pow(param.ydir.x,2) + pow(param.ydir.y,2) + pow(param.ydir.z,2),0.5);
                // calculate the coordinates x,y,z of pixel(i,j) of the slice in global coordinates system
                // the slice is described by two orthogonal vectors xdir & ydir
                double dx = param.pos.x + (i - im2D.width/2)*param.xdir.x/xdirVecLength + (j - im2D.height/2)*param.ydir.x/ydirVecLength;
                double dy = param.pos.y + (i - im2D.width/2)*param.xdir.y/xdirVecLength + (j - im2D.height/2)*param.ydir.y/ydirVecLength;
                double dz = param.pos.z + (i - im2D.width/2)*param.xdir.z/xdirVecLength + (j - im2D.height/2)*param.ydir.z/ydirVecLength;
                // round the x,y,z coordinates TODO: CONSIDER SCALE!!
                int x = (int) (dx + .5);
                int y = (int) (dy + .5);
                int z = (int) ((dz/image.pixelSpacingZ*image.pixelSpacingXY )+.5);
                // find the corresponding voxel index
                int iIndex3DImgae = x+ y*image.height  + z*image.height*image.slices;
                // check if the voxel exists / the voxel index is inside the image boundaries
                int iHuVal = 0;
                if (x>=0 && x<image.width && y>=0 && y<image.height && z>=0 && z<image.slices){
                    iHuVal = image.pImage[iIndex3DImgae];
                }
                int iIndex2DImage = (int)i+ (int)j*im2D.height;
                im2D.pImage[iIndex2DImage] = iHuVal;
            }
        }

    } catch (...) {
        error_stat = -2;
    }
    return error_stat;
}
bool MyLib::calc3Dreflection(const image2D* im2D, image2D& im2DOut){
    int error_stat = true; // returns true if ok. false if not.
    try{
        int sx = 2;
        int sy = 2;
        for (int i = 1; i < im2D->width - 1 ; i++){
            for (int j = 1; j < im2D->height - 1 ; j++){
                int leftXNeighbor  = im2D->pImage[j * im2D->height + i-1];
                int rightXNeighbor = im2D->pImage[j * im2D->height + i+1];
                int leftYNeighbor  = im2D->pImage[(j-1) * im2D->height + i];
                int rightYNeighbor = im2D->pImage[(j+1) * im2D->height + i];
                int Tx = leftXNeighbor - rightXNeighbor;
                int Ty = leftYNeighbor - rightYNeighbor ;
                int iReflection = 255* (sx*sy) / std::pow(std::pow(sy*Tx,2) + std::pow(sx*Ty,2) + std::pow(sx*sy,2),0.5) ;
                im2DOut.pImage[i + j*im2D->width] = iReflection;
            }
        }
    }
    catch (...) {
        error_stat = false;
    }
    return error_stat;
}
