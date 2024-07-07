#ifndef ROTATERIGHT_H
#define ROTATERIGHT_H

#include <QObject>
#include "device.h"

class RotateRight : public Device
{
    Q_OBJECT
public:
    Item *item;
    explicit RotateRight(int x, int y, int type, QObject *parent = nullptr, bool virt = false);

    void update(int flip) override;
    bool output(bool mark=false) override;
    bool checkIpt(int ix, int iy, QVector<Item *> &ipt) override;
    void input(QVector<Item *> &ipt, int ix=0, int iy=0) override;
};

#endif // ROTATERIGHT_H
