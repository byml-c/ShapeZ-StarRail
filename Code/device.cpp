#include "device.h"

// 全局设备列表
QList<Device *> devices;
QMap<QPair<int, int>, Device*> devicesMap;

Device::Device(int x, int y, int w, int h, int type, QString imgPath,
               QObject *parent, bool virt)
    : QObject{parent}, selected(false), selectable(true), virt(virt),
    imgPath(imgPath), x(x), y(y), w(w), h(h), type(type),
    flipCount(0)
{
    // 在加入时加入 devices 和 devicesMap
    devices.append(this);
    setDirect(getDirect(), false);
    setSelectColor();
}
Device::~Device(){
    // 在析构时从 devices 和 devicesMap 删除
    devices.removeOne(this);
    wipeMap();
}
void Device::setMap(){
    if(virt) return ;
    gridMap[y][x] = type;
    for(int i=0;i<h;++i){
        for(int j=0;j<w;++j){
            if(i!=0 || j!=0){
                gridMap[y+i][x+j] = 0b0001001; // 写入设备占位符
            }
            devicesMap[QPair<int, int>(x+j, y+i)] = this;
        }
    }
}
void Device::wipeMap(){
    if(virt) return ;
    for(int i=0;i<h;++i){
        for(int j=0;j<w;++j){
            if((gridMap[y+i][x+j]&1) && (gridMap[y+i][x+j] != 1)){
                // 清空设备标志符和占位符
                gridMap[y+i][x+j] = 0;
                // 从地图中删除设备
                auto it = devicesMap.find(QPair<int, int>(x+j, y+i));
                if(it != devicesMap.end()){
                    if(*it != this){
                        // 删除占用位置的其他设备
                        delete *it;
                    }
                    devicesMap.remove(QPair<int, int>(x+j, y+i));
                }
            }
        }
    }
}
void Device::setVirt(bool v){
    if(!virt && v){
        // 从非虚拟转化为虚拟，需要先删去其对应位置的值
        wipeMap();
    }
    virt = v;
}
void Device::setSize(int nx, int ny, int nw, int nh){
    x = nx; y = ny; w = nw; h = nh;
}
void Device::setPos(int nx, int ny, bool wipe){
    if(wipe) wipeMap();
    x = nx; y = ny;
    setMap();
}
int Device::getDirect(){
    return (type>>1) & 0b11;
}
void Device::setDirect(int direct, bool wipe){
    if(wipe) wipeMap();
    int dir = type & 0b110;
    type = (type^dir)|(direct<<1);
    if(((direct & 1) && w>h) || (!(direct & 1) && h>w)){
        // 当向右或向左时，出现宽度大于高度的情况，就交换
        // 当向上或向下时，出现高度大于宽度的情况，就交换
        int tmp = w; w = h; h = tmp;
    }
    setMap();
}
void Device::setImage(QString path){
    imgPath = path;
}
void Device::setSelectColor(int ctype){
    if(ctype == 0){
        selectR = 36; selectG = 173; selectB = 243;
    }else if(ctype == 1){
        selectR = 230; selectG = 126; selectB = 34;
    }
}
void Device::draw(QPainter &painter){
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
        pix
    );
    painter.save();
}

void Device::input(QVector<Item *> &items, int, int){
    for(auto &item:items){
        if(item != nullptr){
            delete item;
            item = nullptr;
        }
    }
}

QPair<int, int> Device::getOutput(int sx, int sy){
    if(sx == -1 || sy == -1){
        sx = x; sy = y;
    }

    int to = getDirect();
    int nx = -1, ny = -1;
    switch(to){
    case 0:
        if(y > 0){
            nx = sx; ny = sy-1;
        }
        break;
    case 1:
        if(x < WB-1){
            nx = sx+1; ny = sy;
        }
        break;
    case 2:
        if(y < HB-1){
            nx = sx; ny = sy+1;
        }
        break;
    case 3:
        if(x > 0){
            nx = sx-1; ny = sy;
        }
        break;
    default:
        break;
    }
    return QPair<int, int>(nx, ny);
}
void Device::update(int){

}
bool Device::checkIpt(int , int , QVector<Item *> &){
    return true;
}
bool Device::checkOpt(){
    return true;
}
bool Device::output(bool ){
    return true;
}
bool Device::isIn(int px, int py){
    if(px<x*PX || px>(x+w)*PX){
        return false;
    }else if(py<y*PX || py>(y+h)*PX){
        return false;
    }else return true;
}
void Device::setSelect(bool select, bool force){
    if(selectable || force){
        selected = select;
    }
}
int Device::getDeviceType(){
    return (type>>3)&0b1111;
}
void Device::updateMark(int flip){
    if(flip % markInterval  == 0){
        output(true);
    }
}
