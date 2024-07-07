#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    welcome = new Welcome(this);
    engine = new GameEngine(this);
    engine->hide(); welcome->show();
    connect(welcome, &Welcome::beginGame, this, [=](QFile *file){
        // 读取存档，开始游戏
        engine->show(); welcome->hide();
        engine->load(file);
        engine->start();
    });
    connect(welcome, &Welcome::restartGame, this, [=](){
        // 开始新游戏
        engine->show(); welcome->hide();
        engine->load(nullptr);
        engine->start();
    });
}
void MainWindow::resizeEvent(QResizeEvent *){
    engine->resize(size());
    welcome->resize(size());
}
MainWindow::~MainWindow()
{

}
