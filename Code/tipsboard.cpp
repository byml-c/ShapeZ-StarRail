#include "tipsboard.h"

TipsBoard::TipsBoard(QWidget *parent)
    : QWidget{parent}, tips(this), board(this)
{
    setFixedSize(1080, 600);

    board.setFixedSize(1080, 600);
    board.setStyleSheet("background-color: #ffffff; border-radius: 10px; padding-top: 20px; padding-bottom: 20px;");

    tips.setFixedSize(1008, 1862);
    tips.setPixmap(QPixmap(":/img/Tips.png"));
    tips.setParent(&board);
    board.setWidget(&tips);
    board.setAlignment(Qt::AlignCenter);
    board.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    close.set(width()-50, 20, 30, 30, ":/img/CloseButton.png", this);
    connect(&close, &QPushButton::clicked, this, [=](){
        setHidden(!isHidden());
    });

    hide();
}
