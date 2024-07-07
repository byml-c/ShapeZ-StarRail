#include "item.h"

QList<Item *> items;
Item::Item(int rx, int ry, int type, QObject *parent)
    : QObject{parent}, show(true), rx(rx), ry(ry),
    rotation(0), proc(0)
{
    setType(type);
    items.append(this);
}
Item::~Item(){
    items.removeOne(this);
}

int Item::getDirType(int direct){
    return (type>>(1+4*(3-direct)))&0b1111;
}
QString Item::getDirImage(int tp){
    switch(tp){
    case 1:
        return ROUND_IMG;
    case 2:
        return RECT_IMG;
    case 3:
        return BUBBLE_IMG;
    case 4:
        return MARK_IMG;
    default:
        return "";
    }
}
void Item::setDirType(int d0, int d1, int d2, int d3){
    int d[4] = {d0, d1, d2, d3};

    type = (type>>17);
    for(int i=0;i<4;++i)
        type = (type<<4)|d[i];
    type <<= 1;
}
void Item::draw(QPainter &painter){
    if(!show) return ;

    int tp = getDirType(3);
    if(tp != 4){
        painter.drawPixmap(
            QRect(BaseX+rx, BaseY+ry, 80, 80),
            QPixmap(BASE_IMG)
            );
        if(tp == 3){
            painter.drawPixmap(
                QRect(BaseX+rx+22, BaseY+ry+24, 36, 34),
                QPixmap(getDirImage(tp))
                );
        }else{
            if(tp=getDirType(0), tp!=0){
                painter.drawPixmap(
                    QRect(BaseX+rx+18, BaseY+ry+18, 20, 20),
                    QPixmap(getDirImage(tp))
                    );
            }
            if(tp=getDirType(1), tp!=0){
                painter.drawPixmap(
                    QRect(BaseX+rx+42, BaseY+ry+18, 20, 20),
                    QPixmap(getDirImage(tp)).transformed(
                        QTransform().rotate(90))
                    );
            }
            if(tp=getDirType(2), tp!=0){
                painter.drawPixmap(
                    QRect(BaseX+rx+42, BaseY+ry+42, 20, 20),
                    QPixmap(getDirImage(tp)).transformed(
                        QTransform().rotate(180))
                    );
            }
            if(tp=getDirType(3), tp!=0){
                painter.drawPixmap(
                    QRect(BaseX+rx+18, BaseY+ry+42, 20, 20),
                    QPixmap(getDirImage(tp)).transformed(
                        QTransform().rotate(270))
                    );
            }
        }
    }else{
        double temp = 45+(rotation%90+90)%90;
        temp = fabs(sqrt(2)*20.0*sin(temp/180.0*acos(-1.0)));
        int sz = ceil(temp);
        painter.drawPixmap(
            QRect(BaseX+rx+(80-sz)/2, BaseY+ry+(80-sz)/2, sz, sz),
            QPixmap(getDirImage(tp)).transformed(
                QTransform().rotate(rotation))
            );
    }
    painter.save();
}
void Item::setType(int typ){
    type = typ;
}
int Item::getType(){
    return type;
}
void Item::setPos(QPoint p){
    rx = p.x(); ry = p.y();
}
void Item::setPos(int nx, int ny){
    rx = nx; ry = ny;
}
void Item::addPos(int vx, int vy){
    rx += vx; ry += vy;
}
bool Item::isSplittable(){
    return (type>>17)&1;
}
void Item::setProcess(int pro){
    proc = pro;
}
int Item::getProcess(){
    return proc;
}
void Item::addProcess(int pro){
    proc += pro;
}
void Item::setShow(bool s){
    show = s;
}
void Item::setRotation(int rot){
    rotation = rot;
}
bool Item::isMark(){
    return getDirType(3) == 4;
}
Item* Item::markType(int rx, int ry, QObject* parent){
    return new Item(rx, ry, 0b000000000000001000, parent);
}
void Item::resetProcess(){
    proc = proc%PX;
}
