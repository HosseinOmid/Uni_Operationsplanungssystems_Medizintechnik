#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ImageLoader : public QMainWindow
{
    Q_OBJECT

public:
    ImageLoader(QWidget *parent = nullptr);
    ~ImageLoader();

private:
    Ui::MainWindow *ui;
    short* m_pImageData;
    short* m_pImageData3D;
    int windowing(int HU_value, int startValue, int windowWidth);

private slots:
    void MalePixel();
    void MalePixel_12bit();
    void MalePixel_3D();
    void updateWindowingStart(int value);
    void updateWindowingWidth(int value);
    void updateLayerNr(int value);
    void update2DView();
    void update3DView();
};
#endif // ImageLoader // old name: MAINWINDOW_H
