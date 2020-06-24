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

    m_pView3D = new View3D(this);
    ui->tabWidget->addTab(m_pView3D, "View Image");

    connect(m_pWidget, SIGNAL(LOG(QString)), this, SLOT(LOG(QString)));
}

//void methode1(){}


SWEIDMT::~SWEIDMT()
{
    delete ui;
    delete m_pWidget;
}


void SWEIDMT::LOG(QString str)
{
    ui->textEdit->append(str);
}
