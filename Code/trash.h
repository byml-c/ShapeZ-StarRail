#ifndef TRASH_H
#define TRASH_H

#include <QObject>

#include "device.h"

class Trash : public Device
{
    Q_OBJECT
public:
    explicit Trash(int x, int y, int type, QObject *parent = nullptr, bool virt = false);

signals:
};

#endif // TRASH_H
