#include "view3d.h"
#include "ui_view3d.h"

View3D::View3D(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::View3D)
{
    ui->setupUi(this);
}

View3D::~View3D()
{
    delete ui;
}
