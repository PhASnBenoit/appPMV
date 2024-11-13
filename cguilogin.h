#ifndef CGUILOGIN_H
#define CGUILOGIN_H

#include <QDialog>
#include <QCloseEvent>
#include <QtWidgets>
#include <QPushButton>
#include <QDebug>
#include <QString>
#include <QInputDialog>

#include <QRandomGenerator>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


namespace Ui {
class CGuiLogin;
}

class CGuiLogin : public QDialog
{
    Q_OBJECT

public:
    explicit CGuiLogin(QWidget *parent = nullptr, QString *codePIN = nullptr);
    ~CGuiLogin() override;

private:
    Ui::CGuiLogin *ui;
//    bool _bascule;
    QString *_codePIN;

signals:

private slots:
     // Réservé au code à six chiffres
    void digit_pressed();
    void on_pbEffacer_clicked();

public slots:

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // CGUILOGIN_H
