#ifndef CANEMOMETRE_H
#define CANEMOMETRE_H

#include <QObject>

class CAnemometre : public QObject
{
    Q_OBJECT
public:
    explicit CAnemometre(QObject *parent = nullptr);
    float lireVent();

private:
    void init();

signals:

public slots:
};

#endif // CANEMOMETRE_H
