#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QGuiApplication>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QEvent>
#include <QLabel>
#include <QKeyEvent>
#include <QClipboard>
#include "infolabel.h"
#include <QDebug>
#define Marge 5
#define cout qDebug() << "[文件:" << __FILE__<<"的第"<< __LINE__ << "行]输出:"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    void Init();
    void widgetInit();
    void trayIconInit();
    void rightMenuInit();
private:
    int8_t pressAreaContains(QPoint p);
    uint8_t edgeCheck(QPoint &nowPoint);
    void startCapture();
    void aboutMessage();
    void markCurrentRect();
    void updateAdjustAreaRects();
    void exitScreenshot(bool exit);
    void setCurrentCursor(uint8_t p);
    void adjustCurrentRect(QPoint now);
    void displayCaptureInfo(QPoint &p);
    void startCapture(QSystemTrayIcon::ActivationReason reason);
protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override;

private:
    ///托盘图标
    QSystemTrayIcon *trayIcon;
    ///截图选项
    QAction *option;
    ///退出选项
    QAction *exit;
    ///提示
    QAction *tips;
    ///右键托盘菜单
    QMenu *menu;

    ///透明度
    uint8_t TranslateBit = 80;
    ///右键菜单
    QMenu *rightMenu;

    ///截图区域
    QRect CaptureRect;
    ///全屏图
    QPixmap pixmap;
    ///截取图
    QPixmap captruePixmap;
    ///调整截图区域
    QRect adjustAreaRects[8];
    ///按下的坐标
    QPoint PressPoint;
    ///原截图区的四个坐标
    QPoint LeftTop,RightBottom,LeftBottom,RightTop;
    ///鼠标是否按下
    bool mouse_pressed=false;
    ///鼠标点击的区域（这里指调节截图区大小的八个小正方形）
    uint8_t mouseArea;
    ///自定义Label（放大镜，取色器，文本显示）
    InfoLabel *labels;
    ///当前鼠标坐标
    QPoint now;
    ///按键检测栈
    QList<int> keys;
};
#endif // WIDGET_H
