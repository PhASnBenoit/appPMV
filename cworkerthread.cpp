#include "cworkerthread.h"
#include "../biblis/cgpio.h"

CWorkerThread::CWorkerThread(QObject *parent) : QObject(parent)
{
    _cell1 = new CCapteurPassage(GPIO_CAPT1);
    _cell2 = new CCapteurPassage(GPIO_CAPT2);
    _fin1 = _fin2 = _send1 = _send2 = _finCourse = false;
}

CWorkerThread::~CWorkerThread()
{
    delete _cell1;
    delete _cell2;
}

void CWorkerThread::run()
{
    // boucle de lecture des capteurs
    // condition de sortie : stopCourse
    while(!_finCourse) {

        if (!_send1) {
            _fin1 = _cell1->lire();// lire capteur 1
            if (_fin1) {
                _send1 = true;
                emit sig_coureurArrived(1, "fdfdf");
            } // if fin1
        } // if send1

        if (!_send2) {
            _fin2 = _cell2->lire();// lire capteur 1
            if (_fin2) {
                _send2 = true;
                emit sig_coureurArrived(2, "fdfdf");
            } // if fin2
        } // if send2

    } // while
    emit sig_finCourse();

} // method

void CWorkerThread::on_stopCourse()
{
    _finCourse = true;
} // method
