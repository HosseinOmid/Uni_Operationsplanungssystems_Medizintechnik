#ifndef MYLIB_H
#define MYLIB_H
#include "MyLib_global.h"
#include <QFile>
#include <cmath>
//#include "Eigen/Core"
// Autor: Hossein Omid Beiki

/**
 * @brief The image3D struct stores the 3D image data and all of its parameters
 */
struct image3D{
    int width;
    int height;
    int slices;
    double pixelSpacingXY;
    double pixelSpacingZ;
    short* pImage;
};

/**
 * @brief The image2D struct stores the 2D image data and its parameters
 */
struct image2D{
public:
    image2D(int w, int h){
        pImage = new short[w*h];
        width = w;
        height = h;
        memset(pImage, 0, w*h*sizeof(short));
    }
    ~image2D(){
        delete pImage;
    }
    int width;
    int height;
    short* pImage;
};
struct vector{
    double x;
    double y;
    double z;
};

struct Reconstruction{
  //Eigen::Vector3d pos;
  //Eigen::Vector3d xdir;
  //Eigen::Vector3d ydir;
  vector pos;
  vector xdir;
  vector ydir;
  double scale;
};

class MYLIB_EXPORT MyLib{
public:
    MyLib();
    /**
      * @brief windowing calculates a windowed pixel
      * @param HU_value HU value to be windowed
      * @param startValue Start value for windowing [-1024..3072]
      * @param windowWidth Width value for windowing [1..4096]
      * @return  0 if ok.-1 if HU_value is out of range. -2 if windowing parameters are out of range
      */
    static int windowing(int HU_value, int startValue, int windowWidth, int& iGrauwert);
    /**
     * @brief getSlice calculates an arbitrary slice
     * @param image HU original dataset
     * @param param reconstruction parameters
     * @param im2D reconstructed 2d image
     * @return  0 if ok. -1 if input image is incorrect. -2 if output im2D is incorrect.
     */
    static int getSlice(const image3D& image, const Reconstruction& param, image2D& im2D);

    /**
     * @brief calc3Dreflection
     * @param im2D input 2d image
     * @param im2DOut output 2d image
     * @return true if ok. false if not.
     */
    static bool calc3Dreflection(const image2D* im2D, image2D& im2DOut);
};

#endif // MYLIB_H
