#include "miner.h"

Miner::Miner(int x, int y, int type, QObject *parent, bool virt)
    : Device(x, y, 1, 1, type, MINER_IMG, parent, virt),
    item(nullptr)
{

}
Miner::~Miner(){
    if(item != nullptr)
        delete item;
}
int Miner::getMineType(){
    return type>>7;
}
void Miner::update(int flip){
    updateMark(flip);
    if(item == nullptr){
        item = new Item(x*PX, y*PX, getMineType(), parent());
        item->setShow(false);
    }
    flipCount ++;
    if(flipCount > baseInterval-5*minerGrade){
        flipCount = 0;
        output();
    }
}
QString Miner::getMineImg(){
    int tp = (type>>8)&0b1111;
    if(tp == 1){
        return ROUND_MINE_IMG;
    }else if(tp == 2){
        return RECT_MINE_IMG;
    }else if(tp == 3){
        return BUBBLE_MINE_IMG;
    }else return "";
}
void Miner::draw(QPainter &painter){
    if(imgPath.size() == 0) return ;
    QPixmap pix = QPixmap(imgPath). \
                  transformed(QTransform().rotate(90*getDirect()));
    if(selected){
        painter.setPen(QPen(QColor(selectR, selectG, selectB, 127), 4, Qt::SolidLine));
        painter.setBrush(QBrush(QColor(selectR, selectG, selectB, 63), Qt::SolidPattern));
        painter.drawRect(BaseX+x*PX, BaseY+y*PX, w*PX, h*PX);
    }
    painter.drawPixmap(
        QRect(BaseX+x*PX+MA, BaseY+y*PX+MA, w*PX-2*MA, h*PX-2*MA),
        getMineImg()
        );
    painter.drawPixmap(
        QRect(BaseX+x*PX+MA, BaseY+y*PX+MA, w*PX-2*MA, h*PX-2*MA),
        pix
        );
    painter.save();
}
bool Miner::output(bool mark){
    auto dev = devicesMap.find(getOutput());
    if(dev == devicesMap.end()){
        return false;
    }else{
        if(mark){
            auto device = dev.value();
            QVector<Item *> opt({Item::markType(x*PX, y*PX)});
            if(device->checkIpt(x, y, opt)){
                device->input(opt, x, y);
                return true;
            }else return false;
        }else{
            auto device = dev.value();
            QVector<Item *> opt({item});
            if(device->checkIpt(x, y, opt)){
                device->input(opt, x, y);
                item = nullptr;
                return true;
            }else return false;
        }
    }
}
bool Miner::checkIpt(int , int , QVector<Item *>&){
    return false;
}
