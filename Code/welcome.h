#ifndef WELCOME_H
#define WELCOME_H

#include <QFile>
#include <QWidget>
#include <QPainter>
#include <QFileDialog>
#include <QPushButton>
#include <QTextStream>
#include <QMessageBox>
#include "global.h"

class Welcome : public QWidget
{
    Q_OBJECT
public:
    QPushButton begin, restart;
    explicit Welcome(QWidget *parent = nullptr);
    bool check(QFile *file); // 存档格式校验

private:
    void resizeEvent(QResizeEvent *event) override;

signals:
    void beginGame(QFile *file = nullptr);
    void restartGame();

private slots:
    void paintEvent(QPaintEvent *event) override;
};

#endif // WELCOME_H
