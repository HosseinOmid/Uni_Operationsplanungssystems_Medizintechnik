#ifndef SWEIDMT_H
#define SWEIDMT_H

#include <QWidget>
// aus Aufgabe 2
#include "imageloader.h"

namespace Ui {
class SWEIDMT;
}

class SWEIDMT : public QWidget
{
    Q_OBJECT

public:
    explicit SWEIDMT(QWidget *parent = nullptr);
    ~SWEIDMT();

private:
    Ui::SWEIDMT *ui;
    ImageLoader *m_pWidget;
};

#endif // SWEIDMT_H
