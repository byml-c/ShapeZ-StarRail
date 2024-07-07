#ifndef ERASER_H
#define ERASER_H

#include "device.h"

class Eraser : public Device
{
public:
    explicit Eraser(int x, int y, int type, QObject *parent = nullptr, bool virt=true);
};

#endif // ERASER_H
