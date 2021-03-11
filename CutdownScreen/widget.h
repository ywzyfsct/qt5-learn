#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QEvent>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QClipboard>
#include <systemShortCut.h>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#define Marge 5

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:
    QSystemTrayIcon *trayIcon;      //托盘图标
    QAction *option;                //截图选项
    QAction *exit;                  //退出选项
    QMenu *menu;                    //右键托盘菜单

    QPoint Press;                   //按下坐标
    QPoint NowPoint;                //当前鼠标坐标
    QPoint RightBottom;             //截图框右下角坐标
    QPoint LeftTop;                 //截图框左上角坐标
    QRectF rectf;                   //截图区域
    QPixmap pixmap;                 //图像保存

    bool pressing=false;            //是否按下
    bool hasArea = false;           //是否存在截图区域

    int8_t ifPressSomeArea = false; //鼠标状态

    QLabel *label = nullptr;        //像素显示
private:
    void backOrExit();              //返回或退出
    void SetCurrentCursor();        //设置当前光标样式
};
#endif // WIDGET_H
