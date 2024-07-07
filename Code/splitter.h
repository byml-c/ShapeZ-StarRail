#ifndef SPLITTER_H
#define SPLITTER_H

#include <QObject>

#include "device.h"

class Splitter : public Device
{
    Q_OBJECT
public:
    Item *item;
    explicit Splitter(int x, int y, int type, QObject *parent = nullptr, bool virt = false);

    void update(int flip) override;
    bool output(bool mark=false) override;
    bool checkIpt(int ix, int iy, QVector<Item *> &ipt) override;
    void input(QVector<Item *> &ipt, int ix=0, int iy=0) override;
signals:
};

#endif // SPLITTER_H
