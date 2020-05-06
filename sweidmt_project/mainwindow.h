#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    short* m_pImageData;
    int windowing(int HU_value, int startValue, int windowWidth);

private slots:
    void MalePixel();
    void MalePixel_12bit();

};
#endif // MAINWINDOW_H
