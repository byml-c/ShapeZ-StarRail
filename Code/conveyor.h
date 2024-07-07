#ifndef CONVEYOR_H
#define CONVEYOR_H

#include <QObject>
#include <QList>
#include <QPair>
#include "device.h"

class Conveyor : public Device
{
    Q_OBJECT
public:
    Item *onOpt;
    bool optFail; // 在一次更新中未能完成输出，此时不接受输入
    QList<Item *>itemList; // 同一时刻，一条传送带上按顺序有多个元素
    QVector<Item *> removeList; // 一轮更新后，离开本传送带的元素
    QPoint center; // 旋转中心
    explicit Conveyor(int x, int y, int type, QObject *parent = nullptr, bool virt = false);
    ~Conveyor();
    void draw(QPainter &painter) override;
    void update(int flip) override;
    bool output(bool mark=false) override;
    void updateMark(int flip) override;
    bool valid(); // 传送带是否有效
    bool checkIpt(int ix, int iy, QVector<Item *> &ipt) override;
    void input(QVector<Item *> &ipt, int ix=0, int iy=0) override;
    int getFrom();
    void setFrom(int fr);
    int getTo();
    void setTo(int to);
    void setDirect(int fr, int to, bool wipt=true);
    void swapDirect(); // 交换 from 和 to
    void setVirt(bool virt) override;
    void setType(int type); // 设置 type，同时给 center 赋值
    void movePace(Item *item); // 令某个元素向前一步
    QPair<int, int> getInput(); // 判断输入方向上是否存在设备
    QPoint getBase(int x, int y, int direct); // 获 direct 方向的对应的初始坐标/最终坐标
signals:
};

#endif // CONVEYOR_H
