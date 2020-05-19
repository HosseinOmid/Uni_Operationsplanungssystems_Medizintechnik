#include "imageloader.h"
#include "ui_imageloader.h"
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>
#include <cmath>

ImageLoader::ImageLoader(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton_Pixel, SIGNAL(clicked()), this, SLOT(MalePixel()));
    connect(ui->pushButton_12bit, SIGNAL(clicked()), this, SLOT(MalePixel_12bit()));
    connect(ui->pushButton_load3D, SIGNAL(clicked()), this, SLOT(MalePixel_3D()));
    connect(ui->pushButton_Tiefenkarte, SIGNAL(clicked()), this, SLOT(updateTiefenkarteView()));
    connect(ui->pushButton_3D, SIGNAL(clicked()), this, SLOT(update3Dreflection()));


    connect(ui->slider_StartValue, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingStart(int)));
    connect(ui->slider_WindowWidth, SIGNAL(valueChanged(int)), this, SLOT(updateWindowingWidth(int)));
    connect(ui->slider_LayerNr, SIGNAL(valueChanged(int)), this, SLOT(updateLayerNr(int)));
    connect(ui->Slider_Treshold, SIGNAL(valueChanged(int)), this, SLOT(updateTreshold(int)));



    // Speicher der Größe 512*512 reservieren
    m_pImageData = new short[512*512];
    m_pImageData3D = new short[512*512*130];
    tiefenkarte = new short[512*512];
    //default values for the sliders
    //ui->slider_WindowWidth->setValue(1000);
    //ui->slider_WindowWidth->setValue(1000);

    data2Dloaded = false ;

}

ImageLoader::~ImageLoader()
{
    delete ui;
    // Speicher wieder freigeben
    delete[] m_pImageData;
    delete[] m_pImageData3D;
    delete[] tiefenkarte;

}
int ImageLoader::windowing(int HU_value, int startValue, int windowWidth)
{
    int iGrauwert = 0;
    //Fensterung berechnen
    if (HU_value< startValue)
        iGrauwert = 0;
    if (HU_value> startValue+windowWidth)
        iGrauwert = 255;
    if (HU_value>= startValue  && HU_value<= startValue+windowWidth)
        iGrauwert = 255.0 / windowWidth * (HU_value - startValue);
    return iGrauwert;
}

void ImageLoader::updateWindowingStart(int value)
{
    ui->label_Start->setText("Start: " + QString::number(value));
    updateView();
}
void ImageLoader::updateWindowingWidth(int value)
{
    ui->label_Width->setText("Width: " + QString::number(value));
    updateView();
}
void ImageLoader::updateLayerNr(int value)
{
    ui->label_Layer->setText("Layer: " + QString::number(value));
    updateView();
}
void ImageLoader::updateTreshold(int value)
{
    ui->label_Treshold->setText("Treshold: " + QString::number(value));
    updateView();
}



void ImageLoader::update3Dreflection()
{
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(512,512, QImage::Format_RGB32);
    for (int i = 1; i < 511; i++)
    {
        for (int j = 1; j < 511; j++)
        {
            //int iTiefe = tiefenkarte[j * 512 + i];
            int sx = 2;
            int sy = 2;
            int leftXNeighbor  = tiefenkarte[j * 512 + i-1];
            int rightXNeighbor = tiefenkarte[j * 512 + i+1];
            int leftYNeighbor  = tiefenkarte[(j-1) * 512 + i];
            int rightYNeighbor = tiefenkarte[(j+1) * 512 + i];
            int Tx = leftXNeighbor - rightXNeighbor;
            int Ty = leftYNeighbor - rightYNeighbor ;

            int iReflection = 255* (sx*sy) / std::pow(std::pow(sy*Tx,2) + std::pow(sx*Ty,2) + std::pow(sx*sy,2),0.5) ;

            image.setPixel(i,j,qRgb(iReflection, iReflection, iReflection));

        }
    }
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}
void ImageLoader::updateTiefenkarteView()
{
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(512,512, QImage::Format_RGB32);

    int treshold_red = ui->Slider_Treshold->value();

    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            int iTiefe = 129; // initialize
            for (int iLayer = 129; iLayer>= 0; iLayer--)
            {
                int index = j * 512 + i + 512*512*iLayer ;
                int HU_value = m_pImageData3D[index];
                if(HU_value >treshold_red)
                {
                    iTiefe = 129 - iLayer;
                    break;
                }
            }
            tiefenkarte[j * 512 + i] = iTiefe;
            image.setPixel(i,j,qRgb(iTiefe, iTiefe, iTiefe));
        }
    }
    // Bild auf Benutzeroberfläche anzeigen
    ui->label_image->setPixmap(QPixmap::fromImage(image));

}
void ImageLoader::updateView()
{
    if (data2Dloaded)
    {
        update2DView();
    }
    else
    {
        update3DView();
    }
}
void ImageLoader::update3DView()
{
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(512,512, QImage::Format_RGB32);

    int startValue = ui->slider_StartValue->value();
    int windowWidth = ui->slider_WindowWidth->value();

    int layerNr = ui->slider_LayerNr->value();

    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            int index = j * 512 + i + 512*512*layerNr ;
            int HU_value = m_pImageData3D[index];
            int iGrauwert = windowing(HU_value, startValue, windowWidth);
            image.setPixel(i,j,qRgb(iGrauwert, iGrauwert, iGrauwert));
        }
    }

    // Bild auf Benutzeroberfläche anzeigen
    ui->label_image->setPixmap(QPixmap::fromImage(image));

}
void ImageLoader::update2DView()
{
    // Erzeugen ein Objekt vom Typ QImage
    QImage image(512,512, QImage::Format_RGB32);

    int startValue = ui->slider_StartValue->value();
    int windowWidth = ui->slider_WindowWidth->value();

    int treshold_red = ui->Slider_Treshold->value();

    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            int index = j * 512 + i ;
            int HU_value = m_pImageData[index];
            int iGrauwert = windowing(HU_value, startValue, windowWidth);
            if (HU_value < treshold_red)
            {
                image.setPixel(i,j,qRgb(iGrauwert, iGrauwert, iGrauwert));

            }
            else
            {
                image.setPixel(i,j,qRgb(255, 0, 0));
            }
        }
    }

    // Bild auf Benutzeroberfläche anzeigen
    ui->label_image->setPixmap(QPixmap::fromImage(image));

}

void ImageLoader::MalePixel_3D()
{
    // Pfad zu der Datei festlegen
    QFile dataFile("D:/OneDrive/Uni_Unterlagen/2020SS_SoftwareEntwicklung/Zusatzmaterial/Kopf_CT_130.raw");

    bool bFileOpen = dataFile.open(QIODevice::ReadOnly);
    if (!bFileOpen)
    {
        //QMessageBox::critical(this, "ACHTUNG", "Datei konnte nicht geöffnet werden");
        return;
    }

    // Datei lesen
    int iNumberBytesRead = dataFile.read((char*)m_pImageData3D, 512*512*130*sizeof(short));
    // zum vergleich der Dateilänge mit der eingelesenen Länge
    int iFileSize = dataFile.size();

    dataFile.close();

    data2Dloaded = false;

    update3DView();
}
void ImageLoader::MalePixel_12bit()
{
    // Pfad zu der Datei festlegen
    QFile dataFile("D:/OneDrive/Uni_Unterlagen/2020SS_SoftwareEntwicklung/Zusatzmaterial/12bit_short.raw");

    bool bFileOpen = dataFile.open(QIODevice::ReadOnly);
    if (!bFileOpen)
    {
        //QMessageBox::critical(this, "ACHTUNG", "Datei konnte nicht geöffnet werden");
        return;
    }

    // Datei lesen
    int iNumberBytesRead = dataFile.read((char*)m_pImageData, 512*512*sizeof(short));
    // zum vergleich der Dateilänge mit der eingelesenen Länge
    int iFileSize = dataFile.size();

    dataFile.close();

    data2Dloaded = true;

    update2DView();
}

void ImageLoader::MalePixel()
{
    //ui->pushButton_Pixel->setText("Funktioniert");

    // Erzeugen ein Objekt vom Typ QImage
    QImage image(512,512, QImage::Format_RGB32);

    // reserviere zusammenhängenden Speicherbereich der Größe 512*512
    char imageData[512*512];

    // Pfad zu der Datei festlegen
    QFile dataFile("D:/OneDrive/Uni_Unterlagen/2020SS_SoftwareEntwicklung/Programme/Aufgabe1/Aufgabe1/8bit.raw");

    bool bFileOpen = dataFile.open(QIODevice::ReadOnly);
    if (!bFileOpen)
    {
        //QMessageBox::critical(this, "ACHTUNG", "Datei konnte nicht geöffnet werden");
        return;
    }


    // Datei lesen
    int iNumberBytesRead = dataFile.read(imageData, 512*512);
    // zum vergleich der Dateilänge mit der eingelesenen Länge
    int iFileSize = dataFile.size();

    dataFile.close();

    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            int index = j * 512 + i ;
            int iGrauwert = imageData[index];
            image.setPixel(i,j,qRgb(iGrauwert, iGrauwert, iGrauwert));
        }
    }

    // Bild auf Benutzeroberfläche anzeigen
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}
