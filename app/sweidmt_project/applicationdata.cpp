#include "applicationdata.h"
#include <QFile>

ApplicationData::ApplicationData()
{
    m_pImageData = new short[512*512*130];
    m_pTiefenkarte = new short[512*512];
}

const short* ApplicationData::getImage(){
    return m_pImageData;
}

const short* ApplicationData::getDepthMap(){
    return m_pTiefenkarte;
}


bool ApplicationData::uploadImage(QString path)
{
    //m_pImageData = new short[512*512*130];
    //Die Funktionen uploadImage() und calculateDepthMap() sollen bei einem Fehlerfall einen false-Wert zurückliefern, ansonsten true.
    QFile dataFile(path);
    bool bFileOpen = dataFile.open(QIODevice::ReadOnly);
    if (!bFileOpen)
    {
        //QMessageBox::critical(this, "ACHTUNG", "Datei konnte nicht geöffnet werden");
        return bFileOpen;
    }

    // Datei lesen
    short* x;
    x = new short[512*512*130];
    dataFile.read((char*)x, 512*512*130*sizeof(short));
    m_pImageData = x;

    dataFile.close();
    return true;
}

bool ApplicationData::calculateDepthMap(int threshold)
{
    int iTiefe = 129;
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++){
            for (int iLayer = 129; iLayer>= 0; iLayer--){
                int index = j * 512 + i + 512*512*iLayer ;
                int HU_value = m_pImageData[index];
                if(HU_value >threshold){
                    iTiefe = 129 - iLayer;
                    break;
                }
            }
            m_pTiefenkarte[j * 512 + i] = iTiefe;
        }
    }
    return true;
}
