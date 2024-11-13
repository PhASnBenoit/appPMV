#ifndef CLOGINNUMBERSDIALOG_H
#define CLOGINNUMBERSDIALOG_H

#include <QWidget>
#include <QMessageBox>
#include <QCloseEvent>

class CLoginNumbersDialog : public QWidget
{
    Q_OBJECT
public:
    explicit CLoginNumbersDialog(QWidget *parent = nullptr);
    ~CLoginNumbersDialog();

private:
    Ui::CLoginNumbersDialog *ui;

signals:


private slots:


public slots:
    void on_credentials(bool state);

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // CLOGINNUMBERSDIALOG_H
