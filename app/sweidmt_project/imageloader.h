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

    Reconstruction reco;
    vector planeNormalVector;
    point firstPoint;
    point secPoint;
    point sliceFrameSize;
    int countPoint;
    bool bothPointsAreSelected;
    bool drillTrajectoryIsDefined;
    double drillDiameter;
    double scale;
    image2D* reco_im2D;

private slots:
    void loadData();

    void updateView();

    void reconstructSlice();
    double calculateDrillLength();
    void resetPoints();

    void mousePressEvent(QMouseEvent *event);
    void update3DreflectionXY();
    void update3DreflectionXZ();
    void updateDepthMapXY();
    void updateDepthMapXZ();

    void updateWindowingStartXY(int value);
    void updateWindowingWidthXY(int value);
    void updateLayerNrXY(int value);
    void updateTresholdXY_2(int value);

    void updateAllSlidernLabels();
    void updateWindowingStartXZ(int value);
    void updateWindowingWidthXZ(int value);
    void updateLayerNrXZ(int value);
    void updateTresholdXZ_2(int value);

    void updateScale(int value);
    void updateAllLabelsDescribingSlice();
    void updateWindowingStartSlice(int value);
    void updateWindowingWidthSlice(int value);
    void updateSliceDepth(int value);

signals:
    void LOG(QString str);
};
#endif // ImageLoader // old name: MAINWINDOW_H
