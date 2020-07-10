#ifndef APPLIKATIONDATA_H
#define APPLIKATIONDATA_H
#include <cmath>
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>
#include <cmath>
#include <mylib.h>

class ApplicationData{
public:
    ApplicationData();
    ~ApplicationData();
    //const short* getImage();
    const image2D* getDepthMapXY();
    const image2D* getDepthMapXZ();
    const image3D getImageData3D();

    /**
     * @brief ApplicationData::uploadImage reads and stores the data
     * @param path is the path of the file to be read
     * @return true if ok. false if error
     */
    bool uploadImage(QString path);
    bool calculateDepthMapXY(int threshold);
    bool calculateDepthMapXZ(int threshold);
private:
    image3D im3D;
    image2D* m_depthMapXY;
    image2D* m_depthMapXZ;

    const int maxHUVal = 3072;
    const int minHUVal = -1024;
};
#endif // APPLICATIONDATA_H
