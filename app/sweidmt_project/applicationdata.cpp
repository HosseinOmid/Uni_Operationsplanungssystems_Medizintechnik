#include "applicationdata.h"
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>

ApplicationData::ApplicationData()
{
    im3D.pImage = new short[512*512*512];
    im3D.width = 512;
    im3D.height = 512;
    im3D.slices = 512;
    im3D.pixelSpacingXY= 1.2695;
    im3D.pixelSpacingZ = 2.0000;

    m_depthMap = new image2D(512, 512);
    m_depthMapFront = new image2D(512, 512);

    //m_pTiefenkarte = new short[512*512];
    //m_pTiefenkarteFront = new short[512*512];
}
ApplicationData::~ApplicationData()
{    
    //delete[] m_pImageData;
    //delete[] m_pTiefenkarte;
    //delete[] m_pTiefenkarteFront;
    //delete[] m_depthMap;
    //delete[] m_depthMapFront;
}

const image2D* ApplicationData::getDepthMap(){
    return m_depthMap;
}
const image2D* ApplicationData::getDepthMapFront(){
    return m_depthMapFront;
}
const image3D ApplicationData::getImageData3D(){
    return im3D;
}


bool ApplicationData::uploadImage(QString path)
{
    //Die Funktionen uploadImage() und calculateDepthMap() sollen bei einem Fehlerfall einen false-Wert zurückliefern, ansonsten true.
    bool error_stat = false;
    QFile dataFile(path);
    bool bFileOpen = dataFile.open(QIODevice::ReadOnly);
    if (bFileOpen)
    {    
        // Die Datei lesen
        int iNumberBytesRead = dataFile.read((char*)im3D.pImage, im3D.width *im3D.height *im3D.slices *sizeof(short));
        dataFile.close();

        // zum vergleich der Dateilänge mit der eingelesenen Länge
        int iFileSize = dataFile.size();
        error_stat = iFileSize == iNumberBytesRead;
    }
    return error_stat;

}

bool ApplicationData::calculateDepthMap(int threshold)
{
    //return: 0 if ok.-1 if threshold is out of range or error.
    bool error_stat = false;
    if (threshold<=3072 && threshold>= -1024){
        error_stat = true;
        int iHU_value;
        int iIndex;
        int iTiefe;
        for (int i = 0; i < 512; i++) {
            for (int j = 0; j < 512; j++){
                iTiefe = 511;
                for (int iLayer = 511; iLayer>= 0; iLayer--){
                    iIndex = j * 512 + i + 512*512*iLayer ;
                    iHU_value = im3D.pImage[iIndex];
                    if(iHU_value >threshold){
                        iTiefe = 512 - iLayer;
                        break;
                    }
                }
                m_depthMap->pImage[j * 512 + i] = iTiefe;
            }
        }
    }
    return error_stat;
}
bool ApplicationData::calculateDepthMapFront(int threshold)
{
    //return: 0 if ok.-1 if threshold is out of range.
    bool error_stat = false;
    if (threshold<=3072 && threshold>= -1024){
        error_stat = true;
        int iHU_value;
        int iIndex;
        int iTiefe;
        for (int i = 0; i < 512; i++) {
           for (int iLayer = 0; iLayer < 512; iLayer++){
               iTiefe = 511;
               for (int j = 0; j<= 512; j++){
                   iIndex = j * 512 + i + 512*512*iLayer ;
                   iHU_value = im3D.pImage[iIndex];
                   if(iHU_value >threshold){
                        iTiefe = j;
                        break;
                   }
                }
                m_depthMapFront->pImage[iLayer * 512 + i] = iTiefe;
            }
        }
    }
    return error_stat;
}
