#include "cguitypecourse.h"
#include "ui_cguitypecourse.h"

CGuiTypeCourse::CGuiTypeCourse(QWidget *parent, T_TYPE_COURSE *type) :
    QDialog(parent),
    ui(new Ui::CGuiTypeCourse)
{
    ui->setupUi(this);
    _type = type;
}

CGuiTypeCourse::~CGuiTypeCourse()
{
    delete ui;
}

void CGuiTypeCourse::on_pb2c_clicked()
{
    *_type = DEPART_ARRETE;
    emit accept();
}

void CGuiTypeCourse::on_pb1c_clicked()
{
    *_type = DEPART_LANCE;
    emit accept();
}
