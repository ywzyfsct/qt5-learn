#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    /*      托盘图标     不生效了不知道为啥*/
    trayIcon = new QSystemTrayIcon(QIcon(":/new/prefix1/icon/ICON.ico"),this);
    option = new QAction(QIcon(":/new/prefix1/icon/option.ico"),"截图",this);
    exit = new QAction(QIcon(":/new/prefix1/icon/exit.ico"),"退出",this);
    connect(option,&QAction::triggered,this,&QWidget::showMaximized);
    connect(exit,&QAction::triggered,this,&QWidget::close);
    this->setToolTip("截图工具1.0");
    menu = new QMenu(this);
    menu->addAction(option);
    menu->addAction(exit);
    trayIcon->setContextMenu(menu);
    trayIcon->show();
    trayIcon->showMessage("截图工具1.0","ctrl+l 开启截图  ctrl+p 关闭程序\n选择截图区域后：\n右键鼠标或键盘按下esc取消选区\n选区只有右下角可以调节大小",QIcon(":/new/prefix1/icon/ICON.ico"),2000);

    this->setMouseTracking(true);

    /*      窗体样式     */
    this->setWindowFlags(Qt::FramelessWindowHint);          //无边框
    this->setAttribute(Qt::WA_TranslucentBackground, true); //透明背景

    MyGlobalShortCut *begin = new MyGlobalShortCut("ctrl+l",this);
    connect(begin,&MyGlobalShortCut::activated,this,&Widget::showMaximized);
    MyGlobalShortCut *end = new MyGlobalShortCut("ctrl+p",this);
    connect(end,&MyGlobalShortCut::activated,[=](){
        QMessageBox::information(this,"提示","截图工具已关闭","确定");
        Widget::close();
    });
}

Widget::~Widget(){

}

void Widget::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);   //覆盖模式（包括透明度）
    painter.fillRect(this->rect(),QColor(0,0,0,80));                //填充透明度的黑色背景
    QPen pen(Qt::blue,Qt::SolidLine);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawRect(rectf); //画矩形框
    painter.fillRect(rectf,QColor(0,0,0,1));
}

void Widget::mousePressEvent(QMouseEvent *event){
    //双击事件
    if(event->type()==QEvent::MouseButtonDblClick){
        if(hasArea){
            QPixmap pixmap=QApplication::screens().at(0)->grabWindow(QApplication::desktop()->winId(),rectf.x(),rectf.y(),rectf.width(),rectf.height());
            pixmap.save("D:\\Desktop\\1.png","png");
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setPixmap(pixmap);
            backOrExit();
            this->hide();
        }
    }
    else{
        if(event->button() == Qt::LeftButton){
            if(!pressing){
                pressing = true;            //已按下
                Press = event->pos();       //记录按下坐标
                if(!hasArea)               //如果不存在截图区域
                    LeftTop = Press;    //则Press 记录为 截图区域的左上角坐标LeftTop
                else {  //如果已存在截图区域 进入判断按下的位置为哪个区域
                    //若果按下了右下角区域（调整截图区大小的区域）    记录为1
                    if(QRect(RightBottom-QPoint(Marge,Marge),RightBottom+QPoint(Marge,Marge)).contains(Press)) ifPressSomeArea = 1;
                    //在截图区中间按下（移动截图位置）  记录为2
                    else if(rectf.adjusted(Marge,Marge,-Marge,-Marge).contains(Press)) ifPressSomeArea = 2;
                }
            }
        }
        //右键等同于按下ESC 返回或退出截图
        else if(event->button() == Qt::RightButton) {
            backOrExit();
        }
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event){
    NowPoint = event->pos();        //记录当前鼠标坐标
    SetCurrentCursor();             //设置光标样式
    if(pressing){                   //是按下状态
       if(!hasArea) {
            rectf = QRectF(LeftTop.x(),LeftTop.y(),NowPoint.x()-LeftTop.x(),NowPoint.y()-LeftTop.y()).normalized();
       }
       else {
            switch ( ifPressSomeArea ) {
            case 1:
                rectf = QRectF(LeftTop.x(),LeftTop.y(),NowPoint.x()-LeftTop.x(),NowPoint.y()-LeftTop.y()).normalized();
                break;
            case 2:
                QPoint movePoint = LeftTop+NowPoint-Press;
                if(movePoint.x()<0) movePoint.rx()=0;
                if(movePoint.y()<0) movePoint.ry()=0;
                if(movePoint.x()+rectf.width() > this->width()) movePoint.rx()=this->width()-rectf.width();
                if(movePoint.y()+rectf.height()>this->height()) movePoint.ry()=this->height()-rectf.height();
                rectf.moveTo(movePoint);
                break;
            }
       }
       repaint();
    }
    //qDebug()<<event->pos();
}

void Widget::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        if(ifPressSomeArea || !hasArea ) {
            LeftTop.setY(rectf.y());
            LeftTop.setX(rectf.x());
            RightBottom.setX(rectf.x()+rectf.width());
            RightBottom.setY(rectf.y()+rectf.height());
            hasArea = true;
            ifPressSomeArea = 0;
        }
        pressing = false;
    }
    qDebug()<<Press<<endl<<RightBottom;
}

//ESC键盘事件
void Widget::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Escape )
        backOrExit();
}

//返回或退出
void Widget::backOrExit(){
    if(hasArea){
        hasArea = false;
        rectf = QRectF(0,0,0,0);
        repaint();
    }
    else this->hide();
}
//设置相应位置光标的样式
void Widget::SetCurrentCursor()
{
    bool rb = QRect(RightBottom-QPoint(Marge,Marge),RightBottom+QPoint(Marge,Marge)).contains(NowPoint);
    bool inner =  rectf.adjusted(Marge,Marge,-Marge,-Marge).contains(NowPoint);
    if( rb || ifPressSomeArea==1 ) this->setCursor(Qt::SizeFDiagCursor);
    else if(inner || ifPressSomeArea==2) this->setCursor(Qt::SizeAllCursor);
    else this->setCursor(Qt::ArrowCursor);
}




