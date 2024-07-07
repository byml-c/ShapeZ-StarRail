#include "gameEngine.h"

GameEngine::GameEngine(QWidget *parent)
    : QWidget{parent}, tips(this), waitInfity(false),
    musicPlayer(this), audio(this),
    background(":/img/IX.png"),
    tools(this), store(this),
    select(nullptr), putType(0), putting(nullptr), center(nullptr),
    lastUpdate(0), lastRender(0)
{
    setStyleSheet("QPushButton{ border: none; backgroundColor: transparent; }"
                  "QPushButton:hover { background-color: rgba(36, 173, 243, 50); border-radius: 10px 10px; }");

    // 设置右上方的暂停和重置
    pause = false;
    pauseBtn.set(width()-PX*2, 0, PX, PX, PAUSE_BTN, this);
    connect(&pauseBtn, &QPushButton::clicked, this, [=](){
        this->pause = !this->pause;
        if(this->pause){
            pauseBtn.setImg(PLAY_BTN);
        }else pauseBtn.setImg(PAUSE_BTN);
    });

    onDrag = false;

    // 设置提示
    // tips.move((width()-tips.width())/2, (height()-tips.height())/2);
    tipsBtn.set(width()-PX, 0, PX, PX, TIPS_BTN, this);
    connect(&tipsBtn, &QPushButton::clicked, this, [=](){
        tips.setHidden(!tips.isHidden());
    });

    // 设置工具栏
    // tools.move((width()-tools.width())/2, height()-PX-40);
    tools.show();
    connect(&tools, &ToolBar::put, this, &GameEngine::put);
    connect(&tools, &ToolBar::toggleStore, this, &GameEngine::toggleStore);
    connect(this, &GameEngine::resetToolButton, &tools, &ToolBar::resetBtn);

    // 设置商店
    // store.move((width()-store.width())/2, (height()-store.height())/2);
    store.hide();
    connect(&store, &Store::upgradeMap, this, [=](){
        this->expandMap();
    });
    connect(&store, &Store::upgradeCenter, this, [=](){
        center->setSize(center->x, center->y, 4, 4);
        center->wipeMap();
        center->setMap();
    });
    connect(&store, &Store::resetStore, this, [=](){
        emit resetToolButton(0);
    });

    // 启动监听
    setMouseTracking(true);
    grabKeyboard();

    // 绑定计时器槽
    connect(&timer, &QTimer::timeout, this, &GameEngine::play);

    // 初始化时载入字体
    int fontId = QFontDatabase::addApplicationFont(FONT_PATH);
    FONTS = QFontDatabase::applicationFontFamilies(fontId);
}
GameEngine::~GameEngine(){
    // 析构时进行保存
    save();
    // 保存后析构所有的设备
    for(auto &dev:devices){
        delete dev; dev = nullptr;
    }
}
void GameEngine::toggleStore(){
    if(!store.partial || !store.isVisible()){
        store.setVisible(!store.isVisible());
    }
    if(!store.isVisible()){
        emit resetToolButton(0);
    }
}
void GameEngine::start(){
    timer.setInterval(0);
    timer.start();
}
void GameEngine::put(int type, int sx, int sy){
    if(putType != 0){
        // 如果点击某一个按钮时，已经处于 putType 状态
        if(putType == 2){
            if(putConveyor.size()>0){
                for(auto &con:putConveyor){
                    delete con;
                }
                putConveyor.clear();
            }
        }else{
            if(putting != nullptr){
                delete putting;
                putting = nullptr;
            }
        }
        // 如果重复点击的同一个按钮，则代表取消放置状态
        if(putType == type){
            emit resetToolButton(putType);
            putType = 0;
            return ;
        }else{
            // 否则正常进行（切换按钮）
            emit resetToolButton(putType);
        }
    }

    putType = type;
    sx = (sx-BaseX)/PX; sy = (sy-BaseY)/PX;
    if(sx<0||sx>=WB||sy<0||sy>HB) return ;
    if(putType == 2){
        conveyorDown = false;
        putConveyor.append(
            new Conveyor(sx, sy, 0b100010001, nullptr, true));
    }else{
        switch(putType){
        case 3:
            // Miner
            putting = new Miner(sx, sy, 0b0011001, nullptr, true);
            break;
        case 4:
            // Splitter
            putting = new Splitter(sx, sy, 0b0100001, nullptr, true);
            break;
        case 5:
            // Trash
            putting = new Trash(sx, sy, 0b0101001, nullptr, true);
            break;
        case 7:
            // Combiner
            putting = new Combiner(sx, sy, 0b0111001, nullptr, true);
            break;
        case 8:
            // Eraser
            putting = new Eraser(sx, sy, 0b1000001, nullptr, true);
            break;
        case 9:
            // RightRotater
            putting = new RotateRight(sx, sy, 0b1001001, nullptr, true);
            break;
        default:
            putType = 0;
            putting = nullptr;
            break;
        }
        if(putType == 8){
            mouseDown = false;
        }
    }
}
void GameEngine::mouseMoveEvent(QMouseEvent *event){
    QPointF pos = event->position();
    int nx = ((int)(pos.x()-BaseX)/PX), ny = ((int)(pos.y()-BaseY)/PX);
    nx = qMax(0, qMin(nx, WB-1)); ny = qMax(0, qMin(ny, HB-1));

    if(onDrag){
        BaseX = min(max(width()-WB*PX, startBaseX+pos.x()-dragX), 0);
        BaseY = min(max(height()-HB*PX, startBaseY+pos.y()-dragY), 0);
        // BaseX = BaseX/PX*PX; BaseY = BaseY/PX*PX;
        // qWarning()<<"set base "<<BaseX<<BaseY;
        return ;
    }
    if(putType == 0) return ;
    if(putType == 2){
        if(!conveyorDown){
            auto con = putConveyor.front();
            con->x = nx;
            con->y = ny;
            con->setSelect(true, true);
            if(checkPut()){
                con->setSelectColor(0);
            }else con->setSelectColor(1);
        }else{
            Conveyor *last = putConveyor.last();
            int lx = last->x, ly = last->y;
            if(lx == nx && ly == ny){
                // 原位则不作任何操作
            }else{
                // 规划路径
                QList<QPair<int, int>> points;
                if(nx >= lx){
                    for(int i=lx+1;i<=nx;++i){
                        points.push_back(QPair<int, int>(i, ly));
                    }
                }else{
                    for(int i=lx-1;i>=nx;--i){
                        points.push_back(QPair<int, int>(i, ly));
                    }
                }
                if(ny >= ly){
                    for(int i=ly+1;i<=ny;++i){
                        points.push_back(QPair<int, int>(nx, i));
                    }
                }else{
                    for(int i=ly-1;i>=ny;--i){
                        points.push_back(QPair<int, int>(nx, i));
                    }
                }

                // 进行路径循环
                for(auto &point:points){
                    int px = point.first, py = point.second;

                    // 是否存在重复的情况
                    Conveyor *cyc = nullptr;
                    for(auto &con:putConveyor){
                        if(con->x == px && con->y == py){
                            cyc = con; break;
                        }
                    }
                    if(gridMap[py][px] != 0 || cyc != nullptr){
                        if(putConveyor.size()>=2 &&
                            putConveyor.lastIndexOf(cyc) == putConveyor.size()-2){
                            delete last;
                            putConveyor.removeLast();
                        }else{
                            // 根据鼠标移动方向设置最后出去的方向
                            if(px == lx+1 && py == ly){
                                if(reverse){
                                    last->setFrom(1);
                                }else last->setTo(1);
                            }else if(px == lx && py == ly+1){
                                if(reverse){
                                    last->setFrom(2);
                                }else last->setTo(2);
                            }else if(px == lx-1 && py == ly){
                                if(reverse){
                                    last->setFrom(3);
                                }else last->setTo(3);
                            }else if(px == lx && py == ly-1){
                                if(reverse){
                                    last->setFrom(0);
                                }else last->setTo(0);
                            }
                        }
                        break;
                    }else{
                        // 正常连线
                        if(exist && putConveyor.size() == 1){
                            reverse = false;
                        }

                        bool flag = false;
                        Conveyor *next = new Conveyor(
                            px, py, last->type, nullptr, true);
                        next->setSelect(true, true);
                        if(px == lx+1 && py == ly){
                            if(last->getFrom() == 1){
                                if(exist){
                                    reverse = true;
                                }else last->swapDirect();
                            }else{
                                if(reverse){
                                    last->setFrom(1);
                                }else last->setTo(1);
                            }

                            if(reverse){
                                next->setFrom(1);
                                next->setTo(3);
                            }else{
                                next->setFrom(3);
                                next->setTo(1);
                            }
                            flag = true;
                        }else if(px == lx && py == ly+1){
                            if(last->getFrom() == 2){
                                if(exist){
                                    reverse = true;
                                }else last->swapDirect();
                            }else{
                                if(reverse){
                                    last->setFrom(2);
                                }else last->setTo(2);
                            }

                            if(reverse){
                                next->setFrom(2);
                                next->setTo(0);
                            }else{
                                next->setFrom(0);
                                next->setTo(2);
                            }
                            flag = true;
                        }else if(px == lx-1 && py == ly){
                            if(last->getFrom() == 3){
                                if(exist){
                                    reverse = true;
                                }else last->swapDirect();
                            }else{
                                if(reverse){
                                    last->setFrom(3);
                                }else last->setTo(3);
                            }

                            if(reverse){
                                next->setFrom(3);
                                next->setTo(1);
                            }else{
                                next->setFrom(1);
                                next->setTo(3);
                            }
                            flag = true;
                        }else if(px == lx && py == ly-1){
                            if(last->getFrom() == 0){
                                if(exist){
                                    reverse = true;
                                }else last->swapDirect();
                            }else{
                                if(reverse){
                                    last->setFrom(0);
                                }else last->setTo(0);
                            }

                            if(reverse){
                                next->setFrom(0);
                                next->setTo(2);
                            }else{
                                next->setFrom(2);
                                next->setTo(0);
                            }
                            flag = true;
                        }

                        if(flag){
                            putConveyor.append(next);
                            last = next;
                            lx = px; ly = py;
                        }else{
                            delete next;
                            break;
                        }
                    }
                }
            }
        }
    }else if(putType == 8){
        putting->setPos(nx, ny, false);
        putting->setSelect(true, true);
        if(mouseDown){
            if(nx>=0&&nx<WB&&ny>=0&&HB){
                auto it = devicesMap.find(QPair<int, int>(nx, ny));
                if(it != devicesMap.end()){
                    if(it.value()->getDeviceType()==2){
                        delete it.value();
                        putting->setSelectColor(0);
                    }else putting->setSelectColor(1);
                }else{
                    if(gridMap[ny][nx] != 0){
                        putting->setSelectColor(1);
                    }else putting->setSelectColor(0);
                }
            }
        }else putting->setSelectColor(1);
    }else{
        putting->setPos(nx, ny, false);
        putting->setSelect(true, true);
        if(checkPut()){
            putting->setSelectColor(0);
        }else putting->setSelectColor(1);
    }
}
bool GameEngine::checkPut(){
    if(putType == 0) return false;

    if(putType == 2){
        for(auto &dev:putConveyor){
            int putX = dev->x, putY = dev->y;
            if(gridMap[putY][putX] != 0){
                return false;
            }else if(!dev->valid()){
                return false;
            }
        }
        return true;
    }else{
        int putX = putting->x, putY = putting->y;
        if(putType == 3){
            // 采矿机需要放在有矿的地方
            if(gridMap[putY][putX] != 0 && !(gridMap[putY][putX]&1)){
                return true;
            }else return false;
        }else{
            bool puttable = true;
            for(int dy=0;dy<putting->h;++dy){
                for(int dx=0;dx<putting->w;++dx){
                    if(gridMap[putY+dy][putX+dx] != 0){
                        puttable = false; break;
                    }
                }
            }
            if(!puttable){
                return false;
            }else return true;
        }
    }
}
void GameEngine::keyPressEvent(QKeyEvent *event){
    if(putType == 0) return ;
    if(putType != 2){
        switch(event->key()){
        case Qt::Key_W:
            // 向上
            putting->setDirect(0, false);
            break;
        case Qt::Key_D:
            // 向右
            putting->setDirect(1, false);
            break;
        case Qt::Key_S:
            // 向下
            putting->setDirect(2, false);
            break;
        case Qt::Key_A:
            // 向左
            putting->setDirect(3, false);
            break;
        default:
            break;
        }
        if(checkPut()){
            putting->setSelectColor(0);
        }else putting->setSelectColor(1);
    }else if(putConveyor.size() == 1){
        auto pt = putConveyor.front();
        switch(event->key()){
        case Qt::Key_W:
            // 向上
            pt->setDirect(2, 0, false);
            break;
        case Qt::Key_D:
            // 向右
            pt->setDirect(3, 1, false);
            break;
        case Qt::Key_S:
            // 向下
            pt->setDirect(0, 2, false);
            break;
        case Qt::Key_A:
            // 向左
            pt->setDirect(1, 3, false);
            break;
        default:
            break;
        }
        if(checkPut()){
            pt->setSelectColor(0);
        }else pt->setSelectColor(1);
    }
}
void GameEngine::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){ // 左键选中或确认放置
        if(putType == 0){
            Device *newSelect = nullptr;
            QPointF pos = event->position();
            int nx = ((int)(pos.x())-BaseX)/PX;
            int ny = ((int)(pos.y())-BaseY)/PX;
            if(nx>=0&&nx<WB&&ny>=0&&HB){
                auto it = devicesMap.find(QPair<int, int>(nx, ny));
                if(it != devicesMap.end()){
                    newSelect = *it;
                }
            }
            if(newSelect != nullptr){
                if(select != nullptr && select != newSelect){
                    select->setSelect(false);
                }
                if(select != newSelect){
                    select = newSelect;
                    int type = newSelect->getDeviceType();
                    if(type == 2){
                        // 按下传送带代表拖曳开始
                        auto con = (Conveyor *)select;
                        putType = 2; conveyorDown = true;
                        exist = true; reverse = false;
                        con->setVirt(true);
                        con->setSelectColor(0);
                        con->setSelect(true, true);
                        putConveyor.append(con);
                        select = nullptr;
                    }else select->setSelect(true);
                }
            }else{
                if(select != nullptr){
                    select->setSelect(false);
                    select = nullptr;
                }
                onDrag = true;
                dragX = pos.x(); dragY = pos.y();
                startBaseX = BaseX; startBaseY = BaseY;
                // qWarning()<<"start drag"<<dragX<<dragY;
            }
        }else{
            if(putType == 2){
                if(!conveyorDown){
                    conveyorDown = true;
                    exist = false; reverse = false;
                }
            }else if(putType == 8){
                mouseDown = true;
            }else{
                if(checkPut()){
                    // 放置非传送带类元素
                    if(putType == 3){
                        // 给采矿机赋值对应的矿
                        int type = putting->type;
                        int putX = putting->x, putY = putting->y;
                        putting->type = (gridMap[putY][putX]<<7)|(type & 0b1111111);
                    }
                    putting->setVirt(false);
                    putting->setSelect(false, true);
                    putting->setPos(putting->x, putting->y);
                    putting = nullptr;
                }else{
                    delete putting;
                    putting = nullptr;
                }
                emit resetToolButton(putType);
                putType = 0;
            }
        }
    }else if(event->button() == Qt::RightButton){
        if(putType != 0){
            // 取消放置
            if(putType == 2){
                for(auto &con:putConveyor){
                    delete con;
                }
                putConveyor.clear();
            }else{
                delete putting;
                putting = nullptr;
            }
            emit resetToolButton(putType);
            putType = 0;
        }else{
            Device *newSelect = nullptr;
            QPointF pos = event->position();
            int nx = ((int)(pos.x())-BaseX)/PX;
            int ny = ((int)(pos.y())-BaseY)/PX;
            if(nx>=0&&nx<WB&&ny>=0&&HB){
                auto it = devicesMap.find(QPair<int, int>(nx, ny));
                if(it != devicesMap.end()){
                    newSelect = *it;
                }
            }
            if(newSelect != nullptr){
                int type = (newSelect->type >> 3) & 0b1111;
                if(type == 0b0011){
                    // 采矿机还原成矿
                    int typ = newSelect->type>>7;
                    int nx = newSelect->x, ny = newSelect->y;
                    delete newSelect;
                    gridMap[ny][nx] = typ;
                }else if(type == 0b0110){
                    // 交付中心不可删除
                }else{
                    // 其他设备直接删除
                    delete newSelect;
                }
            }
        }
    }
}
void GameEngine::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){ // 左键选中或确认放置
        if(onDrag){
            onDrag = false;
            return ;
        }
        if(putType == 2 && conveyorDown){
            if(checkPut()){
                // 放置传送带类元素
                for(auto &dev:putConveyor){
                    dev->setVirt(false);
                    dev->setSelect(false, true);
                    dev->setPos(dev->x, dev->y, false);
                }
                putConveyor.clear();
            }else{
                for(auto &dev:putConveyor){
                    delete dev;
                }
                putConveyor.clear();
            }
            emit resetToolButton(putType);
            putType = 0;
            conveyorDown = false;
        }else if(putType == 8){
            mouseDown = false;
        }
    }
}
void GameEngine::paintEvent(QPaintEvent *){
    QPainter painter(this);
    if(waitInfity){
        painter.drawPixmap(QRect(0, 0, WW, WH), QPixmap(background));
    }else{
        drawMap(painter);
        for(auto &it:devices){
            int typ = ((it->type >> 3) & 0b1111);
            if(typ!=2){
                continue;
            }else it->draw(painter);
        }
        for(auto &item:items){
            if(item->isMark()){
                item->draw(painter);
            }
        }
        for(auto &item:items){
            if(!item->isMark()){
                item->draw(painter);
            }
        }
        for(auto &it:devices){
            int typ = ((it->type >> 3) & 0b1111);
            if(typ==2){
                continue;
            }else it->draw(painter);
        }
    }
    painter.end();
}
void GameEngine::drawMap(QPainter &painter){

    painter.drawPixmap(
        QRect(BaseX, BaseY, 2560, 1600),
        QPixmap(background)
        );
    painter.drawPixmap(
        QRect(BaseX%PX, BaseY%PX, WW+PX*2, WH+PX*2),
        QPixmap(GRID_IMG)
        );
    if(width() > WB*PX || height() > HB*PX){
        QPen pen(QColor(238, 151, 12, 200));
        pen.setWidth(4);
        painter.setPen(pen);
        if(width() > WB*PX){
            painter.drawLine(WB*PX, 0, WB*PX, HB*PX);
        }
        if(height() > HB*PX){
            painter.drawLine(0, HB*PX, WB*PX, HB*PX);
        }
    }

    for(int y=0;y<HB;++y){
        for(int x=0;x<WB;++x){
            if(gridMap[y][x]!=1 && (gridMap[y][x]&1)) continue;
            if(gridMap[y][x] == 1){
                painter.drawPixmap(
                    QRect(BaseX+x*PX+MA, BaseY+y*PX+MA, PX-MA*2, PX-MA*2),
                    QPixmap(BLOCK_IMG)
                    );
            }else if((gridMap[y][x]>>1&0b1111) == 1){
                painter.drawPixmap(
                    QRect(BaseX+x*PX+MA, BaseY+y*PX+MA, PX-MA*2, PX-MA*2),
                    QPixmap(ROUND_MINE_IMG)
                    );
            }else if((gridMap[y][x]>>1&0b1111) == 2){
                painter.drawPixmap(
                    QRect(BaseX+x*PX+MA, BaseY+y*PX+MA, PX-MA*2, PX-MA*2),
                    QPixmap(RECT_MINE_IMG)
                    );
            }else if((gridMap[y][x]>>1&0b1111) == 3){
                painter.drawPixmap(
                    QRect(BaseX+x*PX+MA, BaseY+y*PX+MA, PX-MA*2, PX-MA*2),
                    QPixmap(BUBBLE_MINE_IMG)
                    );
            }
        }
    }
    painter.save();
}

void GameEngine::play(){
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qint64 deltaUpdate = now - lastUpdate;
    qint64 deltaRender = now - lastRender;

    // 更新游戏逻辑
    if(deltaUpdate >= 16){
        flip ++;
        lastUpdate = now;
        if(!pause && !store.isVisible() && !tips.isVisible()){
            // 更新游戏逻辑
            for(auto &it:devices){
                if(it->virt) continue;
                it->update(flip);
            }
        }
    }

    // 更新渲染逻辑
    if(deltaRender >= 16){
        lastRender = now;
        update();
    }
}

void GameEngine::save(){
    if(gridMap.empty()) return ;

    QFile saveFile(savePath);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing:" << savePath;
        return ;
    }

    QTextStream out(&saveFile);
    out<<BaseX<<" "<<BaseY<<"\n";
    out<<process<<" "<<reqCount<<" "<<proCount<<" "<<coin<<"\n";
    out<<mapGrade<<" "<<centerGrade<<" "<<itemGrade<<"\n";
    out<<conveyorGrade<<" "<<minerGrade<<" "<<splitterGrade<<"\n\n";
    for(int i=0;i<HB;++i){
        for(int j=0;j<WB;++j){
            out<<gridMap[i][j]<<" ";
        }
        out<<"\n";
    }
    out<<"\n"<<demands.size()<<"\n";
    for(auto &demand:demands){
        out<<demand.first<<" "<<demand.second<<"\n";
    }
    saveFile.close();
}

void GameEngine::load(QFile *loadFile){
    if(loadFile == nullptr){
        expandMap(true);
    }else{
        // savePath = QFileInfo(*loadFile).absoluteFilePath();

        gridMap.clear();
        QTextStream in(loadFile);
        in>>BaseX>>BaseY;
        in>>process>>reqCount>>proCount>>coin;
        in>>mapGrade>>centerGrade>>itemGrade;
        in>>conveyorGrade>>minerGrade>>splitterGrade;
        expandMap(true, &in);

        demands.clear();
        int demandsLength = 0;
        in>>demandsLength;
        for(int i=0;i<demandsLength;++i){
            int dx, dy;
            in>>dx>>dy;
            demands.append(QPair<int, int>(dx, dy));
        }

        loadFile->close();
    }
    for(int y=0;y<HB;++y){
        for(int x=0;x<WB;++x){
            if(gridMap[y][x] <= 1 || !(gridMap[y][x]&1)) continue;
            int tp = (gridMap[y][x]>>3)&0b1111;
            switch(tp){
            case 2:
                new Conveyor(x, y, gridMap[y][x], nullptr);
                break;
            case 3:
                new Miner(x, y, gridMap[y][x], nullptr);
                break;
            case 4:
                new Splitter(x, y, gridMap[y][x], nullptr);
                break;
            case 5:
                new Trash(x, y, gridMap[y][x], nullptr);
                break;
            case 6:
                center = new Center(x, y, nullptr);
                connect(center, &Center::finishSection, this, &GameEngine::finishSection);
                connect(center, &Center::finishSubsection, this, &GameEngine::finishSubsection);
                break;
            case 7:
                new Combiner(x, y, gridMap[y][x], nullptr);
                break;
            case 9:
                new RotateRight(x, y, gridMap[y][x], nullptr);
                break;
            default:
                break;
            }
        }
    }
    setBack();
}

void GameEngine::expandMap(bool init, QTextStream *in){
    int SW = WB, SH = HB;
    WB = mapSize[mapGrade][0];
    HB = mapSize[mapGrade][1];
    gridMap.resize(HB);
    for(int y=0;y<HB;++y){
        gridMap[y].resize(WB);
        if(init){
            if(in == nullptr){
                for(int x=0;x<WB;++x){
                    gridMap[y][x] = baseGridMap[y][x];
                }
            }else{
                for(int x=0;x<WB;++x){
                    (*in)>>gridMap[y][x];
                }
            }
        }else if(y>=SH){
            for(int x=0;x<WB;++x){
                gridMap[y][x] = baseGridMap[y][x];
            }
        }else{
            for(int x=SW;x<WB;++x){
                gridMap[y][x] = baseGridMap[y][x];
            }
        }
    }
}

void GameEngine::finishSubsection(){
    // 弹出局部强化选项
    store.setPartial(true);
    store.setHidden(false);
    // 进行存档
    save();
}
void GameEngine::finishSection(){
    // 清空局部强化内容
    minerGrade = 0;
    conveyorGrade = 0;
    splitterGrade = 0;

    if(proCount == 3){
        // 开启无尽模式！
        pause = true;
        waitInfity = true;
        pauseBtn.setHidden(true); tipsBtn.setHidden(true);
        tools.setHidden(true); store.setHidden(true);
        save();

        infityBtn.set(585, 765, 483, 114, ":/img/infityButton.png", this);
        infityBtn.show();
        connect(&infityBtn, &ToolButton::clicked, this, [=]{
            pause = false;
            waitInfity = false;
            infityBtn.setHidden(true);
            pauseBtn.setHidden(false); tipsBtn.setHidden(false);
            tools.setHidden(false);
            proCount ++;
            setBack();
        });
    }
    setBack();
    // 进行存档
    save();
}
void GameEngine::setBack(){
    audio.setVolume(0.3);
    musicPlayer.setAudioOutput(&audio);
    switch(proCount){
    case 0:
        background = ":/img/sky.png";
        musicPlayer.setSource(QUrl(MUSIC_HEART));
        break;
    case 1:
        background = ":/img/secret.png";
        musicPlayer.setSource(QUrl(MUSIC_FEATHER));
        break;
    case 2:
        background = ":/img/IX.png";
        musicPlayer.setSource(QUrl(MUSIC_GALAXY));
        break;
    case 3:
        background = ":/img/Finish.png";
        musicPlayer.setSource(QUrl(MUSIC_JOURNEY));
        break;
    default:
        background = ":/img/Start.png";
        musicPlayer.setSource(QUrl(MUSIC_SUN));
    }
    update();
    musicPlayer.setLoops(-1);
    musicPlayer.play();
}

void GameEngine::resizeEvent(QResizeEvent *){
    pauseBtn.setPos(width()-PX*2, 0);
    tipsBtn.setPos(width()-PX, 0);
    tips.move((width()-tips.width())/2, (height()-tips.height())/2-20);
    tools.move((width()-tools.width())/2, height()-PX-40);
    store.move((width()-store.width())/2, (height()-store.height())/2-20);

    if(width() > WB*PX){
        BaseX = 0;
    }else BaseX = max(width()-WB*PX, BaseX);
    if(height() > HB*PX){
        BaseY = 0;
    }else BaseY = max(height()-HB*PX, BaseY);
}
