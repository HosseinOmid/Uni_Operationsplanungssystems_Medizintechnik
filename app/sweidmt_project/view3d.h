#ifndef VIEW3D_H
#define VIEW3D_H

#include <QWidget>

namespace Ui {
class View3D;
}

class View3D : public QWidget
{
    Q_OBJECT

public:
    explicit View3D(QWidget *parent = nullptr);
    ~View3D();

private:
    Ui::View3D *ui;
};

#endif // VIEW3D_H
