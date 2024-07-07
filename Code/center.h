#ifndef CENTER_H
#define CENTER_H

#include "device.h"

class Center : public Device
{
    Q_OBJECT
public:
    Item reqItem;
    int reqType, reqNum; // 记录需求
    explicit Center(int x, int y, QObject *parent = nullptr);
    void setRequire(int pro, bool reset=true);
    void input(QVector<Item *> &ipt, int ix=0, int iy=0) override;
    void draw(QPainter &painter) override;

signals:
    void finishSubsection();
    void finishSection();
};

#endif // CENTER_H
