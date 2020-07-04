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
    //connect(ui->pushButton_Tiefenkarte, SIGNAL(clicked()), this, SLOT(updateTiefenkarteView()));
    connect(ui->pushButton_3D, SIGNAL(clicked()), this, SLOT(update3Dreflection()));
    connect(ui->pushButton_3D_Front, SIGNAL(clicked()), this, SLOT(update3DreflectionFront()));
    connect(ui->slider_StartValue, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingStart(int)));
    connect(ui->slider_WindowWidth, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingWidth(int)));
    connect(ui->slider_LayerNr, SIGNAL(valueChanged(int)), this, SLOT(updateLayerNr(int)));
    connect(ui->Slider_Threshold, SIGNAL(valueChanged(int)), this, SLOT(updateTreshold(int)));
    connect(ui->slider_StartValue_Front, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingStartFront(int)));
    connect(ui->slider_WindowWidth_Front, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingWidthFront(int)));
    connect(ui->slider_LayerNr_Front, SIGNAL(valueChanged(int)), this, SLOT(updateLayerNrFront(int)));
    connect(ui->Slider_Threshold_Front, SIGNAL(valueChanged(int)), this, SLOT(updateTresholdFront(int)));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(updateView()));
    //connect(ui->radioButton_DepthMap, SIGNAL(toggled()), this, SLOT(updateView()));
    //connect(ui->radioButton_3D, SIGNAL(toggled()), this, SLOT(updateView()));
    //connect(ui->radioButton_ViewLayers, SIGNAL(toggled()), this, SLOT(updateView()));
    connect(ui->radioButton_DepthMapFront, SIGNAL(toggled()), this, SLOT(updateView()));
    connect(ui->radioButton_3DFront, SIGNAL(toggled()), this, SLOT(updateView()));
    connect(ui->radioButton_ViewLayersFront, SIGNAL(toggled()), this, SLOT(updateView()));

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
void ImageLoader::updateView()
{
    // get the data from the database
    const image3D tmp_im3D = m_pData->getImageData3D();

    // creat objects of type QImage to visualize it in GUI
    QImage image(tmp_im3D.width,tmp_im3D.height, QImage::Format_RGB32);
    QImage imageFront(tmp_im3D.width,tmp_im3D.slices, QImage::Format_RGB32);

    // read values from the GUI
    int startValue = ui->slider_StartValue->value();
    int windowWidth = ui->slider_WindowWidth->value();
    int layerNr = ui->slider_LayerNr->value();
    int treshold = ui->Slider_Threshold->value();

    int startValueFront = ui->slider_StartValue_Front->value();
    int windowWidthFront = ui->slider_WindowWidth_Front->value();
    int layerNrFront = ui->slider_LayerNr_Front->value();
    int tresholdFront = ui->Slider_Threshold_Front->value();


    if (false){
        // ------- Top-View -------------------------------------------------------------
        for (int i = 0; i < tmp_im3D.width; i++){
            for (int j = 0; j < tmp_im3D.height; j++){
                int index = j * tmp_im3D.width + i + tmp_im3D.width*tmp_im3D.height*layerNr ;
                int HU_value = tmp_im3D.pImage[index];
                int iGrauwert;
                int error_stat = MyLib::windowing(HU_value, startValue, windowWidth, iGrauwert);
                image.setPixel(i,j,qRgb(iGrauwert, iGrauwert, iGrauwert));
            }
        }
    }
    if(ui->radioButton_ViewLayersFront->isChecked()){
        //-------------- front view -----------------------------------------------------
        for (int i = 0; i < 512; i++){
            for (int z = 0; z < 512; z++){
                int index = layerNrFront * 512 + i + 512*512*z ;
                int HU_value = tmp_im3D.pImage[index];
                int iGrauwert;
                int error_stat = MyLib::windowing(HU_value, startValueFront, windowWidthFront, iGrauwert);
                imageFront.setPixel(i,z,qRgb(iGrauwert, iGrauwert, iGrauwert));
            }
        }
    }
    if (false) {
        //-------------- top view -----------------------------------------------------
        bool stat = m_pData->calculateDepthMap(treshold);
        const image2D* tmpDepthMap = m_pData->getDepthMap();
        image2D im2D = image2D(tmpDepthMap->width,tmpDepthMap->height);
        stat = MyLib::calc3Dreflection(tmpDepthMap, im2D);
        for (int i = 0; i < tmpDepthMap->width; i++){
            for (int j = 0; j < tmpDepthMap->height; j++){
                int iReflection = tmpDepthMap->pImage[i + tmpDepthMap->width*j];
                image.setPixel(i,j,qRgb(iReflection, iReflection, iReflection));
            }
        }
    }
    if (ui->radioButton_3DFront->isChecked()) {
        //-------------- front view -----------------------------------------------------
        bool stat = m_pData->calculateDepthMapFront(tresholdFront);
        const image2D* tmpDepthMap = m_pData->getDepthMapFront();
        image2D im2D = image2D(tmpDepthMap->width,tmpDepthMap->height);
        stat = MyLib::calc3Dreflection(tmpDepthMap, im2D);
        for (int i = 0; i < tmpDepthMap->width; i++){
            for (int z = 0; z < tmpDepthMap->height; z++){
                int iReflection = tmpDepthMap->pImage[i + tmpDepthMap->width*z];
                imageFront.setPixel(i,z,qRgb(iReflection, iReflection, iReflection));
            }
        }
    }
    if (false) {
        //-------------- top view -----------------------------------------------------
        bool stat = m_pData->calculateDepthMap(treshold);
        const image2D* tmpDepthMap = m_pData->getDepthMap();
        for (int i = 0; i < tmpDepthMap->width; i++){
            for (int j = 0; j < tmpDepthMap->height; j++){
                int iDepth = tmpDepthMap->pImage[i + tmpDepthMap->width*j];
                image.setPixel(i,j,qRgb(iDepth, iDepth, iDepth));
            }
        }
    }
    if (ui->radioButton_DepthMapFront->isChecked()) {

    }


    // ------- draw lines & squares showing the current layers -------------------------------
    // draw a yellow line which shows the front layer view
    for (int i = 0; i < 512; i++) {
        image.setPixel(i,layerNrFront,qRgb(255, 255, 0));
    }
    // draw a green square around the top view
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 3; j++){
            imageFront.setPixel(i,j,qRgb(255, 255, 0));
            imageFront.setPixel(j,i,qRgb(255, 255, 0));
            imageFront.setPixel(i,511-j,qRgb(255, 255, 0));
            imageFront.setPixel(511-j,i,qRgb(255, 255, 0));
        }
    }
    // draw a green line which shows the top layer view
    for (int i = 0; i < 512; i++) {
        imageFront.setPixel(i,layerNr,qRgb(0, 255, 0));
    }
    // draw a green square around the top view
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 3; j++){
            image.setPixel(i,j,qRgb(0, 255, 0));
            image.setPixel(j,i,qRgb(0, 255, 0));
            image.setPixel(i,511-j,qRgb(0, 255, 0));
            image.setPixel(511-j,i,qRgb(0, 255, 0));
        }
    }

    // ------- draw selected points and the connecting line ------------------------
    if (drawLine){ // draw the line connecting first point to the second point in top view
        for (float i = 0; i < 512; i++) {
            float tanLine = (firstPoint.y - secPoint.y)/(firstPoint.x - secPoint.x);
            float dj = (i-secPoint.x)* tanLine + secPoint.y;
            int j = (int)dj;
            image.setPixel(i,j,qRgb(255, 0, 0));
        }
        for (float j = 0; j < 512; j++) {
            float tanLine = (firstPoint.x - secPoint.x)/(firstPoint.y - secPoint.y);
            float di = (j-secPoint.y)* tanLine + secPoint.x;
            int i = (int)di;
            image.setPixel(i,j,qRgb(255, 0, 0));
        }
        // draw a circle around the second point
        for (int i = 0; i<=512 ; i++){
                for (int j = 0; j<=512 ; j++){
                    if(pow(i- secPoint.x,2) + pow(j-secPoint.y,2) < 36 ){
                        image.setPixel(i,j, qRgb(255, 0, 0));
                    }
                }
         }
        // draw a line which visualize the second point selected in the top view in front view
        for (int z = 0; z < 512 ; z++){
            imageFront.setPixel(secPoint.x,z, qRgb(255, 0, 0));
         }
    }
    if (countPoint==2 || countPoint==3) // first point got selected/updated
    {
        // draw a circle around the first point
        for (int i = 0; i < 512 ; i++){
                for (int j = 0; j < 512 ; j++){
                    if(pow(i- firstPoint.x,2) + pow(j-firstPoint.y,2) < 36 ){
                        image.setPixel(i,j, qRgb(255, 0, 0));
                    }
                }
         }
        // draw a line which visualize the first point selected in the top view in front view
        for (int z = 0; z < 512 ; z++){
            imageFront.setPixel(firstPoint.x,z, qRgb(255, 0, 0));
         }
    }



    if (drawLineFront){ // draw the line connecting first point to the second point in front view
        for (float i = 0; i < 512; i++) {
            float tanLine = (firstPointFront.z - secPointFront.z)/(firstPointFront.x - secPointFront.x);
            float dj = (i-secPointFront.x)* tanLine + secPointFront.z;
            int j = (int)dj;
            imageFront.setPixel(i,j,qRgb(255, 0, 0));
        }
        for (float z = 0; z < 512; z++) {
            float tanLine = (firstPointFront.x - secPointFront.x)/(firstPointFront.z - secPointFront.z);
            float di = (z-secPointFront.z)* tanLine + secPointFront.x;
            int i = (int)di;
            imageFront.setPixel(i,z,qRgb(255, 0, 0));
        }
        // draw a circle around the second point in front view
        for (int i = 0; i<=512 ; i++){
                for (int z = 0; z<=512 ; z++){
                    if(pow(i- secPointFront.x,2) + pow(z-secPointFront.z,2) < 36 ){
                        imageFront.setPixel(i,z, qRgb(255, 0, 0));
                    }
                }
         }
        // draw a line which visualize the second point selected in the front view in top view
        for (int j = 0; j < 512 ; j++){
            image.setPixel(secPointFront.x,j, qRgb(255, 0, 0));
         }
    }
    if (countPointFront==2 || countPointFront==3) // first point got selected/updated
    {
        // draw a circle around the first point in front view
        for (int i = 0; i<=512 ; i++){
                for (int z = 0; z<=512 ; z++){
                    if(pow(i- firstPointFront.x,2) + pow(z-firstPointFront.z,2) < 36 ){
                        imageFront.setPixel(i,z, qRgb(255, 0, 0));
                    }
                }
         }
        // draw a line which visualize the first point selected in the front view in top view
        for (int j = 0; j < 512 ; j++){
            image.setPixel(firstPointFront.x,j, qRgb(255, 0, 0));
         }
    }


    // ------- update pics in the GUI -----------------------------------------
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_image->setPixmap(QPixmap::fromImage(image));
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_image_front->setPixmap(QPixmap::fromImage(imageFront));


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
            if (iIndex>=0 && iIndex< 512*512*512){
                int iHuVal = tmp_im3D.pImage[x+y*512+z*512*512];
                int error_stat = MyLib::windowing(iHuVal, startValue, windowWidth, iGrayVal);
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

// reserve functions --------------------------------------------------------
void ImageLoader::update3Dreflection(){
    int treshold = ui->Slider_Threshold->value();
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
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}
void ImageLoader::update3DreflectionFront(){
    int tresholdFront = ui->Slider_Threshold_Front->value();
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
    ui->label_image_front->setPixmap(QPixmap::fromImage(image));
}

// GUI functions ------------------------------------------------------------
void ImageLoader::updateWindowingStart(int value){
    ui->label_Start->setText("Start: " + QString::number(value));
    updateView();
}
void ImageLoader::updateWindowingWidth(int value){
    ui->label_Width->setText("Width: " + QString::number(value));
    updateView();
}
void ImageLoader::updateLayerNr(int value){
    ui->label_Layer->setText("Layer: " + QString::number(value));
    updateView();
}
void ImageLoader::updateTreshold(int value){
    ui->label_Threshold->setText("Threshold: " + QString::number(value));
    updateView();
}
void ImageLoader::updateWindowingStartFront(int value){
    ui->label_Start_Front->setText("Start: " + QString::number(value));
    updateView();
}
void ImageLoader::updateWindowingWidthFront(int value){
    ui->label_Width_Front->setText("Width: " + QString::number(value));
    updateView();
}
void ImageLoader::updateLayerNrFront(int value){
    ui->label_Layer_Front->setText("Layer: " + QString::number(value));
    updateView();
}
void ImageLoader::updateTresholdFront(int value){
    ui->label_Threshold_Front->setText("Threshold: " + QString::number(value));
    updateView();
}
void ImageLoader::updateAllSlidernLabels(){
    // update text labels at the beginning
    int startValue = ui->slider_StartValue->value();
    int windowWidth = ui->slider_WindowWidth->value();
    int layerNr = ui->slider_LayerNr->value();
    int treshold = ui->Slider_Threshold->value();
    ui->label_Start->setText("Start: " + QString::number(startValue));
    ui->label_Width->setText("Width: " + QString::number(windowWidth));
    ui->label_Layer->setText("Layer: " + QString::number(layerNr));
    ui->label_Threshold->setText("Threshold: " + QString::number(treshold));

    int startValueFront = ui->slider_StartValue_Front->value();
    int windowWidthFront = ui->slider_WindowWidth_Front->value();
    int layerNrFront = ui->slider_LayerNr_Front->value();
    int tresholdFront = ui->Slider_Threshold_Front->value();
    ui->label_Start_Front->setText("Start: " + QString::number(startValueFront));
    ui->label_Width_Front->setText("Width: " + QString::number(windowWidthFront));
    ui->label_Layer_Front->setText("Layer: " + QString::number(layerNrFront));
    ui->label_Threshold->setText("Threshold: " + QString::number(tresholdFront));
}
