#ifndef COMBINER_H
#define COMBINER_H

#include "device.h"

class Combiner : public Device
{
public:
    Item *item1, *item2;
    explicit Combiner(int x, int y, int type, QObject *parent = nullptr, bool virt = false);

    void update(int flip) override;
    bool output(bool mark=false) override;
    bool checkIpt(int ix, int iy, QVector<Item *> &ipt) override;
    void input(QVector<Item *> &ipt, int ix, int iy) override;
};

#endif // COMBINER_H
