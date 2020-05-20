#ifndef APPLIKATIONDATA_H
#define APPLIKATIONDATA_H
#include <QFile>


class ApplicationData
{
public:
    ApplicationData();
    const short* getImage();
    const short* getDepthMap();

    bool uploadImage(QString path);
    bool calculateDepthMap(int threshold);


private:
    short* m_pTiefenkarte;
    short* m_pImageData;
};

#endif // APPLICATIONDATA_H
