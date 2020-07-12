#include "imageloader.h"
#include "ui_ImageLoader.h"
#include "applicationdata.h"
#include "mylib.h"
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>
#include <cmath>
#include <QMouseEvent>
// Autor: Hossein Omid Beiki
// constructor ------------------------------------------------------------------------------
ImageLoader::ImageLoader(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    sliceFrameSize.x = 512;
    sliceFrameSize.y = 512;
    reco_im2D =  new image2D(sliceFrameSize.x,sliceFrameSize.y);
    // connect events of GUI with functions
    ui->setupUi(this);

    // load Data
    connect(ui->pushButton_load3D, SIGNAL(clicked()), this, SLOT(loadData()));

    // reconstruct a selected slice
    connect(ui->pushButton_reconstruct, SIGNAL(clicked()), this, SLOT(reconstructSlice()));
    connect(ui->Slider_Scale, SIGNAL(valueChanged(int)), this, SLOT(updateScale(int)));
    connect(ui->pushButton_resetPoints, SIGNAL(clicked()), this, SLOT(resetPoints()));

    // GUI slider and labels
    connect(ui->slider_StartValueXY, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingStartXY(int)));
    connect(ui->slider_WindowWidthXY, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingWidthXY(int)));
    connect(ui->slider_LayerNrXY, SIGNAL(valueChanged(int)), this, SLOT(updateLayerNrXY(int)));
    connect(ui->slider_StartValueXZ, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingStartXZ(int)));
    connect(ui->slider_WindowWidthXZ, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingWidthXZ(int)));
    connect(ui->slider_LayerNrXZ, SIGNAL(valueChanged(int)), this, SLOT(updateLayerNrXZ(int)));
    connect(ui->Slider_ThresholdXZ, SIGNAL(valueChanged(int)), this, SLOT(updateTresholdXZ_2(int)));
    connect(ui->Slider_ThresholdXY, SIGNAL(valueChanged(int)), this, SLOT(updateTresholdXY_2(int)));
    connect(ui->slider_StartValueSlice, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingStartSlice(int)));
    connect(ui->slider_WindowWidthSlice, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingWidthSlice(int)));
    connect(ui->Slider_SliceDepth, SIGNAL(valueChanged(int)), this, SLOT(updateSliceDepth(int)));

    // other features
    connect(ui->pushButton_DepthMapXZ, SIGNAL(clicked()), this, SLOT(updateDepthMapXY()));
    connect(ui->pushButton_DepthMapXZ, SIGNAL(clicked()), this, SLOT(updateDepthMapXZ()));
    connect(ui->pushButton_3DXY, SIGNAL(clicked()), this, SLOT(update3DreflectionXY()));
    connect(ui->pushButton_3DXZ, SIGNAL(clicked()), this, SLOT(update3DreflectionXZ()));

    // update all slider and labels
    ImageLoader::updateAllSlidernLabels();

    // initialize important variabels
    countPoint = 1;
    bothPointsAreSelected = false;
    drillTrajectoryIsDefined = false;
    scale = 1;
}

// destructor -------------------------------------------------------------------------------
ImageLoader::~ImageLoader(){
    delete ui;
    // Speicher wieder freigeben
    // delete m_pData; //brauchen wir das? führt zu Fehler...
}

// important functional functions -----------------------------------------------------------
void ImageLoader::setData(ApplicationData *pData){
    // creat a connection to application data
    this->m_pData = pData;
}

void ImageLoader::reconstructSlice(){
    QString drillDiamQstr =  ui->textEdit_DrillDiam->toPlainText();
    drillDiameter = drillDiamQstr.toDouble();

    reco.scale = scale;

    double a = secPoint.x - firstPoint.x;
    double b = secPoint.y - firstPoint.y;
    double c = secPoint.z - firstPoint.z;

    int sliceDistToFirstPointPerc = ui->Slider_SliceDepth->value();

    reco.pos.x = firstPoint.x + a * sliceDistToFirstPointPerc/100;
    reco.pos.y = firstPoint.y + b * sliceDistToFirstPointPerc/100;
    reco.pos.z = firstPoint.z + c * sliceDistToFirstPointPerc/100;

    // equation of a plane: ax + by + cz = d
    // also we know that the point reco.pos is place on the plan, so:
    // double d = a*reco.pos.x + b*reco.pos.y + c*reco.pos.z;
    // so the plane ist defined well.
    // we want to define two orthogonal vectors which describe our plane.
    // there are infit such vectors, since there are infinit lines on a plane.
    // so we just define that xdir has no z component, and its x component has the length of 1
    // also we define that ydir has x component of length 1. Later we can normalize both vectors.
    // so consider xdir=(1, y1, 0) ; ydir=(1, y2, z2)
    // we have 3 unknown variables and the following 3 equations:
    // 1) xdir and normal vector (a,b,c) are orthogonal. so the scalar product should be zero.
    //    xdir . (a,b,c) = 0 -> 1*a + y1*b + 0*c = 0 -> y1 = -a/b
    // so we get xdir = (1, -a/b, 0)
    // 2) xdir and ydir are orthogonal, e.a (xdir.ydir = 0)
    //    it means 1*1 + (-a)/b*y2 + 0*z2 = 0 -> y2 = b/a
    // 3) ydir is orthogonal to normal vector (a,b,c):
    //    ydir . (a,b,c) = 0 -> 1*a + b/a*b + z2*c = 0 -> z2 = (-a - b^2/a)/c
    // so we get ydir = (1, b/a, (-a - b^2/a)/c)
    // now we multiply xdir with the scalar a to avoid deviding by zero, in case a ist zero
    // also we multiply ydir with the scalar a*c to avoid deviding by zero

    // special cases are considered separately as following:
    if (a==0 && b==0){
        reco.xdir.x = 1;
        reco.xdir.y = 0;
        reco.xdir.z = 0;

        reco.ydir.x = 0;
        reco.ydir.y = 1;
        reco.ydir.z = 0;
    }
    else if(a==0 && c==0){
        reco.xdir.x = 1;
        reco.xdir.y = 0;
        reco.xdir.z = 0;

        reco.ydir.x = 0;
        reco.ydir.y = 0;
        reco.ydir.z = 1;
    }
    else if(b==0 && c==0){
        reco.xdir.x = 0;
        reco.xdir.y = 1;
        reco.xdir.z = 0;

        reco.ydir.x = 0;
        reco.ydir.y = 0;
        reco.ydir.z = 1;
    }
    else{ // see above comments
        reco.xdir.x = b;
        reco.xdir.y = -a;
        reco.xdir.z = 0;

        reco.ydir.x = -c*a;
        reco.ydir.y = -c*b;
        reco.ydir.z = -(-a*a - b*b);
    }
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
    // error_stat: 0 if ok. -1 if input image is incorrect. -2 if output im2D is incorrect.
    if (err_stat==0){
        emit LOG("Slice got reconstructed successfully.");
        drillTrajectoryIsDefined = true;
        updateView();
        updateAllLabelsDescribingSlice();
    }
    else{
        emit LOG("Error in reconstructing the selected slice. Please make sure the loaded image size is correct.");
    }
}

void ImageLoader::mousePressEvent(QMouseEvent *event){
    drillTrajectoryIsDefined = false;
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
            bothPointsAreSelected = true;
            emit LOG("Second point got successfully chosen.");
        }
        updateView();
    }
    if (ui->label_imageXZ->rect().contains(localPosXZ)){
        if (bothPointsAreSelected){
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
    int startValueXZ = ui->slider_StartValueXZ->value();
    int windowWidthXZ = ui->slider_WindowWidthXZ->value();
    int layerNrXZ = ui->slider_LayerNrXZ->value();
    // -------------- XY View ----------------------------------------------------
    for (int i = 0; i < tmp_im3D.width; i++){
        for (int j = 0; j < tmp_im3D.height; j++){
            int index = j * tmp_im3D.width + i + tmp_im3D.width*tmp_im3D.height*layerNrXY ;
            int HU_value = tmp_im3D.pImage[index];
            int iGrauwert;
            int error_stat = MyLib::windowing(HU_value, startValueXY, windowWidthXY, iGrauwert);
            if (error_stat==-1) // 0 if ok. -1 if HU_value is out of range. -2 if windowing parameters are out of range)
                emit LOG("Erro in Windowing! HU-value is out of range!");
            else if (error_stat==-2)
                emit LOG("Erro in Windowing! Windowing parameters are out of range!");
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
            if (error_stat==-1) // 0 if ok. -1 if HU_value is out of range. -2 if windowing parameters are out of range)
                emit LOG("Erro in Windowing! HU-value is out of range!");
            else if (error_stat==-2)
                emit LOG("Erro in Windowing! Windowing parameters are out of range!");
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
    if (bothPointsAreSelected){ // draw the line connecting first point to the second point in top view
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
    if (countPoint==2 || bothPointsAreSelected){ // first point got selected/updated
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

    if (bothPointsAreSelected){ // draw the line connecting first point to the second point in XZ view
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
    if (countPoint==2 || bothPointsAreSelected){ // first point got selected/updated
        // draw a circle around the first point in XZ view
        for (int i = 0; i< tmp_im3D.width ; i++){
                for (int z = 0; z< tmp_im3D.slices ; z++){
                    if(pow(i- firstPoint.x,2) + pow(z-firstPoint.z,2) < 36 ){
                        imageXZ.setPixel(i,z, qRgb(255, 0, 0));
                    }
                }
         }
    }

    if(drillTrajectoryIsDefined){
        // draw a circle around the pos point in both views
        for (int i = 0; i< tmp_im3D.width ; i++){
             for (int j = 0; j< tmp_im3D.height ; j++){
                  if(pow(i- reco.pos.x,2)+ pow(j-reco.pos.y,2)< 36 ){
                         imageXY.setPixel(i,j, qRgb(255, 255, 0));
                  }
             }
         }
        for (int i = 0; i< tmp_im3D.width ; i++){
            for (int z = 0; z< tmp_im3D.slices ; z++){
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
        for (int l=-100; l<=100; l++){
            float dx = reco.pos.x + (-b)*l/(pow(pow(a,2)+pow(b,2),.5));
            float dy = reco.pos.y + (a) *l/(pow(pow(a,2)+pow(b,2),.5));
            int x = (int)dx;
            int y = (int)dy;
            if (x>=0 && x<sliceFrameSize.x && y>=0 && y<sliceFrameSize.y){
                imageXY.setPixel(x,y, qRgb(255, 255, 0));
            }
        }
        for (int l=-100; l<=100; l++){
            float dx = reco.pos.x + (-c)*l/(pow(pow(a,2)+pow(b,2),.5));
            float dz = reco.pos.z + (a) *l/(pow(pow(a,2)+pow(c,2),.5));
            int x = (int)dx;
            int z = (int)dz;
            if (x>=0 && x<sliceFrameSize.x && z>=0 && z<sliceFrameSize.y){
                imageXZ.setPixel(x,z, qRgb(255, 255, 0));
            }
        }
    }
    //------------------ Slice View --------------------------------------------
    QImage imageReco(reco_im2D->width,reco_im2D->height, QImage::Format_RGB32);
    if (drillTrajectoryIsDefined){
        int startSlice = ui->slider_StartValueSlice->value();
        int windowWidthSlice = ui->slider_WindowWidthSlice->value();
        for (int i = 0; i < reco_im2D->width; i++){
            for (int j = 0; j < reco_im2D->height; j++){
                int iHuVal = reco_im2D->pImage[i + reco_im2D->width*j];
                int iGrayVal;
                int error_stat = MyLib::windowing(iHuVal, startSlice, windowWidthSlice, iGrayVal);
                if (error_stat==-1) // 0 if ok. -1 if HU_value is out of range. -2 if windowing parameters are out of range)
                    emit LOG("Erro in Windowing! HU-value is out of range!");
                else if (error_stat==-2)
                    emit LOG("Erro in Windowing! Windowing parameters are out of range!");
                imageReco.setPixel(i,j, qRgb(iGrayVal, iGrayVal, iGrayVal));
            }
        }
        // Draw a red circle in size of drill diameter
        double i0 = reco_im2D->width/2;
        double j0 = reco_im2D->height/2;
        for (float i = 0; i < reco_im2D->width; i = i+0.1) {
            // circle equation: (i-i0)^2 + (j-j0)^2 = r^2
            double scaledRadius = drillDiameter/2*scale;
            double deltaj2 = pow(scaledRadius,2) - pow(i-i0,2);
            if (deltaj2<0)
                continue;
            double dj1 = pow(deltaj2,.5) + j0;
            int j1 = (int)dj1;
            double dj2 = -pow(deltaj2,.5) + j0;
            int j2 = (int)dj2;
            if (j1> 0 && j2>0 && j1<reco_im2D->height && j2<reco_im2D->height){
                imageReco.setPixel(i,j1, qRgb(255, 0, 0));
                imageReco.setPixel(i,j2, qRgb(255, 0, 0));
            }
        }
        // draw a yellow square around the reconstructed slice
        for (int i = 0; i < reco_im2D->width; i++) {
            for (int j = 0; j < 3; j++){
                imageReco.setPixel(i,j,qRgb(255, 255, 0));
                imageReco.setPixel(i,reco_im2D->height-j-1,qRgb(255, 255, 0));
            }
        }
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < reco_im2D->height; j++){
                imageReco.setPixel(i,j,qRgb(255, 255, 0));
                imageReco.setPixel(reco_im2D->width-i-1,j,qRgb(255, 255, 0));
            }
        }
    }
    // ------- update pics in the GUI -----------------------------------------
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_imageXY->setPixmap(QPixmap::fromImage(imageXY));
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_imageXZ->setPixmap(QPixmap::fromImage(imageXZ));
    // Bild auf Benutzeroberfläche anzeigen
    if (drillTrajectoryIsDefined)
        ui->label->setPixmap(QPixmap::fromImage(imageReco));
}

void ImageLoader::loadData(){
    // set path of the data
    QString path = QFileDialog::getOpenFileName(this, "Open Image", "./","Raw Image Files (*.raw)");
    // read the data and report the satus
    bool stat = m_pData->uploadImage(path);
    if (stat){ // if uploading image was successful
        updateView();
        emit LOG(path);
        emit LOG("Data got successfully loaded.");

    }
    else{
        QMessageBox::critical(this, "ERROR", "Could not open/read the data");
    }
}

double ImageLoader::calculateDrillLength(){
    // get the data from the database
    const image3D tmp_im3D = m_pData->getImageData3D();
    double drillLength = pow(pow((firstPoint.x-secPoint.x)*tmp_im3D.pixelSpacingXY,2) + pow((firstPoint.y-secPoint.y)*tmp_im3D.pixelSpacingXY,2)+ pow((firstPoint.z- secPoint.z)*tmp_im3D.pixelSpacingZ ,2),.5);
    return drillLength;
}

void ImageLoader::resetPoints(){
    countPoint = 1;
    bothPointsAreSelected = false;
    drillTrajectoryIsDefined = false;
    updateView();
}
// GUI functions ----------------------------------------------------------------------------
void ImageLoader::updateAllLabelsDescribingSlice(){
    ui->label_p1->setText("Point 1: (" + QString::number(firstPoint.x) + "," + QString::number(firstPoint.y) + "," + QString::number(firstPoint.z) + ")");
    ui->label_p2->setText("Point 2: (" + QString::number(secPoint.x) + "," + QString::number(secPoint.y) + "," + QString::number(secPoint.z) + ")");
    ui->label_pos->setText("Slice Pos: (" + QString::number(round(reco.pos.x)) + "," + QString::number(round(reco.pos.y)) + "," + QString::number(round(reco.pos.z)) + ")");
    ui->label_xdir->setText("xdir: (" + QString::number(round(reco.xdir.x*10)/10) + "," + QString::number(round(reco.xdir.y*10)/10) + "," + QString::number(round(reco.xdir.z*10)/10) + ")");
    ui->label_ydir->setText("ydir: (" + QString::number(round(reco.ydir.x*10)/10) + "," + QString::number(round(reco.ydir.y*10)/10) + "," + QString::number(round(reco.ydir.z*10)/10) + ")");
    ui->label_DrillLength->setText("Drill Length: " + QString::number(round(calculateDrillLength()*10)/10)  + " mm");
}
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
void ImageLoader::updateTresholdXZ_2(int value){
    ui->label_ThresholdXZ_2->setText("Threshold: " + QString::number(value));
}
void ImageLoader::updateWindowingStartSlice(int value){
    ui->label_StartSlice->setText("Start :" + QString::number(value));
    if (drillTrajectoryIsDefined == true){
        reconstructSlice();
    }
}
void ImageLoader::updateWindowingWidthSlice(int value){
    ui->label_WidthSlice->setText("Width: " + QString::number(value));
    if (drillTrajectoryIsDefined == true){
        reconstructSlice();
    }
}
void ImageLoader::updateSliceDepth(int value){
    ui->label_SliceDepth->setText("Move layer along the trajectory % " + QString::number(value));
    if (drillTrajectoryIsDefined == true){
        reconstructSlice();
    }
}
void ImageLoader::updateScale(int value){
    scale = value;
    ui->label_ScaleFac->setText("Scale Factror: " + QString::number(scale));
    if (drillTrajectoryIsDefined == true){
        reconstructSlice();
    }
}
void ImageLoader::updateAllSlidernLabels(){
    // update text labels at the beginning
    int startValueXY = ui->slider_StartValueXY->value();
    int windowWidthXY = ui->slider_WindowWidthXY->value();
    int layerNrXY = ui->slider_LayerNrXY->value();
    int thresholdXY = ui->Slider_ThresholdXY->value();

    ui->label_StartXY->setText("Start: " + QString::number(startValueXY));
    ui->label_WidthXY->setText("Width: " + QString::number(windowWidthXY));
    ui->label_LayerXY->setText("Layer: " + QString::number(layerNrXY));
    ui->label_ThresholdXY_2->setText("Threshold: " + QString::number(thresholdXY));

    int startValueXZ = ui->slider_StartValueXZ->value();
    int windowWidthXZ = ui->slider_WindowWidthXZ->value();
    int layerNrXZ = ui->slider_LayerNrXZ->value();
    int thresholdXZ = ui->Slider_ThresholdXZ->value();

    ui->label_StartXZ->setText("Start: " + QString::number(startValueXZ));
    ui->label_WidthXZ->setText("Width: " + QString::number(windowWidthXZ));
    ui->label_LayerXZ->setText("Layer: " + QString::number(layerNrXZ));
    ui->label_ThresholdXZ_2->setText("Threshold: " + QString::number(thresholdXZ));

    int SliceDepth = ui->Slider_SliceDepth->value();
    int startSlice = ui->slider_StartValueSlice->value();
    int windowWidthSlice = ui->slider_WindowWidthSlice->value();

    ui->label_StartSlice->setText("Start: " + QString::number(startSlice));
    ui->label_WidthSlice->setText("Width: " + QString::number(windowWidthSlice));
    ui->label_SliceDepth->setText("Move layer along the trajectory % " + QString::number(SliceDepth));

    scale = ui->Slider_Scale->value();
    ui->label_ScaleFac->setText("Scale Factror: " + QString::number(scale));
}
// fuctions for other features --------------------------------------------------------------
void ImageLoader::update3DreflectionXY(){
    int treshold = ui->Slider_ThresholdXY->value();
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
    int thresholdXZ = ui->Slider_ThresholdXZ->value();
    bool stat = m_pData->calculateDepthMapXZ(thresholdXZ);
    const image2D* depthMap = m_pData->getDepthMapXZ();
    image2D im2D = image2D(depthMap->width,depthMap->height);
    stat = MyLib::calc3Dreflection(depthMap, im2D);
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(depthMap->width,depthMap->height, QImage::Format_RGB32);
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
    int threshold = ui->Slider_ThresholdXY->value();
    bool stat = m_pData->calculateDepthMapXY(threshold);
    const image2D* depthMap = m_pData->getDepthMapXY();
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(depthMap->width,depthMap->height, QImage::Format_RGB32);
    for (int i = 1; i < depthMap->width-1; i++){
        for (int j = 1; j < depthMap->height-1; j++){
            int iDepth = depthMap->pImage[i + depthMap->width*j];
            image.setPixel(i,j,qRgb(iDepth, iDepth, iDepth));
        }
    }
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_3DXY->setPixmap(QPixmap::fromImage(image));
}
void ImageLoader::updateDepthMapXZ(){
    int treshold = ui->Slider_ThresholdXZ->value();
    bool stat = m_pData->calculateDepthMapXZ(treshold);
    const image2D* depthMap = m_pData->getDepthMapXZ();
    image2D im2D = image2D(depthMap->width,depthMap->height);
    stat = MyLib::calc3Dreflection(depthMap, im2D);
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(depthMap->width,depthMap->height, QImage::Format_RGB32);
    for (int i = 1; i < depthMap->width-1; i++){
        for (int j = 1; j < depthMap->height-1; j++){
            int iDepth = depthMap->pImage[i + depthMap->width*j];
            image.setPixel(i,j,qRgb(iDepth, iDepth, iDepth));
        }
    }
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_3DXZ->setPixmap(QPixmap::fromImage(image));
}
