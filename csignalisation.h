#ifndef CSIGNALISATION_H
#define CSIGNALISATION_H

#include <QObject>
#include <QWidget>

class CSignalisation : public QObject
{
    Q_OBJECT
public:
    explicit CSignalisation(QObject *parent = nullptr);
    void init();
    void sequenceAvantCourse(int duree);
    void sequenceAVosMarques(int duree);
    void sequencePret(int duree);
    void sequencePartez(int duree);

private:
    void allumer();
    void eteindre();

signals:

public slots:
};

#endif // CSIGNALISATION_H
