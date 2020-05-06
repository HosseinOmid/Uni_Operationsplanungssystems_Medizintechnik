#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton_Pixel, SIGNAL(clicked()), this, SLOT(MalePixel()));
    connect(ui->pushButton_12bit, SIGNAL(clicked()), this, SLOT(MalePixel_12bit()));

    // Speicher der Größe 512*512 reservieren
    m_pImageData = new short[512*512];
}

MainWindow::~MainWindow()
{
    delete ui;
    // Speicher wieder freigeben
    delete[] m_pImageData;
}
int MainWindow::windowing(int HU_value, int startValue, int windowWidth)
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
void MainWindow::MalePixel_12bit()
{
    // Pfad zu der Datei festlegen
    QFile dataFile("D:/OneDrive/Uni_Unterlagen/2020SS_SoftwareEntwicklung/Programme/Aufgabe1/Aufgabe1/12bit_short.raw");

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

    // Erzeugen ein Objekt vom Typ QImage
    QImage image(512,512, QImage::Format_RGB32);

    int startValue = 800;
    int windowWidth = 400;

    for (int i = 0; i < 512; i++)
    {
        for (int j = 0; j < 512; j++)
        {
            int index = j * 512 + i ;
            int HU_value = m_pImageData[index];
            int iGrauwert = windowing(HU_value, startValue, windowWidth);
            image.setPixel(i,j,qRgb(iGrauwert, iGrauwert, iGrauwert));
        }
    }

    // Bild auf Benutzeroberfläche anzeigen
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::MalePixel()
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





    /*
    // schreibe einen Pixel
    image.setPixel(50,100, qRgb(255, 0, 0));

    // eine horizontale Linie rot malen
    int i;
    for (i = 1; i<=512 ; i++)
    {
        image.setPixel(i,50, qRgb(255, 0, 0));
    }


    // eine vertikale Linie grün malen
    for (i = 1; i<=512 ; i++)
    {
        image.setPixel(100,i, qRgb(0, 255, 0));
    }

    // eine schräge Linie mit der Steigung y = 2*x in der Farbe Blau malen
    for (i = 1; i<=512/2 ; i++)
    {
        image.setPixel(i,2*i, qRgb(0, 0, 255));
    }

    // die ganze Bildfläche rot ausmalen
    int j;
    for (i = 1; i<=512 ; i++)
    {
        for (j = 1; j<=512 ; j++)
        {
            // image.setPixel(i,j, qRgb(255, 0, 0));
        }
    }

    // den Farbwert abhängig von der Pixelposition ändern (z.B. einen Farbverlauf)
    for (i = 1; i<=512 ; i++)
    {
        for (j = 1; j<=512 ; j++)
        {
            image.setPixel(i,j, qRgb((i+j)/(2) , (i), (2*j)));
        }
    }


    // einen kreisförmigen Bereich andersfarbig malen als den Rest
    for (i = 1; i<=512 ; i++)
    {
        for (j = 1; j<=512 ; j++)
        {

            //if(pow(i-100,2) + pow(j-100,2) < 100 )
            //{
            //    image.setPixel(i,j, qRgb(255, 0, 0));
            //}
        }
    }

    */

    // Bild auf Benutzeroberfläche anzeigen
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}
