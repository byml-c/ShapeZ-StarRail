#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QVector>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QQueue>
#include <QScrollArea>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QTextStream>
#include <exception>

#include "global.h"
#include "device.h"
#include "item.h"
#include "trash.h"
#include "miner.h"
#include "center.h"
#include "splitter.h"
#include "conveyor.h"
#include "combiner.h"
#include "eraser.h"
#include "store.h"
#include "toolbar.h"
#include "tipsboard.h"
#include "rotateright.h"

class GameEngine : public QWidget
{
    Q_OBJECT
public:
    // 全局计时
    unsigned long long flip;
    // 游戏是否处于暂停状态、暂停按钮、提示按钮
    bool pause;
    ToolButton pauseBtn, tipsBtn;
    TipsBoard tips;

    // 开启无尽模式
    bool waitInfity;
    ToolButton infityBtn;

    // 背景音乐播放器、背景图片路径
    int mediaChoice;
    QMediaPlayer musicPlayer;
    QAudioOutput audio;
    QString background;

    // 工具栏
    ToolBar tools;
    // 商店
    Store store;

    // 存档路径
    QString savePath = "./data.txt";
    // 是否处于拖动地图状态
    bool onDrag;
    int startBaseX, startBaseY;
    double dragX, dragY;
    // 当前被选择的设备
    Device* select;
    // 处于放置状态的设备类型，为 0 则处于空
    int putType;
    bool conveyorDown, mouseDown; // 是否已经放下第一个 conveyor
    bool exist, reverse; // 是否是对存在的传送带进行操作，是否反向连接
    Device *putting;
    QList<Conveyor *> putConveyor;

    // 存储中心
    Center *center;

    void save(); // 保存进度
    void load(QFile *loadFile=nullptr); // 加载进度
    void start(); // 开始游戏
    bool checkPut(); // 验证是否能够放置
    // 地图渲染
    void drawMap(QPainter &painter);
    // 设置背景音乐和图片
    void setBack();

    // 事件暂存
    QQueue<QEvent *>events;

    // 更新计时
    qint64 lastUpdate, lastRender;

    inline int min(int x, int y){return x>y?y:x;}
    inline int max(int x, int y){return x>y?x:y;}

    explicit GameEngine(QWidget *parent = nullptr);
    ~GameEngine();
private slots:
    // 游戏主循环
    void play();
public slots:
    // 拖曳放置
    void put(int type, int sx, int sy);
    // 开关商店
    void toggleStore();
    // 根据等级扩展地图
    void expandMap(bool init = false, QTextStream *in = nullptr);
    // 完成部分
    void finishSubsection();
    void finishSection();
private:
    QTimer timer;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
signals:
    void resetToolButton(int id);
};

#endif // GAMEENGINE_H
