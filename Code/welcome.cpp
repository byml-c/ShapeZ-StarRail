#include "welcome.h"

Welcome::Welcome(QWidget *parent)
    : QWidget{parent}, begin(this), restart(this)
{
    setStyleSheet("QPushButton {border: none; background: transparent; }"
                  "QPushButton:hover {background-color: rgba(255, 255, 255, 20); border-radius: 45px 45px; }");
    begin.setFixedSize(300, 90);
    begin.move((width()-begin.width())/2, height()*2/3);
    begin.setIconSize(begin.size());
    begin.setIcon(QIcon(QPixmap(":/img/startButton.png")));

    restart.setFixedSize(300, 90);
    restart.move((width()-restart.width())/2, height()*2/3+100);
    restart.setIconSize(restart.size());
    restart.setIcon(QIcon(QPixmap(":/img/newButton.png")));

    connect(&begin, &QPushButton::clicked, this, [=](){
        // QFile loadFile("./data.txt");
        QString filePath = QFileDialog::getOpenFileName(
            this, "读取存档", QDir().absoluteFilePath("./"), "(*.txt)");
        QFile loadFile(filePath);
        if(loadFile.open(QIODevice::ReadOnly)){
            if(check(&loadFile)){
                emit beginGame(&loadFile);
            }else{
                if(QMessageBox::question(this, "错误", "存档校验未通过，是否启动新游戏？",
                                          QMessageBox::Yes|QMessageBox::No) \
                    == QMessageBox::Yes){
                    emit beginGame(nullptr);
                }
            }
        }else{
            if(QMessageBox::question(this, "错误", "存档无法打开，是否启动新游戏？",
                                      QMessageBox::Yes|QMessageBox::No) \
                == QMessageBox::Yes){
                emit beginGame(nullptr);
            }
        }
    });
    connect(&restart, &QPushButton::clicked, this, [=](){
        emit restartGame();
    });
}

void Welcome::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawPixmap(QRect(0, 0, width(), height()), QPixmap(":/img/Welcome.png"));
    painter.save();
}

bool Welcome::check(QFile *file){
    QTextStream in(file);
    try{
        int mapG, ign;
        in>>ign>>ign;
        in>>ign>>ign>>ign>>ign;
        in>>mapG>>ign>>ign;
        in>>ign>>ign>>ign;

        int mx = mapSize[mapG][0], my = mapSize[mapG][1];
        for(int y=0;y<my;++y){
            for(int x=0;x<mx;++x){
                in>>ign;
            }
        }

        int demandsLength = 0;
        in>>demandsLength;
        if(demandsLength == 0){
            throw false;
        }
        for(int i=0;i<demandsLength;++i){
            int dx, dy;
            in>>dx>>dy;
            if(dx == 0 || dy == 0){
                throw false;
            }
        }
        in>>ign;
        if(!in.atEnd()){
            throw false;
        }
        file->seek(0);
        return true;
    }catch(bool fail){
        return false;
    }
}

void Welcome::resizeEvent(QResizeEvent *){
    begin.move((width()-begin.width())/2, height()*2/3);
    restart.move((width()-restart.width())/2, height()*2/3+100);
}
