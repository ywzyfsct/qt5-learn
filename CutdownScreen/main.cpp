#include "widget.h"
#include <QSharedMemory>
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const char *key = "CutScreen";
    QSharedMemory sharedMem(key);
    Widget w;
    if (sharedMem.create(strlen(key))){
        w.showMaximized();
        return a.exec();
    } else QMessageBox::information(&w,"提示","截图工具已在运行\n按 ctrl+l 开启截图\n按 ctrl+p 关闭程序","确定");
    return 0;
}
