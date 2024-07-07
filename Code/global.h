#ifndef GLOBAL_H
#define GLOBAL_H

#include <QVector>
#include <QDebug>
#include <QFontDatabase>

// 定义格子的大小和格子单位PX
#define WW 1600
#define WH 960
#define PX 80
#define MA 4
// 格子数
extern int WB;
extern int HB;
// 地图位移和缩放比例
extern int BaseX;
extern int BaseY;
// 地图中存在障碍物的地方为 0b1，空格子为 0b0，其他数值为对应的矿物/设备
extern QVector<QVector<int>> gridMap; // 地图大小：12(y, 行)*20(x, 列)
extern int baseGridMap[20][32];
extern int mapSize[3][2];
/* 矿物最低位为 0，设备最低位为 1，编号均从 1 开始
 * 矿物 0b(是否可以切分)(4方格块的编号)0
    * 圆形编号：1 0001 0001 0001 0001 0
    * 方形编号：1 0010 0010 0010 0010 0
    * 圆片编号：0 0000 0000 0000 0011 0
    * 三角指向：0 0000 0000 0000 0100 0
 * 设备 0b(编号)(2位方向)1
    * 占位编号                           0001 00 1
    * 传送带编号                      00 0010 00 1(2位开始方向，自身编号，2位结束方向)
    * 采矿机编号 1 0000 0000 0000 0000 0 0011 00 1(矿物编号，自身编号)
    * 分割器编号                         0100 00 1(自身编号)
    * 垃圾桶编号                         0101 00 1(自身编号，方向固定)
    * 中心编号                           0110 00 1(自身编号，方向固定)
    * 合成器编号                         0111 00 1(自身编号)
    * 橡皮擦编号                         1000 00 1(自身编号)
    * 右旋器编号                         1001 00 1(自身编号)
*/

// 定义贴图宏
#define MINER_IMG ":/img/Miner.png"
#define GRID_IMG ":/img/Grid.png"
#define BLOCK_IMG ":/img/Block.png"
#define SPLITTER_IMG ":/img/Splitter.png"
#define TRASH_IMG ":/img/Trash.png"
#define COMBINER_IMG ":/img/Combiner.png"
#define CONVEYOR_S_IMG ":/img/Conveyor.png"
#define CONVEYOR_T_IMG ":/img/ConveyorTurn.png"
#define CONVEYOR_PS_IMG ":/img/ConveyorPut.png"
#define CENTER_2x2_IMG ":/img/Center2x2.png"
#define CENTER_4x4_IMG ":/img/Center4x4.png"

#define BASE_IMG ":/img/Base.png"
#define ROUND_IMG ":/img/Round.png"
#define ROUND_MINE_IMG ":/img/RoundMine.png"
#define RECT_IMG ":/img/Rect.png"
#define RECT_MINE_IMG ":/img/RectMine.png"
#define BUBBLE_IMG ":/img/Bubble.png"
#define BUBBLE_MINE_IMG ":/img/BubbleMine.png"
#define MARK_IMG ":/img/ConveyorMark.png"

#define MINER_BTN ":/img/MinerButton.png"
#define SPLITTER_BTN ":/img/SplitterButton.png"
#define CONVEYOR_BTN ":/img/ConveyorButton.png"
#define COMBINER_BTN ":/img/CombinerButton.png"
#define RIGHT_ROTATE_BTN ":/img/RoaterRightButton.png"
#define TRASH_BTN ":/img/TrashButton.png"
#define ERASER_BTN ":/img/Eraser.png"
#define STORE_BTN ":/img/StoreButton.png"
#define PLAY_BTN ":/img/PlayButton.png"
#define PAUSE_BTN ":/img/PauseButton.png"
#define TIPS_BTN ":/img/TipsButton.png"
#define TOOLBAR ":/img/ToolBar.png"

#define MUSIC_HEART "qrc:/music/Heart.mp3"
#define MUSIC_FEATHER "qrc:/music/Feather.mp3"
#define MUSIC_GALAXY "qrc:/music/Galaxy.mp3"
#define MUSIC_JOURNEY "qrc:/music/Journey.mp3"
#define MUSIC_SUN "qrc:/music/Sun.mp3"

#define FONT_PATH ":/font/HanYiWenHei_65W.ttf"
// 可全局访问的字体列表
extern QStringList FONTS;

// 定义当前进度
extern int reqCount, proCount;
// 定义每一关的分割点
extern QVector<int> proNum;
// 定义每一关需求
extern QVector<QPair<int, int>> demands;
// 定义地图等级、交付中心等级、矿产等级
extern int mapGrade, centerGrade, itemGrade;
extern int mapCost[3], centerCost[2], itemCost[3];
// 基础生成速度：帧/个，每帧为 10ms
extern int baseInterval;
// 传送带等级、采矿机等级、分割器和堆叠器等级
extern int conveyorGrade, minerGrade, splitterGrade;
// 定义游戏关卡进度、玩家拥有的金钱
extern int process, coin;
// 移动标记速度、生成速度
extern int markSpeed, markInterval;

#endif // GLOBAL_H
