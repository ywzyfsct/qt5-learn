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
#include <MyGlobalShortCut/MyGlobalShortCut.h>
#include <QMessageBox>
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
    QSystemTrayIcon *trayIcon;  //托盘图标
    QAction *option;            //截图选项
    QAction *exit;              //退出选项
    QMenu *menu;                //右键托盘菜单

    QPoint Press;       //按下坐标
    QPoint NowPoint;    //当前鼠标坐标
    QPoint RightBottom; //截图框右下角坐标
    QPoint LeftTop;     //截图框左上角坐标

    QRectF rectf;
    bool pressing=false;
    bool hasArea = false;

    int8_t ifPressSomeArea = false;

    QRect EightPointRect[8];
    int8_t state;
    QPoint pressedPoint;
private:
    void backOrExit();
    void SetCurrentCursor();
};
#endif // WIDGET_H
