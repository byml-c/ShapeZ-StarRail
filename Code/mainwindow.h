#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QTextStream>
#include <QMainWindow>
#include "welcome.h"
#include "gameEngine.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Welcome *welcome;
    GameEngine *engine;

    void resizeEvent(QResizeEvent *event) override;
};
#endif // MAINWINDOW_H
