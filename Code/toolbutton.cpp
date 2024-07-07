#include "toolbutton.h"

ToolButton::ToolButton() {}

void ToolButton::set(int rx, int ry, int w, int h, QString imgPath, QWidget *parent){
    if(parent != nullptr){
        setParent(parent);
    }
    move(rx, ry);
    setFixedSize(w, h);
    setIconSize(size());
    setIcon(QIcon(QPixmap(imgPath)));
}

void ToolButton::setInLayor(int w, int h, QString imgPath, QWidget *parent){
    if(parent != nullptr){
        setParent(parent);
    }
    setFixedSize(w, h);
    setIconSize(size());
    setIcon(QIcon(QPixmap(imgPath)));
}

void ToolButton::setImg(QString imgPath){
    setIcon(QIcon(QPixmap(imgPath)));
}

void ToolButton::setPos(int rx, int ry){
    move(rx, ry);
}
