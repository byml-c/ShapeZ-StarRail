#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QPainter>
#include <QDebug>

#include "item.h"
#include "global.h"

class Device : public QObject
{
    Q_OBJECT
public:
    // 是否选中，选中加框
    bool selected;
    bool selectable;
    // 是否是虚拟出来的
    bool virt;
    // 贴图的路径
    QString imgPath;
    int x, y, w, h, type;
    // 选择框加重的颜色，0 为蓝色，1 为橙色
    int selectColor, selectR, selectG, selectB;
    // 根据等级决定速度
    int flipCount;

    explicit Device(int x, int y, int w, int h, int type, QString imgPath,
                    QObject *parent = nullptr, bool virt = false);
    virtual ~Device();
    virtual void setVirt(bool virt); // 设置是否是虚拟出来的
    int getDirect(); // 获取方向
    void setDirect(int direct, bool wipe=true); // 设置方向
    void setPos(int x, int y, bool wipe=true); // 设置位置
    void setSelectColor(int ctype=0); // 选择框的颜色，0 为蓝色，1 为橙色
    void setSelect(bool select, bool force=false); // 设置选中
    void setMap(); // 写入自身在地图上的占位
    void wipeMap(); // 删除自身在地图上的占位
    void setSize(int x, int y, int w=-1, int h=-1); // 设置位置和宽高
    void setImage(QString imgPath); // 设置图片
    bool isIn(int px, int py); // 判断坐标是否处于设备区域内
    int getDeviceType(); // 获取设备的编号

    virtual void updateMark(int flip); // 更新标记，进行计数并输出

    virtual void draw(QPainter &painter); // 将设备在画布上画出来
    virtual void input(QVector<Item *> &items, int ix=0, int iy=0); // 接收输入
    virtual bool checkIpt(int ix, int iy, QVector<Item *> &items); // 验证是否能够输入
    virtual bool checkOpt(); // 验证是否能够输出
    virtual bool output(bool mark=false); // 进行输出，控制是否只输出标志
    virtual void update(int flip); // 更新状态
    virtual QPair<int, int> getOutput(int sx=-1, int sy=-1); // 获取输出点
signals:
};

// 设备列表
extern QList<Device *> devices;
// 格子对应的设备列表
extern QMap<QPair<int, int>, Device*> devicesMap;

#endif // DEVICE_H
