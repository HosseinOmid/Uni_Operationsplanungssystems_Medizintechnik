#include "applicationdata.h"
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>
// Autor: Hossein Omid Beiki
ApplicationData::ApplicationData(){    
    im3D.width = 512;
    im3D.height = 512;
    im3D.slices = 256;
    im3D.pImage = new short[im3D.width*im3D.height*im3D.slices];
    im3D.pixelSpacingXY= 1.2695;
    im3D.pixelSpacingZ = 2.0000;
    m_depthMapXY = new image2D(im3D.width, im3D.height);
    m_depthMapXZ = new image2D(im3D.width, im3D.slices);
}
ApplicationData::~ApplicationData(){    
    //delete[] m_depthMap;
    //delete[] m_depthMapFront;
}
const image2D* ApplicationData::getDepthMapXY(){
    return m_depthMapXY;
}
const image2D* ApplicationData::getDepthMapXZ(){
    return m_depthMapXZ;
}
const image3D ApplicationData::getImageData3D(){
    return im3D;
}
bool ApplicationData::uploadImage(QString path){
    //Die Funktionen uploadImage() und calculateDepthMap() sollen bei einem Fehlerfall einen false-Wert zurückliefern, ansonsten true.
    bool error_stat = false;
    QFile dataFile(path);
    // open the data
    bool bFileOpen = dataFile.open(QIODevice::ReadOnly);
    if (bFileOpen)
    {    
        // read the data
        int iNumberBytesRead = dataFile.read((char*)im3D.pImage, im3D.width *im3D.height *im3D.slices *sizeof(short));
        // close the data
        dataFile.close();

        // compare the size of the data with the number of bytes which could be read
        int iFileSize = dataFile.size();
        error_stat = iFileSize == iNumberBytesRead;
    }
    return error_stat;

}
bool ApplicationData::calculateDepthMapXY(int threshold){
    //return: true if ok.false if threshold is out of range or error.
    bool error_stat = false;
    if (threshold <= maxHUVal && threshold >= minHUVal){
        error_stat = true;
        int iHUValue;
        int iIndex;
        int iDepth;
        for (int i = 0; i < im3D.width; i++) {
            for (int j = 0; j < im3D.height; j++){
                iDepth = im3D.slices - 1;
                for (int iLayer = im3D.slices - 1; iLayer>= 0; iLayer--){
                    iIndex = i + j * im3D.height + im3D.width * im3D.height * iLayer;
                    iHUValue = im3D.pImage[iIndex];
                    if(iHUValue > threshold){
                        iDepth = im3D.slices - iLayer;
                        break;
                    }
                }
                m_depthMapXY->pImage[j * m_depthMapXY->width + i] = iDepth;
            }
        }
    }
    return error_stat;
}
bool ApplicationData::calculateDepthMapXZ(int threshold){
    //return: 0 if ok.-1 if threshold is out of range.
    bool error_stat = false;
    if (threshold <= maxHUVal && threshold >= minHUVal){
        error_stat = true;
        int iHUValue;
        int iIndex;
        int iDepth;
        for (int i = 0; i < im3D.width; i++) {
           for (int iLayer = 0; iLayer < im3D.slices; iLayer++){
               iDepth = im3D.height - 1;
               for (int j = 0; j <= im3D.height; j++){
                   iIndex = i + j * im3D.height + im3D.width * im3D.height * iLayer;
                   iHUValue = im3D.pImage[iIndex];
                   if(iHUValue > threshold){
                        iDepth = j;
                        break;
                   }
                }
                m_depthMapXZ->pImage[iLayer * m_depthMapXZ->width + i] = iDepth;
            }
        }
    }
    return error_stat;
}
