#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QPushButton>

class ToolButton : public QPushButton
{
public:
    ToolButton();
    void set(int rx, int ry, int w, int h, QString imgPath, QWidget *parent = nullptr);
    void setInLayor(int w, int h, QString imgPath, QWidget *parent = nullptr);
    void setPos(int rx, int ry);
    void setImg(QString imgPath);
};

#endif // TOOLBUTTON_H
