#include "conveyor.h"

Conveyor::Conveyor(int x, int y, int type, QObject *parent, bool virt)
    : Device(x, y, 1, 1, type, CONVEYOR_S_IMG, parent, virt),
    onOpt(nullptr)
{
    setType(type);
}
Conveyor::~Conveyor(){
    for(auto &item:itemList){
        delete item;
    }
    itemList.clear();
    wipeMap();
}
bool Conveyor::valid(){
    if(getFrom() == getTo()){
        return false;
    }else return true;
}
void Conveyor::setDirect(int fr, int to, bool wipe){
    if(wipe) wipeMap();
    setType((fr<<7)|(0b0010<<3)|(to<<1)|1);
}
int Conveyor::getFrom(){
    return (type>>7)&0b11;
}
void Conveyor::setFrom(int fr){
    setType((fr<<7)|(type&0b1111111));
}
int Conveyor::getTo(){
    return getDirect();
}
void Conveyor::setTo(int to){
    setType(((type>>3)<<3)|((to<<1)|1));
}
void Conveyor::swapDirect(){
    int fr = getFrom(), to = getTo();
    setFrom(to); setTo(fr);
}
void Conveyor::setType(int typ){
    type = typ;
    int fr = getFrom(), to = getTo();
    if((fr==0&&to==3)||(fr==3&&to==0)){
        center = QPoint(x*PX-PX/2, y*PX-PX/2);
    }else if((fr==0&&to==1)||(fr==1&&to==0)){
        center = QPoint(x*PX+PX/2, y*PX-PX/2);
    }else if((fr==2&&to==1)||(fr==1&&to==2)){
        center = QPoint(x*PX+PX/2, y*PX+PX/2);
    }else if((fr==2&&to==3)||(fr==3&&to==2)){
        center = QPoint(x*PX-PX/2, y*PX+PX/2);
    }else center = QPoint(0, 0);
}
/* 贴图关系对照表
 * Normal
    2-0 0
    3-1 90
    0-2 180
    1-3 270
    (fr^to) == 0b10     90*to
 * D
    1-2 0
    2-3 90
    3-0 180
    0-1 270
    to == (fr+1)%4      90*((fr+3)%4)
 * T
    1-0 0
    2-1 90
    3-2 180
    0-3 270
    fr == (to+1)%4      90*to
*/
void Conveyor::draw(QPainter &painter){
    if(selected){
        painter.setPen(QPen(QColor(selectR, selectG, selectB, 127), 4, Qt::SolidLine));
        painter.setBrush(QBrush(QColor(selectR, selectG, selectB, 63), Qt::SolidPattern));
        painter.drawRect(BaseX+x*PX, BaseY+y*PX, w*PX, h*PX);
    }
    int fr = getFrom(), to = getTo();
    if((fr^to) == 0b10){
        painter.drawPixmap(
            QRect(BaseX+x*PX, BaseY+y*PX, w*PX, h*PX),
            QPixmap(virt?CONVEYOR_PS_IMG:CONVEYOR_S_IMG). \
                transformed(QTransform().rotate(90*to))
            );
    }else if(to == (fr+1)%4){
        painter.drawPixmap(
            QRect(BaseX+x*PX, BaseY+y*PX, w*PX, h*PX),
            QPixmap(CONVEYOR_T_IMG). \
                transformed(QTransform().rotate(90*fr))
            );
    }else if(fr == (to+1)%4){
        painter.drawPixmap(
            QRect(BaseX+x*PX, BaseY+y*PX, w*PX, h*PX),
            QPixmap(CONVEYOR_T_IMG). \
            transformed(QTransform().rotate(90*to))
            );
    }
    painter.save();
}
QPoint Conveyor::getBase(int nx, int ny, int direct){
    int x0, y0;
    switch(direct){
    case 0:
        x0 = nx*PX, y0 = ny*PX-PX/2; break;
    case 1:
        x0 = nx*PX+PX/2, y0 = ny*PX; break;
    case 2:
        x0 = nx*PX, y0 = ny*PX+PX/2; break;
    case 3:
        x0 = nx*PX-PX/2, y0 = ny*PX; break;
    default:
        x0 = y0 = 0; break;
    }
    return QPoint(x0, y0);
}
void Conveyor::movePace(Item *item){
    int fr = getFrom(), to = getTo();
    if(item->getProcess() >= 80){
        onOpt = item;
        if(!output() && !item->isMark())
            optFail = true;
        onOpt = nullptr;
    }else{
        int factor[] = {4, 8, 16, 20, 40};
        int speed = factor[conveyorGrade]; // 像素/帧
        if(item->isMark()) speed = markSpeed;

        item->addProcess(speed);// 进度(上限为80)/帧
        int pro = item->getProcess();
        if((fr^to) == 0b10){
            if(to == 0){
                item->setPos(x*PX, y*PX+PX/2-pro);
            }else if(to == 1){
                item->setPos(x*PX-PX/2+pro, y*PX);
            }else if(to == 2){
                item->setPos(x*PX, y*PX-PX/2+pro);
            }else{
                item->setPos(x*PX+PX/2-pro, y*PX);
            }
        }else{
            double proc = ((double)pro)/80.0;
            double theta = proc*asin(1.0);
            if(fr==2&&to==1){
                theta = acos(-1.0)-theta;
            }else if(fr==1&&to==2){
                theta = asin(1.0)+theta;
            }else if(fr==2&&to==3){
                // theta = theta;
            }else if(fr==3&&to==2){
                theta = asin(1.0)-theta;
            }else if(fr==0&&to==3){
                theta = -theta;
            }else if(fr==3&&to==0){
                theta = theta-asin(1.0);
            }else if(fr==0&&to==1){
                theta = acos(-1.0)+theta;
            }else if(fr==1&&to==0){
                theta = -asin(1.0)-theta;
            }
            item->setPos(
                center.x()+PX/2.0*cos(theta),
                center.y()-PX/2.0*sin(theta)
                );
            if(item->isMark()){
                // 旋转
                if(fr==(to+1)%4){ // 顺时针
                    item->setRotation(90*(fr^0b10)+(int)(proc*90));
                }else{ // 逆时针
                    item->setRotation(90*(fr^0b10)-(int)(proc*90));
                }
            }
        }
    }
}
void Conveyor::update(int flip){
    updateMark(flip);
    optFail = false;
    if(itemList.empty()) return ;

    removeList.clear();
    for(auto &item:itemList){
        movePace(item);
    }
    for(auto &item:removeList){
        itemList.removeOne(item);
    }
}
bool Conveyor::checkIpt(int ix, int iy, QVector<Item *> &ipt){
    int fr = getFrom();
    if(ipt.size() < 1) return false;
    if(fr==0 && (ix!=x || iy!=y-1)) return false;
    if(fr==1 && (ix!=x+1 || iy!=y)) return false;
    if(fr==2 && (ix!=x || iy!=y+1)) return false;
    if(fr==3 && (ix!=x-1 || iy!=y)) return false;
    if(ipt[0]->isMark()) return true;
    if(optFail) return false;
    return true;
}
void Conveyor::input(QVector<Item *> &ipt, int, int){
    itemList.push_back(ipt[0]);
    ipt[0]->setShow(true);
    ipt[0]->resetProcess();
    movePace(ipt[0]);
    if(ipt[0]->isMark()){ // 调整方向
        ipt[0]->setRotation(90*(getFrom()^0b10));
    }
}
bool Conveyor::output(bool){
    auto dev = devicesMap.find(getOutput());
    if(dev == devicesMap.end()){
        if(onOpt->isMark()){
            delete onOpt;
            removeList.append(onOpt);
        }
        return false;
    }else{
        auto device = dev.value();
        QVector<Item *> opt({onOpt});
        if(device->checkIpt(x, y, opt)){
            device->input(opt, x, y);
            removeList.append(onOpt);
            return true;
        }else{
            if(onOpt->isMark()){
                delete onOpt;
                removeList.append(onOpt);
            }
            return false;
        }
    }
}
void Conveyor::setVirt(bool v){
    if(!virt && v){
        // 从非虚拟转化为虚拟
        wipeMap();
        // 将传送带上面的元素全部删除
        for(auto &item:itemList){
            delete item;
        }
        itemList.clear();
    }
    virt = v;
}
void Conveyor::updateMark(int flip){
    if(flip%markInterval == 0){
        if(devicesMap.find(getInput()) == devicesMap.end()){
            QVector<Item *> ipt({Item::markType(x*PX, y*PX)});
            input(ipt);
        }
    }
}
QPair<int, int> Conveyor::getInput(){
    int fr = getFrom(), ix = x, iy = y;
    if(fr==0) --iy;
    if(fr==1) ++ix;
    if(fr==2) ++iy;
    if(fr==3) --ix;
    return QPair<int, int>(ix, iy);
}
