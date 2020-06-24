#include "sweidmt.h"
#include "ui_sweidmt.h"

SWEIDMT::SWEIDMT(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SWEIDMT)
{
    ui->setupUi(this);
    m_pWidget = new ImageLoader(this);
    ui->tabWidget->addTab(m_pWidget, "Load Image");
    m_pWidget->setData(&m_data);
}


//void methode1(){}


SWEIDMT::~SWEIDMT()
{
    delete ui;
    delete m_pWidget;
}
