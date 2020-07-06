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

struct point{
    double x;
    double y;
    double z;
};
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ImageLoader : public QMainWindow{
    Q_OBJECT

public:
    ImageLoader(QWidget *parent = nullptr);
    ~ImageLoader();
    void setData(ApplicationData* pData);

private:
    Ui::MainWindow *ui;
    ApplicationData *m_pData;

    point firstPoint;
    point secPoint;
    int countPoint;
    bool drawLine;
    point firstPointFront;
    point secPointFront;
    int countPointFront;
    bool drawLineFront;

    //short* m_pImageData;
    //short* m_pImageData3D;
    //short* tiefenkarte;
    //short* m_pTiefenkarteFront;

private slots:
    void loadData();
    void updateWindowingStartXY(int value);
    void updateWindowingWidthXY(int value);
    void updateLayerNrXY(int value);
    void updateTresholdXY(int value);
    void updateTresholdXY_2(int value);

    void updateAllSlidernLabels();
    void updateWindowingStartXZ(int value);
    void updateWindowingWidthXZ(int value);
    void updateLayerNrXZ(int value);
    void updateTresholdXZ(int value);
    void updateTresholdXZ_2(int value);

    void updateView();
    //void updateTiefenkarteView();
    void update3DreflectionXY();
    void update3DreflectionXZ();
    void updateDepthMapXY();
    void updateDepthMapXZ();
    void reconstructSlice();

    //void mousePressEvent(QMouseEvent *event);

signals:
    void LOG(QString str);
};
#endif // ImageLoader // old name: MAINWINDOW_H
