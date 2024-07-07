#include "store.h"

Store::Store(QWidget *parent)
    : QWidget{parent}, partial(false), title(this), money(this)
{
    setFixedSize(1080, 600);

    title.setFixedSize(600, 40);
    title.move(240, 20);
    title.setAlignment(Qt::AlignCenter);
    title.setStyleSheet("font-size: 30px; font-weight: bold; color: #3c3c3c;");

    money.move(width()-250, 20);
    money.setFixedSize(150, 40);
    money.setStyleSheet("background: #3c3c3c; border-radius: 8px; font-size: 25px; color: #ffffff;");
    money.setAlignment(Qt::AlignCenter);

    close.set(width()-50, 25, 30, 30, ":/img/CloseButton.png", this);

    for(int i=0;i<3;++i){
        grade[i].setAlignment(Qt::AlignCenter);
        grade[i].setStyleSheet("background: #ffffff; color: #3c3c3c; border-radius: 8px; font-size: 20px;");
        btn[i].setFixedSize(300, 80);
        btn[i].setStyleSheet("QPushButton{background: #3c3c3c; color: #fff; border-radius: 8px; text-align: center; font-size: 20px; margin-bottom: 20px;}"
                             "QPushButton:hover{background: #5c5c5c;}");
        partialGrade[i].setAlignment(Qt::AlignCenter);
        partialGrade[i].setStyleSheet("margin-top: 80px; margin-bottom: 10px; background: #ffffff; color: #3c3c3c; border-radius: 8px; font-size: 20px; min-height: 80px; max-height: 80px;");
        col[i].addWidget(&partialGrade[i]);
        col[i].addWidget(&grade[i]);
        col[i].addWidget(&btn[i]);
        shelf.addLayout(&col[i]);
    }
    setLayout(&shelf);

    connect(&close, &QPushButton::clicked, this, [=](){
        setHidden(true);
        partial = false;
        emit resetStore();
    });
    connect(&btn[0], &QPushButton::clicked, this, [=](){
        // 局部强化：传送带等级；全局强化：地图
        if(partial){
            if(conveyorGrade <= 3)
                conveyorGrade ++;
            partial = false;
            setHidden(true);
        }else{
            if(mapGrade < 2 && coin >= mapCost[mapGrade+1]){
                mapGrade ++;
                coin -= mapCost[mapGrade];
                fresh();
                emit upgradeMap();
            }
        }
    });
    connect(&btn[1], &QPushButton::clicked, this, [=](){
        // 局部强化：采矿机等级；全局强化：交付中心
        if(partial){
            if(baseInterval - (minerGrade+1)*5 > 0)
                minerGrade ++;
            partial = false;
            setHidden(true);
        }else{
            if(centerGrade < 1 && coin >= centerCost[centerGrade+1]){
                centerGrade ++;
                coin -= centerCost[centerGrade];
                fresh();
                emit upgradeCenter();
            }
        }
    });
    connect(&btn[2], &QPushButton::clicked, this, [=](){
        // 局部强化：堆叠器&切割机等级；全局强化：矿物等级
        if(partial){
            if(baseInterval - (splitterGrade+1)*5 > 0)
                splitterGrade ++;
            partial = false;
            setHidden(true);
        }else{
            if(itemGrade < 2 && coin >= itemCost[itemGrade+1]){
                itemGrade ++;
                coin -= itemCost[itemGrade];
                fresh();
            }
        }
    });
}

void Store::fresh(){
    if(partial){
        title.setText("商店");
        money.setText("金钱："+QString::number(coin));

        partialGrade[0].setText("永久·地图 "+ QString::number(mapGrade+1) + " 级");
        grade[0].setText("局部·传送速度 "+ QString::number(conveyorGrade+1) + " 级");
        if(conveyorGrade >= 4){
            btn[0].setText("MAX");
        }else btn[0].setText("升级（传送速度，本关生效）");

        partialGrade[1].setText("永久·采矿机 "+ QString::number(minerGrade+1) + " 级");
        grade[1].setText("局部·交付中心 "+ QString::number(centerGrade+1) + " 级");
        grade[1].setText("采矿机 "+ QString::number(minerGrade+1) + " 级");
        if(baseInterval - (minerGrade+1)*5 <= 0){
            btn[1].setText("MAX");
        }else btn[1].setText("升级（采矿机，本关生效）");

        partialGrade[2].setText("永久·矿产 "+ QString::number(itemGrade+1) + " 级");
        grade[2].setText("局部·加工速度 "+ QString::number(splitterGrade+1) + " 级");
        if(baseInterval - (splitterGrade+1)*5 <= 0){
            btn[2].setText("MAX");
        }else btn[2].setText("升级（加工速度，本关生效）");
    }else{
        title.setText("商店");
        money.setText("金钱："+QString::number(coin));

        partialGrade[0].setText("局部·传送速度 "+ QString::number(conveyorGrade+1) + " 级");
        grade[0].setText("永久·地图 "+ QString::number(mapGrade+1) + " 级");
        if(mapGrade >= 2) btn[0].setText("MAX");
        else btn[0].setText("升级（地图） "+QString::number(mapCost[mapGrade]));

        partialGrade[1].setText("局部·采矿机 "+ QString::number(minerGrade+1) + " 级");
        grade[1].setText("永久·交付中心 "+ QString::number(centerGrade+1) + " 级");
        if(centerGrade >= 1) btn[1].setText("MAX");
        else btn[1].setText("升级（交付中心） "+QString::number(centerCost[centerGrade]));

        partialGrade[2].setText("局部·加工速度 "+ QString::number(splitterGrade+1) + " 级");
        grade[2].setText("永久·矿产 "+ QString::number(itemGrade+1) + " 级");
        if(itemGrade >= 2) btn[2].setText("MAX");
        else btn[2].setText("升级（矿产） "+QString::number(itemCost[itemGrade]));
    }
}
void Store::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.setPen(QColor(37, 37, 37, 100));
    painter.setBrush(QColor(142, 145, 155));
    painter.drawRoundedRect(rect(), 16, 16);
    painter.setPen(QColor(0, 0, 0, 0));
    painter.setBrush(QColor(37, 37, 37, 100));
    painter.drawRect(10, 175, width()-20, 4);
    painter.save();
    fresh();
}
void Store::setPartial(bool p){
    partial = p;
    fresh();
}
