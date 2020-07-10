#include "imageloader.h"
#include "ui_ImageLoader.h"
#include "applicationdata.h"
#include "mylib.h"
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>
#include <cmath>
#include <QMouseEvent>

// constructor ---------------------------------------------
ImageLoader::ImageLoader(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    reco_im2D =  new image2D(512,512);

    countPoint = 1;
    drawLine = false;
    drawDrillTrajectory = false;

    // connect events of GUI with funktions
    ui->setupUi(this);
    connect(ui->pushButton_load3D, SIGNAL(clicked()), this, SLOT(loadData()));
    connect(ui->pushButton_DepthMapXZ, SIGNAL(clicked()), this, SLOT(updateDepthMapXY()));
    connect(ui->pushButton_DepthMapXZ, SIGNAL(clicked()), this, SLOT(updateDepthMapXZ()));
    connect(ui->pushButton_3DXY, SIGNAL(clicked()), this, SLOT(update3DreflectionXY()));
    connect(ui->pushButton_3DXZ, SIGNAL(clicked()), this, SLOT(update3DreflectionXZ()));
    connect(ui->slider_StartValueXY, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingStartXY(int)));
    connect(ui->slider_WindowWidthXY, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingWidthXY(int)));
    connect(ui->slider_LayerNrXY, SIGNAL(valueChanged(int)), this, SLOT(updateLayerNrXY(int)));
    connect(ui->Slider_ThresholdXY, SIGNAL(valueChanged(int)), this, SLOT(updateTresholdXY(int)));
    connect(ui->slider_StartValueXZ, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingStartXZ(int)));
    connect(ui->slider_WindowWidthXZ, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingWidthXZ(int)));
    connect(ui->slider_LayerNrXZ, SIGNAL(valueChanged(int)), this, SLOT(updateLayerNrXZ(int)));
    connect(ui->Slider_ThresholdXZ, SIGNAL(valueChanged(int)), this, SLOT(updateTresholdXZ(int)));
    connect(ui->Slider_ThresholdXZ_2, SIGNAL(valueChanged(int)), this, SLOT(updateTresholdXZ_2(int)));
    connect(ui->Slider_ThresholdXY_2, SIGNAL(valueChanged(int)), this, SLOT(updateTresholdXY_2(int)));
    connect(ui->Slider_SliceDepth, SIGNAL(valueChanged(int)), this, SLOT(updateSliceDepth(int)));

    connect(ui->pushButton_reconstruct, SIGNAL(clicked()), this, SLOT(reconstructSlice()));

    ImageLoader::updateAllSlidernLabels();
}

// destructor ------------------------------------------
ImageLoader::~ImageLoader(){
    delete ui;
    // Speicher wieder freigeben
    // delete m_pData; //brauchen wir das? führt zu Fehler...
}
// creat a connection to application data --------------
void ImageLoader::setData(ApplicationData *pData){
    this->m_pData = pData;
}

void ImageLoader::reconstructSlice(){
    double a = secPoint.x - firstPoint.x;
    double b = secPoint.y - firstPoint.y;
    double c = secPoint.z - firstPoint.z;

    int sliceDistToFirstPointPerc = ui->Slider_SliceDepth->value();

    reco.pos.x = firstPoint.x + a * sliceDistToFirstPointPerc/100;
    reco.pos.y = firstPoint.y + b * sliceDistToFirstPointPerc/100;
    reco.pos.z = firstPoint.z + c * sliceDistToFirstPointPerc/100;

    // equation of a plane: ax + by + cz = d
    // also we know that the point reco.pos is place on the plan, so:
    double d = a*reco.pos.x + b*reco.pos.y + c*reco.pos.z;
    // we want to define to orthogonal vectors which describe our plane
    // there are infit such vectors, since there are infinit lines on a plane
    // so we just define that xdir has no z component, and its component has the length of 1
    // also we define that ydir has x comonent of length 1. Later we can normalize both vectors.
    // consider xdir=(1, y1, 0) ; ydir=(1, y2, z2)
    // xdir and ydir are orthogonal, e.a (xdir.ydir = 0)
    // and both xdir and ydir are placed on the plan and should satisfy the plane equation.
    // so we get xdir and ydir as following:
    reco.xdir.x = 1;
    reco.xdir.y = (-a)/b;
    reco.xdir.z = 0;

    reco.ydir.x = 1;
    reco.ydir.y = b/a;
    reco.ydir.z = (-a - pow(b,2)/a)/c;

    //normalize xdir and ydir
    double xdirLength = pow(pow(reco.xdir.x,2) + pow(reco.xdir.y,2) + pow(reco.xdir.z,2), .5);
    double ydirLength = pow(pow(reco.ydir.x,2) + pow(reco.ydir.y,2) + pow(reco.ydir.z,2), .5);;
    reco.xdir.x /= xdirLength;
    reco.xdir.y /= xdirLength;
    reco.xdir.z /= xdirLength;

    reco.ydir.x /= ydirLength;
    reco.ydir.y /= ydirLength;
    reco.ydir.z /= ydirLength;

    // get the data from the database
    const image3D tmp_im3D = m_pData->getImageData3D();

    //image2D reco_im2D =  image2D(512,512);
    int err_stat = MyLib::getSlice(tmp_im3D, reco, *reco_im2D);
    drawDrillTrajectory = true;
    updateView();
}

void ImageLoader::mousePressEvent(QMouseEvent *event){
    drawDrillTrajectory = false;
    QPoint globalPos;
    globalPos = event->pos();
    QPoint localPosXY1;
    QPoint localPosXY;
    QPoint localPosXZ1;
    QPoint localPosXZ;
    localPosXY1 = ui->tabWidget->mapFromParent(globalPos);
    localPosXY  = ui->label_imageXY->mapFromParent(localPosXY1);
    localPosXY.setY(localPosXY.y()-30);

    localPosXZ1 = ui->tabWidget_2->mapFromParent(globalPos);
    localPosXZ  = ui->label_imageXZ->mapFromParent(localPosXZ1);
    localPosXZ.setY(localPosXZ.y()-30);

    if (ui->label_imageXY->rect().contains(localPosXY)){
        if (countPoint == 1){
            firstPoint.x = localPosXY.x();
            firstPoint.y = localPosXY.y();
            firstPoint.z = ui->slider_LayerNrXY->value();
            countPoint = 2;
            emit LOG("First point got successfully chosen/updated.");
        }
        else if (countPoint == 2){
            secPoint.x = localPosXY.x();
            secPoint.y = localPosXY.y();
            secPoint.z = ui->slider_LayerNrXY->value();
            countPoint = 1;
            drawLine = true;
            emit LOG("Second point got successfully chosen.");
        }
        updateView();
    }
    if (ui->label_imageXZ->rect().contains(localPosXZ)){
        if (drawLine){
            double dist1 = abs(firstPoint.x - localPosXZ.x());
            double dist2 = abs(secPoint.x - localPosXZ.x());
            if (dist1<dist2){
                firstPoint.z = localPosXZ.y();
                emit LOG("First point got successfully updated.");
            }
            else{
                secPoint.z = localPosXZ.y();
                emit LOG("Second point got successfully updated.");
            }
        }
        else{
            firstPoint.z = localPosXZ.y();
        }
        updateView();
    }
}
void ImageLoader::updateView(){
    // get the data from the database
    const image3D tmp_im3D = m_pData->getImageData3D();
    // creat objects of type QImage to visualize it in GUI
    QImage imageXY(tmp_im3D.width,tmp_im3D.height, QImage::Format_RGB32);
    QImage imageXZ(tmp_im3D.width,tmp_im3D.slices, QImage::Format_RGB32);
    // read values from the GUI
    int startValueXY = ui->slider_StartValueXY->value();
    int windowWidthXY = ui->slider_WindowWidthXY->value();
    int layerNrXY = ui->slider_LayerNrXY->value();
    int tresholdXY = ui->Slider_ThresholdXY->value();
    int startValueXZ = ui->slider_StartValueXZ->value();
    int windowWidthXZ = ui->slider_WindowWidthXZ->value();
    int layerNrXZ = ui->slider_LayerNrXZ->value();
    int tresholdXZ = ui->Slider_ThresholdXZ->value();
    // -------------- XY View ----------------------------------------------------
    for (int i = 0; i < tmp_im3D.width; i++){
        for (int j = 0; j < tmp_im3D.height; j++){
            int index = j * tmp_im3D.width + i + tmp_im3D.width*tmp_im3D.height*layerNrXY ;
            int HU_value = tmp_im3D.pImage[index];
            int iGrauwert;
            int error_stat = MyLib::windowing(HU_value, startValueXY, windowWidthXY, iGrauwert);
            imageXY.setPixel(i,j,qRgb(iGrauwert, iGrauwert, iGrauwert));
        }
    }
    // draw a blue line in the XY view which shows the current XZ layer
    for (int i = 0; i < tmp_im3D.width; i++) {
        imageXY.setPixel(i,layerNrXZ,qRgb(0, 0, 255));
    }
    // draw a green square around the XY view
    for (int i = 0; i < tmp_im3D.width; i++) {
        for (int j = 0; j < 3; j++){
            imageXY.setPixel(i,j,qRgb(0, 255, 0));
            imageXY.setPixel(i,tmp_im3D.height-j-1,qRgb(0, 255, 0));
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < tmp_im3D.height; j++){
            imageXY.setPixel(i,j,qRgb(0, 255, 0));
            imageXY.setPixel(tmp_im3D.width-i-1,j,qRgb(0, 255, 0));
        }
    }
    //-------------- XZ view -----------------------------------------------------
    for (int i = 0; i < tmp_im3D.width; i++){
        for (int z = 0; z < tmp_im3D.slices; z++){
            int index = layerNrXZ * tmp_im3D.width + i + tmp_im3D.width*tmp_im3D.height*z ;
            int HU_value = tmp_im3D.pImage[index];
            int iGrauwert;
            int error_stat = MyLib::windowing(HU_value, startValueXZ, windowWidthXZ, iGrauwert);
            imageXZ.setPixel(i,z,qRgb(iGrauwert, iGrauwert, iGrauwert));
        }
    }
    // draw a blue square around the XZ view
    for (int i = 0; i < tmp_im3D.width; i++) {
        for (int j = 0; j < 3; j++){
            imageXZ.setPixel(i,j,qRgb(0, 0, 255)); 
            imageXZ.setPixel(i,tmp_im3D.slices-j-1,qRgb(0, 0, 255));
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < tmp_im3D.slices; j++){
            imageXZ.setPixel(i,j,qRgb(0, 0, 255));
            imageXZ.setPixel(tmp_im3D.width-i-1,j,qRgb(0, 0, 255));
        }
    }
    // draw a green line in the XY view which shows the current XY layer
    for (int i = 0; i < tmp_im3D.width; i++) {
        imageXZ.setPixel(i,layerNrXY,qRgb(0, 255, 0));
    }

    // ------- draw selected points and the connecting line ------------------------
    if (drawLine){ // draw the line connecting first point to the second point in top view
        float tanLine = (firstPoint.y - secPoint.y)/(firstPoint.x - secPoint.x);
        for (float i = 0; i < 1000; i++) {
            float dx = firstPoint.x + (secPoint.x - firstPoint.x)/1000.0*i; // interpolate between the two points
            float dy = (dx-firstPoint.x)* tanLine + firstPoint.y;
            int x = (int)dx;
            int y = (int)dy;
            imageXY.setPixel(x,y,qRgb(255, 255, 0));
        }
        // draw a circle around the second point
        for (int i = 0; i< tmp_im3D.width ; i++){
                for (int j = 0; j < tmp_im3D.height ; j++){
                    if(pow(i- secPoint.x,2) + pow(j-secPoint.y,2) < 36 ){
                        imageXY.setPixel(i,j, qRgb(255, 0, 0));
                    }
                }
         }
        // draw a line which visualize the second point selected in the top view in XZ view
        for (int z = 0; z < tmp_im3D.slices ; z++){
            imageXZ.setPixel(secPoint.x,z, qRgb(255, 0, 0));
         }
    }
    if (countPoint==2 || drawLine){ // first point got selected/updated
        // draw a circle around the first point
        for (int i = 0; i < tmp_im3D.width ; i++){
                for (int j = 0; j < tmp_im3D.height ; j++){
                    if(pow(i- firstPoint.x,2) + pow(j-firstPoint.y,2) < 36 ){
                        imageXY.setPixel(i,j, qRgb(255, 0, 0));
                    }
                }
         }
        // draw a line which visualize the first point selected in the top view in XZ view
        for (int z = 0; z < tmp_im3D.slices ; z++){
            imageXZ.setPixel(firstPoint.x,z, qRgb(255, 0, 0));
         }
    }

    if (drawLine){ // draw the line connecting first point to the second point in XZ view
        float tanLine = (firstPoint.z - secPoint.z)/(firstPoint.x - secPoint.x);
        for (float i = 0; i < 1000; i++) {
            float dx = firstPoint.x + (secPoint.x - firstPoint.x)/1000.0*i; // interpolate betwenn the two points
            float dy = (dx-firstPoint.x)* tanLine + firstPoint.z;
            int x = (int)dx;
            int y = (int)dy;
            imageXZ.setPixel(x,y,qRgb(255, 255, 0));
        }
        // draw a circle around the second point in XZ view
        for (int i = 0; i< tmp_im3D.width ; i++){
                for (int z = 0; z< tmp_im3D.slices ; z++){
                    if(pow(i- secPoint.x,2) + pow(z-secPoint.z,2) < 36 ){
                        imageXZ.setPixel(i,z, qRgb(255, 0, 0));
                    }
                }
         }
    }
    if (countPoint==2 || drawLine){ // first point got selected/updated
        // draw a circle around the first point in XZ view
        for (int i = 0; i< tmp_im3D.width ; i++){
                for (int z = 0; z< tmp_im3D.slices ; z++){
                    if(pow(i- firstPoint.x,2) + pow(z-firstPoint.z,2) < 36 ){
                        imageXZ.setPixel(i,z, qRgb(255, 0, 0));
                    }
                }
         }
    }

    if(drawDrillTrajectory){
        // draw a circle around the pos point in both views
        for (int i = 0; i<=512 ; i++){
             for (int j = 0; j<=512 ; j++){
                  if(pow(i- reco.pos.x,2)+ pow(j-reco.pos.y,2)< 36 ){
                         imageXY.setPixel(i,j, qRgb(255, 255, 0));
                  }
             }
         }
        for (int i = 0; i<=512 ; i++){
            for (int z = 0; z<=512 ; z++){
                  if(pow(i- reco.pos.x,2) + pow(z-reco.pos.z,2)< 36 ){
                        imageXZ.setPixel(i,z, qRgb(255, 255, 0));
                  }
             }
         }
        // draw the vector in the pos normal to the DrillTrajectory
        double a = secPoint.x - firstPoint.x;
        double b = secPoint.y - firstPoint.y;
        double c = secPoint.z - firstPoint.z;
        // normal to (a,b,c) and passing reco.pos
        float tanLineXY = -a/b;
        float tanLineXZ = -a/c;

        for (int l=-100; l<=100; l++){
            float dx = reco.pos.x + (-b)*l/(pow(pow(a,2)+pow(b,2),.5));
            float dy = reco.pos.y + (a) *l/(pow(pow(a,2)+pow(b,2),.5));
            int x = (int)dx;
            int y = (int)dy;
            if (x>=0 && x<512 && y>=0 && y<512){
                imageXY.setPixel(x,y, qRgb(255, 255, 0));
            }
        }
        for (int l=-100; l<=100; l++){
            float dx = reco.pos.x + (-c)*l/(pow(pow(a,2)+pow(b,2),.5));
            float dz = reco.pos.z + (a) *l/(pow(pow(a,2)+pow(c,2),.5));
            int x = (int)dx;
            int z = (int)dz;
            if (x>=0 && x<512 && z>=0 && z<512){
                imageXZ.setPixel(x,z, qRgb(255, 255, 0));
            }
        }

    }


    //------------------------------------------------------------------------
    QImage imageReco(reco_im2D->width,reco_im2D->height, QImage::Format_RGB32);
    for (int i = 0; i < reco_im2D->width; i++){
        for (int j = 0; j < reco_im2D->height; j++){
            int iHuVal = reco_im2D->pImage[i + reco_im2D->width*j];
            int iGrayVal;
            int error_stat = MyLib::windowing(iHuVal, startValueXY, windowWidthXY, iGrayVal);
            imageReco.setPixel(i,j, qRgb(iGrayVal, iGrayVal, iGrayVal));
        }
    }

    // ------- update pics in the GUI -----------------------------------------
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_imageXY->setPixmap(QPixmap::fromImage(imageXY));
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_imageXZ->setPixmap(QPixmap::fromImage(imageXZ));
    // Bild auf Benutzeroberfläche anzeigen
    ui->label->setPixmap(QPixmap::fromImage(imageReco));
}
void ImageLoader::loadData(){
    // set path of the data
    // QString path = QFileDialog::getOpenFileName(this, "Open Image", "./","Raw Image Files (*.raw)");
    QString path = "C:/Users/Hossein/Desktop/SoftwareentwicklungSS2020_Git_Repo/Femur256.raw";
    // read the data and report the satus
    bool stat = m_pData->uploadImage(path);
    if (stat){ // if uploading image was successful
        updateView();
    }
    else{
        QMessageBox::critical(this, "ERROR", "Could not open/read the data");
    }
}
// fuctions for other features ----------------------------------------------
void ImageLoader::update3DreflectionXY(){
    int treshold = ui->Slider_ThresholdXY_2->value();
    bool stat = m_pData->calculateDepthMapXY(treshold);
    const image2D* tmpTiefenkarte = m_pData->getDepthMapXY();
    image2D im2D = image2D(tmpTiefenkarte->width,tmpTiefenkarte->height);
    stat = MyLib::calc3Dreflection(tmpTiefenkarte, im2D);
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(im2D.width,im2D.height, QImage::Format_RGB32);
    for (int i = 1; i < im2D.width-1; i++){
        for (int j = 1; j < im2D.height-1; j++){
            int iReflection = im2D.pImage[i + j*im2D.width];
            image.setPixel(i,j,qRgb(iReflection, iReflection, iReflection));
        }
    }
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_3DXY->setPixmap(QPixmap::fromImage(image));
}
void ImageLoader::update3DreflectionXZ(){
    int thresholdXZ = ui->Slider_ThresholdXZ_2->value();
    bool stat = m_pData->calculateDepthMapXZ(thresholdXZ);
    const image2D* depthMap = m_pData->getDepthMapXZ();
    image2D im2D = image2D(depthMap->width,depthMap->height);
    stat = MyLib::calc3Dreflection(depthMap, im2D);
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(im2D.width,im2D.height, QImage::Format_RGB32);
    for (int i = 1; i < im2D.width-1 ; i++){
        for (int j = 1; j < im2D.height-1 ; j++){
            int iReflection = im2D.pImage[i + j*im2D.width];
            image.setPixel(i,j,qRgb(iReflection, iReflection, iReflection));
        }
    }
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_3DXZ->setPixmap(QPixmap::fromImage(image));
}
void ImageLoader::updateDepthMapXY(){
    int threshold = ui->Slider_ThresholdXY_2->value();
    bool stat = m_pData->calculateDepthMapXY(threshold);
    const image2D* depthMap = m_pData->getDepthMapXY();
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(depthMap->width,depthMap->height, QImage::Format_RGB32);
    for (int i = 1; i < depthMap->width-1; i++){
        for (int j = 1; j < depthMap->height-1; j++){
            int iReflection = depthMap->pImage[i + depthMap->width*j];
            image.setPixel(i,j,qRgb(iReflection, iReflection, iReflection));
        }
    }
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_3DXY->setPixmap(QPixmap::fromImage(image));
}
void ImageLoader::updateDepthMapXZ(){
    int treshold = ui->Slider_ThresholdXZ_2->value();
    bool stat = m_pData->calculateDepthMapXZ(treshold);
    const image2D* depthMap = m_pData->getDepthMapXZ();
    image2D im2D = image2D(depthMap->width,depthMap->height);
    stat = MyLib::calc3Dreflection(depthMap, im2D);
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(depthMap->width,depthMap->height, QImage::Format_RGB32);
    for (int i = 1; i < depthMap->width-1; i++){
        for (int j = 1; j < depthMap->height-1; j++){
            int iReflection = depthMap->pImage[i + depthMap->width*j];
            image.setPixel(i,j,qRgb(iReflection, iReflection, iReflection));
        }
    }
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_3DXZ->setPixmap(QPixmap::fromImage(image));
}
// GUI functions ------------------------------------------------------------
void ImageLoader::updateWindowingStartXY(int value){
    ui->label_StartXY->setText("Start: " + QString::number(value));
    updateView();
}
void ImageLoader::updateWindowingWidthXY(int value){
    ui->label_WidthXY->setText("Width: " + QString::number(value));
    updateView();
}
void ImageLoader::updateLayerNrXY(int value){
    ui->label_LayerXY->setText("Layer: " + QString::number(value));
    updateView();
}
void ImageLoader::updateTresholdXY(int value){
    ui->label_ThresholdXY->setText("Threshold: " + QString::number(value));
    updateView();
}
void ImageLoader::updateTresholdXY_2(int value){
    ui->label_ThresholdXY_2->setText("Threshold: " + QString::number(value));
}
void ImageLoader::updateWindowingStartXZ(int value){
    ui->label_StartXZ->setText("Start: " + QString::number(value));
    updateView();
}
void ImageLoader::updateWindowingWidthXZ(int value){
    ui->label_WidthXZ->setText("Width: " + QString::number(value));
    updateView();
}
void ImageLoader::updateLayerNrXZ(int value){
    ui->label_LayerXZ->setText("Layer: " + QString::number(value));
    updateView();
}
void ImageLoader::updateTresholdXZ(int value){
    ui->label_ThresholdXZ->setText("Threshold: " + QString::number(value));
    updateView();
}
void ImageLoader::updateTresholdXZ_2(int value){
    ui->label_ThresholdXZ_2->setText("Threshold: " + QString::number(value));
}
void ImageLoader::updateSliceDepth(int value){
    ui->label_SliceDepth->setText("Move layer along the trajectory % " + QString::number(value));
    if (drawDrillTrajectory == true){
        reconstructSlice();
    }
}
void ImageLoader::updateAllSlidernLabels(){
    // update text labels at the beginning
    int startValue = ui->slider_StartValueXY->value();
    int windowWidth = ui->slider_WindowWidthXY->value();
    int layerNr = ui->slider_LayerNrXY->value();
    int treshold = ui->Slider_ThresholdXY->value();
    int treshold_2 = ui->Slider_ThresholdXY_2->value();
    int SliceDepth = ui->Slider_SliceDepth->value();

    ui->label_StartXY->setText("Start: " + QString::number(startValue));
    ui->label_WidthXY->setText("Width: " + QString::number(windowWidth));
    ui->label_LayerXY->setText("Layer: " + QString::number(layerNr));
    ui->label_ThresholdXY->setText("Threshold: " + QString::number(treshold));
    ui->label_ThresholdXY_2->setText("Threshold: " + QString::number(treshold_2));

    int startValueXZ = ui->slider_StartValueXZ->value();
    int windowWidthXZ = ui->slider_WindowWidthXZ->value();
    int layerNrXZ = ui->slider_LayerNrXZ->value();
    int tresholdXZ = ui->Slider_ThresholdXZ->value();
    int tresholdXZ_2 = ui->Slider_ThresholdXZ->value();
    ui->label_StartXZ->setText("Start: " + QString::number(startValueXZ));
    ui->label_WidthXZ->setText("Width: " + QString::number(windowWidthXZ));
    ui->label_LayerXZ->setText("Layer: " + QString::number(layerNrXZ));
    ui->label_ThresholdXZ->setText("Threshold: " + QString::number(tresholdXZ));
    ui->label_ThresholdXZ_2->setText("Threshold: " + QString::number(tresholdXZ_2));
    ui->label_SliceDepth->setText("Move layer along the trajectory % " + QString::number(SliceDepth));
}
