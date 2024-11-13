#ifndef CGUITYPECOURSE_H
#define CGUITYPECOURSE_H

#include <QDialog>
#include "czdc.h"

namespace Ui {
class CGuiTypeCourse;
}

class CGuiTypeCourse : public QDialog
{
    Q_OBJECT

public:
    explicit CGuiTypeCourse(QWidget *parent = nullptr, T_TYPE_COURSE *type = nullptr);
    ~CGuiTypeCourse();

private slots:
    void on_pb2c_clicked();
    void on_pb1c_clicked();

private:
    Ui::CGuiTypeCourse *ui;
    T_TYPE_COURSE *_type;
};

#endif // CGUITYPECOURSE_H
