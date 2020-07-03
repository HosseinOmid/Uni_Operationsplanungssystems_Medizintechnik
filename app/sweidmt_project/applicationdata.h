#ifndef APPLIKATIONDATA_H
#define APPLIKATIONDATA_H
#include <cmath>
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>
#include <cmath>
#include <mylib.h>
#include <qmouseevent>

class ApplicationData
{
public:
    ApplicationData();
    ~ApplicationData();
    //const short* getImage();
    const image2D* getDepthMap();
    const image2D* getDepthMapFront();
    const image3D getImageData3D();



    bool uploadImage(QString path);
    bool calculateDepthMap(int threshold);
    bool calculateDepthMapFront(int threshold);

private:
    short* m_pTiefenkarte;
    short* m_pTiefenkarteFront;
    //short* m_pImageData;
    image3D im3D;
    image2D* m_depthMap;
    image2D* m_depthMapFront;
};

#endif // APPLICATIONDATA_H
