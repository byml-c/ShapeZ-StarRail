#include "splitter.h"

Splitter::Splitter(int x, int y, int type, QObject *parent, bool virt)
    : Device(x, y, 2, 1, type, SPLITTER_IMG, parent, virt),
    item(nullptr)
{

}

bool Splitter::checkIpt(int ix, int iy, QVector<Item *> &ipt){
    int fr = getDirect()^0b10;
    if(ipt.size() < 1) return false;
    if(fr==0 && (ix!=x+1 || iy!=y-1)) return false;
    if(fr==1 && (ix!=x+1 || iy!=y+1)) return false;
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
void Splitter::input(QVector<Item *> &ipt, int, int){
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
void Splitter::update(int flip){
    updateMark(flip);
    if(item != nullptr){
        flipCount ++;
        if(flipCount > baseInterval-5*splitterGrade){
            flipCount = 0;
            output();
        }
    }else flipCount = 0;
}
bool Splitter::output(bool mark){
    QMap<QPair<int, int>, Device *>::Iterator dev1, dev2;
    dev1 = devicesMap.find(getOutput());
    if(getDirect() & 1){
        dev2 = devicesMap.find(getOutput(x, y+1));
    }else dev2 = devicesMap.find(getOutput(x+1, y));

    if(dev1 == devicesMap.end() || dev2 == devicesMap.end()){
        return false;
    }else{
        auto lf = dev1.value();
        auto rt = dev2.value();

        if(mark){
            Item *iteml = Item::markType(x*PX, y*PX);
            Item *itemr = Item::markType(x*PX, y*PX);
            QVector<Item *> optl({iteml}), optr({itemr});
            int rtx = x, rty = y;
            if(getDirect() & 1) ++rty;
            else ++rtx;
            if(lf->checkIpt(x, y, optl) && rt->checkIpt(rtx, rty, optr)){
                lf->input(optl, x, y); rt->input(optr, rtx, rty);
                return true;
            }else{
                delete iteml;
                delete itemr;
                return false;
            }
        }else{
            if(!item->isSplittable()){
                return false;
            }
            int d[4] = {0};
            for(int i=0;i<4;++i){
                d[i] = item->getDirType(i);
                // qWarning()<<x<<y<<"output split "<<i<<d[i];
            }

            Item *iteml = new Item(
                x*PX, y*PX, item->getType());
            Item *itemr = new Item(
                x*PX, y*PX, item->getType());
            iteml->setShow(false); itemr->setShow(false);
            if(getDirect() & 1){ // 左右方向，上下分割
                if((d[0]||d[1]) && (d[2]||d[3])){
                    iteml->setDirType(d[0], d[1], 0, 0);
                    itemr->setDirType(0, 0, d[2], d[3]);
                }else return false;
            }else{ // 上下方向，左右分割
                if((d[0]||d[3]) && (d[1]||d[2])){
                    iteml->setDirType(d[0], 0, 0, d[3]);
                    itemr->setDirType(0, d[1], d[2], 0);
                }else return false;
            }

            QVector<Item *> optl({iteml}), optr({itemr});
            int rtx = x, rty = y;
            if(getDirect() & 1) ++rty;
            else ++rtx;
            if(lf->checkIpt(x, y, optl) && rt->checkIpt(rtx, rty, optr)){
                lf->input(optl, x, y); rt->input(optr, rtx, rty);
                item = nullptr;
                return true;
            }else{
                delete iteml;
                delete itemr;
                return false;
            }
        }
    }
}
