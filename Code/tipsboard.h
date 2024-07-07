#ifndef TIPSBOARD_H
#define TIPSBOARD_H

#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include "toolbutton.h"

class TipsBoard : public QWidget
{
    Q_OBJECT
public:
    QLabel tips;
    QScrollArea board;
    ToolButton close;
    explicit TipsBoard(QWidget *parent = nullptr);

signals:
};

#endif // TIPSBOARD_H
