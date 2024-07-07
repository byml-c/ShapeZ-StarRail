#include "trash.h"

Trash::Trash(int x, int y, int type, QObject *parent, bool virt)
    : Device(x, y, 1, 1, type, TRASH_IMG, parent, virt)
{

}
