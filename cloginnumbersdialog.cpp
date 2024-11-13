#include "cloginnumbersdialog.h"

CLoginNumbersDialog::CLoginNumbersDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CLoginNumbersDialog)
{
    ui->setup(this);
}
