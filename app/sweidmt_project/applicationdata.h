#ifndef APPLIKATIONDATA_H
#define APPLIKATIONDATA_H
#include <cmath>
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>
#include <cmath>
#include <mylib.h>

class ApplicationData
{
public:
    ApplicationData();
    ~ApplicationData();
    //const short* getImage();
    const image2D* getDepthMap();
    const image2D* getDepthMapFront();
    const image3D getImageData3D();


    /**
     * @brief ApplicationData::uploadImage reads and stores the data
     * @param path is the path of the file to be read
     * @return true if ok. false if error
     */
    bool uploadImage(QString path);
    /**
     * @brief calculateDepthMap
     * @param threshold
     * @return true if ok. false if error
     */
    bool calculateDepthMap(int threshold);
    /**
     * @brief calculateDepthMapFront
     * @param threshold
     * @return true if ok. false if error
     */
    bool calculateDepthMapFront(int threshold);

private:
    short* m_pTiefenkarte;
    short* m_pTiefenkarteFront;
    //short* m_pImageData;
    image3D im3D;
    image2D* m_depthMap;
    image2D* m_depthMapFront;

    const int maxHUVal = 3072;
    const int minHUVal = -1024;

};

#endif // APPLICATIONDATA_H
