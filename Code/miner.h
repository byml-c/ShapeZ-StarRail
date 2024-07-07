#ifndef MINER_H
#define MINER_H

#include <QObject>

#include "device.h"

class Miner : public Device
{
    Q_OBJECT
public:
    Item *item;
    explicit Miner(int x, int y, int type, QObject *parent = nullptr, bool virt = false);
    ~Miner();
    void update(int flip) override;
    bool output(bool mark=false) override;
    void draw(QPainter &painter) override;
    bool checkIpt(int ix, int iy, QVector<Item *> &ipt) override;
    int getMineType();
    QString getMineImg(); // 获得矿物的贴图路径
signals:
};

#endif // MINER_H
