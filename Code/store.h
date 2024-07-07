#ifndef STORE_H
#define STORE_H

#include <QWidget>
#include <QLabel>
#include <QStyle>
#include <QPainter>
#include <QBoxLayout>
#include "global.h"
#include "toolbutton.h"

class Store : public QWidget
{
    Q_OBJECT
public:
    // 是否处于局部强化设置模式
    bool partial;

    QLabel title, money, grade[3], partialGrade[3];
    QHBoxLayout shelf;
    QVBoxLayout col[3];
    ToolButton btn[3], close;
    explicit Store(QWidget *parent = nullptr);

    void fresh();
    void setPartial(bool p);

private slots:
    void paintEvent(QPaintEvent *event) override;
signals:
    void resetStore();
    void upgradeMap();
    void upgradeCenter();
};

#endif // STORE_H
