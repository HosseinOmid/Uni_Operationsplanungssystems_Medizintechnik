#include "applicationdata.h"
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>

ApplicationData::ApplicationData()
{
    im3D.pImage = new short[512*512*512];
    //m_pImageData = new short[512*512*130];
    m_pTiefenkarte = new short[512*512];
}
ApplicationData::~ApplicationData()
{
    //delete[] m_pImageData;
    delete[] m_pTiefenkarte;
}

//const short* ApplicationData::getImage(){
//    return m_pImageData;
//}

const short* ApplicationData::getDepthMap(){
    return m_pTiefenkarte;
}

const image3D ApplicationData::getImageData3D(){
    return im3D;
}


bool ApplicationData::uploadImage(QString path)
{
    //m_pImageData = new short[512*512*130];
    //Die Funktionen uploadImage() und calculateDepthMap() sollen bei einem Fehlerfall einen false-Wert zurückliefern, ansonsten true.
    QFile dataFile(path);
    bool bFileOpen = dataFile.open(QIODevice::ReadOnly);
    if (!bFileOpen)
    {
        return bFileOpen;
    }

    // Datei lesen
    int iNumberBytesRead = dataFile.read((char*)im3D.pImage, 512*512*512*sizeof(short));
    //dataFile.read((char*)im3D.pImage, 512*512*130*sizeof(short));
    im3D.width = 512;
    im3D.height = 512;
    im3D.slices = 512;
    //im3D.pImage = m_pImageData;
    im3D.pixelSpacingXY= 1.2695;
    im3D.pixelSpacingZ = 2.0000;
    //m_pImageData = x;

    dataFile.close();

    // zum vergleich der Dateilänge mit der eingelesenen Länge
    int iFileSize = dataFile.size();
    if (iFileSize == iNumberBytesRead)
    {
        return true;}
    else {
        return false;
    }
}

bool ApplicationData::calculateDepthMap(int threshold)
{
    //return: 0 if ok.-1 if threshold is out of range.
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
                m_pTiefenkarte[j * 512 + i] = iTiefe;
            }
        }
    }
    return error_stat;
}
