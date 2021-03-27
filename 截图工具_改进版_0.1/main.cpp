#include "widget.h"

#include <QApplication>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const char *key = "ScreenShot1.0";
    QSharedMemory sharedMem(key);
    Widget w;
    if (sharedMem.create(strlen(key))){
        return a.exec();
    }
    return 0;
}
