#include "rotateright.h"

RotateRight::RotateRight(int x, int y, int type, QObject *parent, bool virt)
    : Device(x, y, 1, 1, type, ":/img/RoaterRight.png", parent, virt),
    item(nullptr)
{

}

bool RotateRight::checkIpt(int ix, int iy, QVector<Item *> &ipt){
    int fr = getDirect()^0b10;
    if(ipt.size() < 1) return false;
    if(fr==0 && (ix!=x || iy!=y-1)) return false;
    if(fr==1 && (ix!=x+1 || iy!=y)) return false;
    if(fr==2 && (ix!=x || iy!=y+1)) return false;
    if(fr==3 && (ix!=x-1 || iy!=y)) return false;
    if(ipt[0]->isMark()) return false;
    if(item != nullptr){
        if(item->type == ipt[0]->type){
            return false;
        }else return true;
    }
    return true;
}
void RotateRight::input(QVector<Item *> &ipt, int, int){
    if(item != nullptr){
        delete item;
    }
    item = ipt[0];
    item->setShow(false);

    if(!item->isSplittable()){
        setSelectColor(1);
        setSelect(true, true);
    }else{
        setSelectColor(0);
        setSelect(false, true);
    }
}
void RotateRight::update(int flip){
    updateMark(flip);
    if(item != nullptr){
        flipCount ++;
        if(flipCount > baseInterval-5*splitterGrade){
            flipCount = 0;
            output();
        }
    }else flipCount = 0;
}
bool RotateRight::output(bool mark){
    QMap<QPair<int, int>, Device *>::Iterator dev;
    dev = devicesMap.find(getOutput());

    if(dev == devicesMap.end()){
        return false;
    }else{
        auto opt = dev.value();
        if(mark){
            Item *oitem = Item::markType(x*PX, y*PX);
            oitem->setShow(false);

            QVector<Item *> optv({oitem});
            if(opt->checkIpt(x, y, optv)){
                opt->input(optv, x, y);
                return true;
            }else{
                delete oitem;
                return false;
            }
        }else{
            if(!item->isSplittable()){
                return false;
            }
            int d[4] = {0};
            for(int i=0;i<4;++i){
                d[i] = item->getDirType(i);
            }

            Item *oitem = new Item(
                x*PX, y*PX, item->getType());
            oitem->setShow(false);
            oitem->setDirType(d[3], d[0], d[1], d[2]);

            QVector<Item *> optv({oitem});
            if(opt->checkIpt(x, y, optv)){
                opt->input(optv, x, y);
                item = nullptr;
                return true;
            }else{
                delete oitem;
                return false;
            }
        }
    }
}
