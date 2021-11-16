#ifndef CWORKERTHREAD_H
#define CWORKERTHREAD_H

#include <QObject>
#include "ccapteurpassage.h"

#define GPIO_CAPT1 4 // à définir précisément !
#define GPIO_CAPT2 6

class CWorkerThread : public QObject
{
    Q_OBJECT
public:
    explicit CWorkerThread(QObject *parent = nullptr);
    ~CWorkerThread();

private:
    bool _finCourse;
    bool _fin1, _fin2; // fin des coureurs
    bool _send1, _send2;
    CCapteurPassage *_cell1, *_cell2;

signals:
    void sig_coureurArrived(int no, QString temps);
    void sig_finCourse();

public slots:
    void run();  // attends que les deux coureurs soient arrivés
    void on_stopCourse();
};

#endif // CWORKERTHREAD_H
