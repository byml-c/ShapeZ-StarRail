#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>
#include <QLayout>
#include <QPainter>
#include "global.h"
#include "toolbutton.h"

class ToolBar : public QWidget
{
    Q_OBJECT
public:
    // miner, conveyor, splitter, combiner, rightrotate, trash, eraser, store;
    QStringList img;
    QHBoxLayout layout;
    ToolButton tool[8];
    QVector<int> types;
    QString pressStyle, normalStyle;
    explicit ToolBar(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;
public slots:
    void resetBtn(int id);
signals:
    void put(int, int, int);
    void toggleStore();
};

#endif // TOOLBAR_H
