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
    const short* getDepthMap();
    const image3D getImageData3D();



    bool uploadImage(QString path);
    bool calculateDepthMap(int threshold);


private:
    short* m_pTiefenkarte;
    //short* m_pImageData;
    image3D im3D;
};

#endif // APPLICATIONDATA_H
