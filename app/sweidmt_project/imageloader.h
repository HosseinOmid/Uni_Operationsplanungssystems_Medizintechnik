#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>
#include <cmath>
#include <QMainWindow>
#include "applicationdata.h"
#include "mylib.h"
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ImageLoader : public QMainWindow
{
    Q_OBJECT

public:
    ImageLoader(QWidget *parent = nullptr);
    ~ImageLoader();
    void setData(ApplicationData* pData);

private:
    Ui::MainWindow *ui;
    ApplicationData *m_pData;

    //short* m_pImageData;
    //short* m_pImageData3D;
    //short* tiefenkarte;

    //int windowing(int HU_value, int startValue, int windowWidth);

private slots:
    void MalePixel_3D();
    void updateWindowingStart(int value);
    void updateWindowingWidth(int value);
    void updateLayerNr(int value);
    void updateTreshold(int value);


    void update3DView();
    void updateView();
    void updateTiefenkarteView();
    void update3Dreflection();

signals:
    void LOG(QString str);
};
#endif // ImageLoader // old name: MAINWINDOW_H
