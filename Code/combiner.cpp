#include "combiner.h"

Combiner::Combiner(int x, int y, int type, QObject *parent, bool virt)
    : Device(x, y, 2, 1, type, COMBINER_IMG, parent, virt),
    item1(nullptr), item2(nullptr)
{

}

bool Combiner::checkIpt(int ix, int iy, QVector<Item *> &ipt){
    int fr = getDirect()^0b10;
    if(ipt.size() < 1) return false;
    if(fr==0 && ((ix!=x&&ix!=x+1)||iy!=y-1)) return false;
    if(fr==1 && (ix!=x+1||(iy!=y&&iy!=y+1))) return false;
    if(fr==2 && ((ix!=x&&ix!=x+1)||iy!=y+1)) return false;
    if(fr==3 && (ix!=x-1||(iy!=y&&iy!=y+1))) return false;
    if(ipt[0]->isMark()) return false;
    if(fr&1){
        if((iy==y)&&(item1!=nullptr)){
            if(item1->type == ipt[0]->type){
                return false;
            }else return true;
        }
        if((iy==y+1)&&(item2!=nullptr)){
            if(item2->type == ipt[0]->type){
                return false;
            }else return true;
        }
    }else{
        if((ix==x)&&(item1!=nullptr)){
            if(item1->type == ipt[0]->type){
                return false;
            }else return true;
        }
        if((ix==x+1)&&(item2!=nullptr)){
            if(item2->type == ipt[0]->type){
                return false;
            }else return true;
        }
    }
    return true;
}
void Combiner::input(QVector<Item *> &ipt, int ix, int iy){
    int fr = getDirect()^0b10;
    // qWarning()<<"combiner input "<<x<<y<<"inp"<<ix<<iy;
    if(((fr&1)&&(iy==y))||((!(fr&1))&&(ix==x))){
        if(item1 != nullptr){
            delete item1;
        }
        item1 = ipt[0];
        item1->setShow(false);
        qWarning()<<"input item 1";
    }else{
        if(item2 != nullptr){
            delete item2;
        }
        item2 = ipt[0];
        item2->setShow(false);
        qWarning()<<"input item 2";
    }
    if((item1 && !item1->isSplittable()) ||
        (item2 && !item2->isSplittable())){
        setSelectColor(1);
        setSelect(true, true);
    }else{
        setSelectColor(0);
        setSelect(false, true);
    }
}
void Combiner::update(int flip){
    updateMark(flip);
    if((item1!=nullptr) && (item2!=nullptr)){
        flipCount ++;
        if(flipCount > baseInterval-splitterGrade){
            flipCount = 0;
            output();
        }
    }else flipCount = 0;
}
bool Combiner::output(bool mark){
    int ox = x, oy = y;
    switch(getDirect()){
    case 2:
        ox++; break;
    case 3:
        oy++; break;
    default: break;
    }
    auto odev =devicesMap.find(getOutput(ox, oy));
    if(odev == devicesMap.end()){
        return false;
    }else{
        if(mark){
            auto opt = odev.value();
            Item *oitem = Item::markType(x*PX, y*PX);
            oitem->setShow(false);

            QVector<Item *> opti({oitem});
            if(opt->checkIpt(ox, oy, opti)){
                opt->input(opti);
                return true;
            }else{
                delete oitem;
                return false;
            }
        }else{
            if(!item1->isSplittable() || !item2->isSplittable()){
                return false;
            }
            int d[4] = {0};
            auto opt = odev.value();
            for(int i=0;i<4;++i){
                int d1 = item1->getDirType(i);
                int d2 = item2->getDirType(i);
                if(d1 && d2) return false;
                d[i] = d1|d2;
            }

            Item *oitem = new Item(
                x*PX, y*PX, item1->getType());
            oitem->setShow(false);
            oitem->setDirType(d[0], d[1], d[2], d[3]);

            QVector<Item *> opti({oitem});
            if(opt->checkIpt(ox, oy, opti)){
                // qWarning()<<ox<<oy<<"output!";
                opt->input(opti, ox, oy);
                item1 = nullptr;
                item2 = nullptr;
                return true;
            }else{
                delete oitem;
                return false;
            }
        }
    }
}
