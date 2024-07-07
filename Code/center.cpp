#include "center.h"

Center::Center(int x, int y, QObject *parent)
    : Device(x, y, (centerGrade+1)*2, (centerGrade+1)*2, 0b0110001, "", parent),
    reqItem(x*PX, y*PX, 0b0, parent)
{
    selectable = false;
    setRequire(process, false);
}

void Center::setRequire(int pro, bool reset){
    if(pro >= demands.size()){
        // 如果不存在关卡，就随机新关卡
        int integrate = rand()%10;
        if(integrate < 2){
            // 小概率要求交付不可分割
            demands.append(QPair<int, int>(
                0b000000000000000110, 10+(rand()%4)*5));
        }else{
            // 四个方向分别随机
            int d[4] = {0}, typ = 1;
            while(true){
                typ = 1;
                for(int i=0;i<4;++i){
                    d[i] = rand()%3;
                    typ = (typ<<4)|d[i];
                }
                // 不能出现四个全空的情况
                if(d[0]|d[1]|d[2]|d[3]){
                    break;
                }
            }
            demands.append(QPair<int, int>(
                typ<<1, 10+(rand()%4)*5));
        }
        pro = demands.size()-1;
    }

    process = pro;
    reqType = demands[pro].first;
    reqNum = demands[pro].second;
    if(reset) reqCount = 0;
    reqItem.setShow(true);
    reqItem.setType(reqType);
}

void Center::input(QVector<Item *> &ipt, int, int){
    for(auto &item:ipt){
        // qWarning()<<item->getType()<<reqType;
        if(item->getType() == reqType){
            reqCount ++;
            coin += 2+centerGrade*5;
            if(reqCount >= reqNum){
                if(proCount < 3){
                    if(process+1 >= proNum[proCount]){
                        proCount ++;
                        setRequire(process+1);
                        emit finishSection();
                    }else{
                        setRequire(process+1);
                        emit finishSubsection();
                    }
                }else{
                    setRequire(process+1);
                    emit finishSubsection();
                }
            }
        }
        delete item;
    }
}

void Center::draw(QPainter &painter){
    if(centerGrade){
        setImage(CENTER_4x4_IMG);
    }else setImage(CENTER_2x2_IMG);
    if(imgPath.size() == 0) return ;

    QPixmap pix = QPixmap(imgPath);
    painter.drawPixmap(
        QRect(BaseX+x*PX+MA, BaseY+y*PX+MA, w*PX-2*MA, h*PX-2*MA),
        pix
        );
    QString opt = QString::number(reqCount)+"/"+QString::number(reqNum);
    painter.setPen(QColor(37, 37, 37));
    if(centerGrade){
        reqItem.setPos(x*PX+56, y*PX+115);
        painter.setFont(QFont(FONTS[0], 24));
        painter.drawText(
            QRect(BaseX+x*PX+135, BaseY+y*PX+100, 120, 70),
            Qt::AlignCenter, opt
            );
    }else{
        reqItem.setPos(x*PX+40, y*PX+28);
        painter.setFont(QFont(FONTS[0], 15));
        painter.drawText(
            QRect(BaseX+x*PX+25, BaseY+y*PX+95, 120, 30),
            Qt::AlignCenter, opt
            );
    }
    painter.save();
    reqItem.draw(painter);
}
