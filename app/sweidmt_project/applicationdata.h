#ifndef APPLIKATIONDATA_H
#define APPLIKATIONDATA_H
#include <cmath>
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>
#include <cmath>

class ApplicationData
{
public:
    ApplicationData();
    ~ApplicationData();
    const short* getImage();
    const short* getDepthMap();

    bool uploadImage(QString path);
    bool calculateDepthMap(int threshold);


private:
    short* m_pTiefenkarte;
    short* m_pImageData;
};

#endif // APPLICATIONDATA_H
