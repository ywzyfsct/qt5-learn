#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QMessageBox>
#include <windows.h>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *event) override;
private:
    QString text = "请拖入需要打包的exe文件";
    QColor color;
    QPoint LeftTop;
    QPoint PressPoint;
    bool Pressed=false;
};
#endif // WIDGET_H
