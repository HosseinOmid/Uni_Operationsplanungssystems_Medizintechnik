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
    countPoint = 1;
    drawLine = false;
    countPointFront = 1;
    drawLineFront = false;
    //m_ApplicationData = new ApplicationData(this);
    //m_ApplicationData->setData(&m_data);

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
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(updateView()));

    ImageLoader::updateAllSlidernLabels();
}

// destructor ------------------------------------------
ImageLoader::~ImageLoader(){
    delete ui;
    // Speicher wieder freigeben
    //delete m_pData; //brauchen wir das?
}
// creat a connection to application data --------------
void ImageLoader::setData(ApplicationData *pData){
    this->m_pData = pData;
}

void ImageLoader::reconstructSlice(){
    Reconstruction reco;
    vector planeNormalVector;

    double a = secPoint.x - firstPoint.x;
    double b = secPoint.y - firstPoint.y;
    double c = secPoint.z - firstPoint.z;
    planeNormalVector.x = a;
    planeNormalVector.x = b;
    planeNormalVector.x = c;

    int sliceDistToFirstPointPerc = ui->horizontalSlider->value();

    reco.pos.x = firstPoint.x + a * sliceDistToFirstPointPerc/100;
    reco.pos.y = firstPoint.y + b * sliceDistToFirstPointPerc/100;
    reco.pos.z = firstPoint.z + c * sliceDistToFirstPointPerc/100;

    // equation on plane: ax + by + cz = d
    // also we know that the point reco.pos is place on the plan
    double d = a*reco.pos.x + b*reco.pos.y + c*reco.pos.z;
    // consider xdir=(1, y1, 0) ; ydir=(1, y2, z2)
    // xdir and ydir are orthogonal, e.a (xdir.ydir = 0)
    // and both xdir and ydir are placed on the plan and should satisfy the plane equation
    // so the result is the following:
    reco.xdir.x = 1;
    reco.xdir.y = (d-a)/b;
    reco.xdir.z = 0;

    reco.ydir.x = 1;
    reco.ydir.y = -b/(d-a);
    reco.ydir.z = (d-a + pow(b,2)/(d-a))/c;
}

/*void ImageLoader::mousePressEvent(QMouseEvent *event){
    QPoint globalPos;
    globalPos = event->pos();
    QPoint localPos;
    QPoint localPosFront;
    localPos = ui->label_image->mapFromParent(globalPos);
    localPosFront = ui->label_image_front->mapFromParent(globalPos);

    if (ui->label_image->rect().contains(localPos)){
        if (countPoint == 1){
            firstPoint.x = localPos.x();
            firstPoint.y = localPos.y();
            countPoint = 2;
            emit LOG("First point got successfully chosen.");
        }
        else if (countPoint == 2){
            secPoint.x = localPos.x();
            secPoint.y = localPos.y();
            countPoint = 3;
            drawLine = true;
            emit LOG("Second point got successfully chosen.");
        }
        else if (countPoint == 3){
            firstPoint.x = localPos.x();
            firstPoint.y = localPos.y();
            countPoint = 2;
            drawLine = true;
            emit LOG("First point got successfully updated");
        }
    }
    if (ui->label_image_front->rect().contains(localPosFront)){
        if (countPoint == 1){
            firstPointFront.x = localPosFront.x();
            firstPointFront.z = localPosFront.y();
            countPointFront = 2;
            emit LOG("First point got successfully chosen.");
        }
        else if (countPointFront == 2){
            secPointFront.x = localPosFront.x();
            secPointFront.z = localPosFront.y();
            countPointFront = 3;
            drawLineFront = true;
            emit LOG("Second point got successfully chosen.");
        }
        else if (countPoint == 3){
            firstPointFront.x = localPosFront.x();
            firstPointFront.z = localPosFront.y();
            countPointFront = 2;
            drawLineFront = true;
            emit LOG("First point got successfully updated");
        }
    }
    updateView();
}
*/
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


    if (true){
        // ------- Top-View -------------------------------------------------------------
        for (int i = 0; i < tmp_im3D.width; i++){
            for (int j = 0; j < tmp_im3D.height; j++){
                int index = j * tmp_im3D.width + i + tmp_im3D.width*tmp_im3D.height*layerNrXY ;
                int HU_value = tmp_im3D.pImage[index];
                int iGrauwert;
                int error_stat = MyLib::windowing(HU_value, startValueXY, windowWidthXY, iGrauwert);
                imageXY.setPixel(i,j,qRgb(iGrauwert, iGrauwert, iGrauwert));
            }
        }
    }
    if(true){
        //-------------- front view -----------------------------------------------------
        for (int i = 0; i < tmp_im3D.width; i++){
            for (int z = 0; z < tmp_im3D.slices; z++){
                int index = layerNrXZ * tmp_im3D.width + i + tmp_im3D.width*tmp_im3D.height*z ;
                int HU_value = tmp_im3D.pImage[index];
                int iGrauwert;
                int error_stat = MyLib::windowing(HU_value, startValueXZ, windowWidthXZ, iGrauwert);
                imageXZ.setPixel(i,z,qRgb(iGrauwert, iGrauwert, iGrauwert));
            }
        }
    }
    // ------- draw lines & squares showing the current layers -------------------------------
    // draw a yellow line which shows the front layer view
    for (int i = 0; i < 512; i++) {
        imageXY.setPixel(i,layerNrXZ,qRgb(255, 255, 0));
    }
    // draw a green square around the top view
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 3; j++){
            imageXZ.setPixel(i,j,qRgb(255, 255, 0));
            imageXZ.setPixel(j,i,qRgb(255, 255, 0));
            imageXZ.setPixel(i,511-j,qRgb(255, 255, 0));
            imageXZ.setPixel(511-j,i,qRgb(255, 255, 0));
        }
    }
    // draw a green line which shows the top layer view
    for (int i = 0; i < 512; i++) {
        imageXZ.setPixel(i,layerNrXY,qRgb(0, 255, 0));
    }
    // draw a green square around the top view
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 3; j++){
            imageXY.setPixel(i,j,qRgb(0, 255, 0));
            imageXY.setPixel(j,i,qRgb(0, 255, 0));
            imageXY.setPixel(i,511-j,qRgb(0, 255, 0));
            imageXY.setPixel(511-j,i,qRgb(0, 255, 0));
        }
    }

    // ------- draw selected points and the connecting line ------------------------
    if (drawLine){ // draw the line connecting first point to the second point in top view
        for (float i = 0; i < 512; i++) {
            float tanLine = (firstPoint.y - secPoint.y)/(firstPoint.x - secPoint.x);
            float dj = (i-secPoint.x)* tanLine + secPoint.y;
            int j = (int)dj;
            imageXY.setPixel(i,j,qRgb(255, 0, 0));
        }
        for (float j = 0; j < 512; j++) {
            float tanLine = (firstPoint.x - secPoint.x)/(firstPoint.y - secPoint.y);
            float di = (j-secPoint.y)* tanLine + secPoint.x;
            int i = (int)di;
            imageXY.setPixel(i,j,qRgb(255, 0, 0));
        }
        // draw a circle around the second point
        for (int i = 0; i<=512 ; i++){
                for (int j = 0; j<=512 ; j++){
                    if(pow(i- secPoint.x,2) + pow(j-secPoint.y,2) < 36 ){
                        imageXY.setPixel(i,j, qRgb(255, 0, 0));
                    }
                }
         }
        // draw a line which visualize the second point selected in the top view in front view
        for (int z = 0; z < 512 ; z++){
            imageXZ.setPixel(secPoint.x,z, qRgb(255, 0, 0));
         }
    }
    if (countPoint==2 || countPoint==3) // first point got selected/updated
    {
        // draw a circle around the first point
        for (int i = 0; i < 512 ; i++){
                for (int j = 0; j < 512 ; j++){
                    if(pow(i- firstPoint.x,2) + pow(j-firstPoint.y,2) < 36 ){
                        imageXY.setPixel(i,j, qRgb(255, 0, 0));
                    }
                }
         }
        // draw a line which visualize the first point selected in the top view in front view
        for (int z = 0; z < 512 ; z++){
            imageXZ.setPixel(firstPoint.x,z, qRgb(255, 0, 0));
         }
    }



    if (drawLineFront){ // draw the line connecting first point to the second point in front view
        for (float i = 0; i < 512; i++) {
            float tanLine = (firstPointFront.z - secPointFront.z)/(firstPointFront.x - secPointFront.x);
            float dj = (i-secPointFront.x)* tanLine + secPointFront.z;
            int j = (int)dj;
            imageXZ.setPixel(i,j,qRgb(255, 0, 0));
        }
        for (float z = 0; z < 512; z++) {
            float tanLine = (firstPointFront.x - secPointFront.x)/(firstPointFront.z - secPointFront.z);
            float di = (z-secPointFront.z)* tanLine + secPointFront.x;
            int i = (int)di;
            imageXZ.setPixel(i,z,qRgb(255, 0, 0));
        }
        // draw a circle around the second point in front view
        for (int i = 0; i<=512 ; i++){
                for (int z = 0; z<=512 ; z++){
                    if(pow(i- secPointFront.x,2) + pow(z-secPointFront.z,2) < 36 ){
                        imageXZ.setPixel(i,z, qRgb(255, 0, 0));
                    }
                }
         }
        // draw a line which visualize the second point selected in the front view in top view
        for (int j = 0; j < 512 ; j++){
            imageXY.setPixel(secPointFront.x,j, qRgb(255, 0, 0));
         }
    }
    if (countPointFront==2 || countPointFront==3) // first point got selected/updated
    {
        // draw a circle around the first point in front view
        for (int i = 0; i<=512 ; i++){
                for (int z = 0; z<=512 ; z++){
                    if(pow(i- firstPointFront.x,2) + pow(z-firstPointFront.z,2) < 36 ){
                        imageXZ.setPixel(i,z, qRgb(255, 0, 0));
                    }
                }
         }
        // draw a line which visualize the first point selected in the front view in top view
        for (int j = 0; j < 512 ; j++){
            imageXY.setPixel(firstPointFront.x,j, qRgb(255, 0, 0));
         }
    }


    // ------- update pics in the GUI -----------------------------------------
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_imageXY->setPixmap(QPixmap::fromImage(imageXY));
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_imageXZ->setPixmap(QPixmap::fromImage(imageXZ));


    //
    QImage image45(tmp_im3D.width,tmp_im3D.height, QImage::Format_RGB32);
    for (int i = 0; i <512; i++) {
        for (int j = 0; j < 512; j++){
            // Ebene: x+z = constant
            double dx = ui->horizontalSlider->value() - j*pow(2,.5);
            int x = (int) dx;
            int y = i;
            double dz = j*pow(2,.5);
            int z = (int) dz;
            int iGrayVal;
            int iIndex = x+y*512+z*512*512;
            if (x>=0 && x<tmp_im3D.width && y>=0 && y<tmp_im3D.height && z>=0 && z<tmp_im3D.slices){
                int iHuVal = tmp_im3D.pImage[iIndex];
                int error_stat = MyLib::windowing(iHuVal, startValueXY, windowWidthXY, iGrayVal);
            }
            else{
                iGrayVal = 0;
            }
            image45.setPixel(i,j, qRgb(iGrayVal, iGrayVal, iGrayVal));
        }
    }
    ui->label->setPixmap(QPixmap::fromImage(image45));
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

// fuctions for other features --------------------------------------------------------
void ImageLoader::update3DreflectionXY(){
    int treshold = ui->Slider_ThresholdXY_2->value();
    bool stat = m_pData->calculateDepthMap(treshold);
    const image2D* tmpTiefenkarte = m_pData->getDepthMap();
    image2D im2D = image2D(tmpTiefenkarte->width,tmpTiefenkarte->height);
    stat = MyLib::calc3Dreflection(tmpTiefenkarte, im2D);
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(512,512, QImage::Format_RGB32);
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
    int tresholdFront = ui->Slider_ThresholdXZ_2->value();
    bool stat = m_pData->calculateDepthMapFront(tresholdFront);
    const image2D* tmpTiefenkarte = m_pData->getDepthMapFront();
    image2D im2D = image2D(tmpTiefenkarte->width,tmpTiefenkarte->height);
    stat = MyLib::calc3Dreflection(tmpTiefenkarte, im2D);
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(512,512, QImage::Format_RGB32);
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
    int treshold = ui->Slider_ThresholdXY_2->value();
    bool stat = m_pData->calculateDepthMap(treshold);
    const image2D* tmpTiefenkarte = m_pData->getDepthMap();
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(512,512, QImage::Format_RGB32);
    for (int i = 1; i < 511; i++){
        for (int j = 1; j < 511; j++){
            int iReflection = tmpTiefenkarte->pImage[i + 512*j];
            image.setPixel(i,j,qRgb(iReflection, iReflection, iReflection));
        }
    }
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_3DXY->setPixmap(QPixmap::fromImage(image));
}
void ImageLoader::updateDepthMapXZ(){
    int treshold = ui->Slider_ThresholdXY_2->value();
    bool stat = m_pData->calculateDepthMapFront(treshold);
    const image2D* tmpTiefenkarte = m_pData->getDepthMapFront();
    image2D im2D = image2D(tmpTiefenkarte->width,tmpTiefenkarte->height);
    stat = MyLib::calc3Dreflection(tmpTiefenkarte, im2D);
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(512,512, QImage::Format_RGB32);
    for (int i = 1; i < 511; i++){
        for (int j = 1; j < 511; j++){
            int iReflection = tmpTiefenkarte->pImage[i + 512*j];
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
void ImageLoader::updateAllSlidernLabels(){
    // update text labels at the beginning
    int startValue = ui->slider_StartValueXY->value();
    int windowWidth = ui->slider_WindowWidthXY->value();
    int layerNr = ui->slider_LayerNrXY->value();
    int treshold = ui->Slider_ThresholdXY->value();
    int treshold_2 = ui->Slider_ThresholdXY_2->value();
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
}
