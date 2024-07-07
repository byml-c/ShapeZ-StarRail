#include "eraser.h"

Eraser::Eraser(int x, int y, int type, QObject *parent, bool virt)
    : Device(x, y, 1, 1, type, ERASER_BTN, parent, virt)
{}
