#include "widget.h"
Widget::~Widget()
{
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    Widget::Init();
    //Widget::widgetInit();
}

void Widget::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawPixmap(this->rect(),pixmap);                        //画出原画
    painter.fillRect(this->rect(),QColor(0,0,0,TranslateBit));      //填充透明黑色 透明度默认80
    if(CaptureRect.isValid()){      //isValid()是Qt自带函数 == 用于判断区域是否有效（即区域面积为空就无效）
        painter.drawPixmap(CaptureRect,pixmap.copy(CaptureRect));   //画局部图
        painter.setPen(QPen(QColor(0,180,180),Qt::SolidLine));      //绿色实线
        painter.drawRect(CaptureRect);                              //画矩形框
        for(QRect rect:adjustAreaRects)                             //画八个小正方形 这是调整截图区大小的区域
            painter.fillRect(rect,QColor(0,180,180));
    }
}

void Widget::keyPressEvent(QKeyEvent *event){
    cout<<event->key();
    keys.append(event->key());
    if(16777249==event->key()){ //16777249 是ctrl的值 按住ctrl让rgb变成十六进制
        QColor color = pixmap.toImage().pixelColor(now);
        QString str1 = QString("长×宽:%1×%2\n").arg(CaptureRect.width()).arg(CaptureRect.height());
        QString str2 = QString("RGB:#%1%2%3\n按C复制RGB色号\n右键返回或退出截图").arg(color.red(),2,16,QLatin1Char('0')).arg(color.green(),2,16,QLatin1Char('0')).arg(color.blue(),2,16,QLatin1Char('0'));
        labels->setTextLabel(str1+str2);
    }
    if(67==event->key()){   //67 为 ‘c'
        QColor color = pixmap.toImage().pixelColor(now);
        QClipboard *clipboard = QGuiApplication::clipboard();
        if(!keys.contains(16777249)){   //如果是按下了组合键 ctrl+c 则复制 十六进制的rgb值
            clipboard->setText(QString("%1,%2,%3").arg(color.red()).arg(color.green()).arg(color.blue()));
        } else{
            clipboard->setText(QString("#%1%2%3").arg(color.red(),2,16,QLatin1Char('0')).arg(color.green(),2,16,QLatin1Char('0')).arg(color.blue(),2,16,QLatin1Char('0')));
        }
    }
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    keys.removeOne(event->key());   //松开的键出栈
}

bool Widget::event(QEvent *event){
    QMouseEvent *e = static_cast<QMouseEvent*>(event);
    if(event->type()==QEvent::MouseButtonDblClick){
        QClipboard *clipboard = QGuiApplication::clipboard();
        //captruePixmap = QGuiApplication::primaryScreen()->grabWindow(0,CaptureRect.x(),CaptureRect.y(),CaptureRect.width(),CaptureRect.height());
        captruePixmap = pixmap.copy(CaptureRect);
        clipboard->setPixmap(captruePixmap);
        Widget::exitScreenshot(true);
    }
    else if(event->type()==QEvent::MouseButtonPress){
        if(e->button()==Qt::LeftButton){
            PressPoint = e->pos();
            mouse_pressed = true;     //按下标志置为真
            if(!CaptureRect.isValid()){
                LeftTop = PressPoint;
                CaptureRect = QRect(LeftTop,LeftTop);
                mouseArea = 8;
                Widget::displayCaptureInfo(LeftTop);
                if(!labels->isVisible()) labels->show();
            }
            else {
                Widget::markCurrentRect();                  //标记当前窗口四个角
                mouseArea = pressAreaContains(e->pos());    //记录按下时的区域信息
            }
        }
        else if(e->button()==Qt::RightButton){
            QPoint now = e->pos();
            if(CaptureRect.isValid()&&CaptureRect.contains(now)){   //右键截图区内 弹出菜单
                rightMenu->move(now);
                rightMenu->show();
            }
            else Widget::exitScreenshot(false);                     //右键截图区外重选截图或退出截图
        }
    }
    else if (event->type()==QEvent::MouseMove) {
        now = e->pos();
        Widget::displayCaptureInfo(now);
        if(mouse_pressed){
            adjustCurrentRect(e->pos());
            repaint();
            Widget::updateAdjustAreaRects();
            //TempRect = CaptureRect;
        }
        else Widget::setCurrentCursor(pressAreaContains(e->pos()));
    }
    else if (event->type()==QEvent::MouseButtonRelease) {
        if(e->button()==Qt::LeftButton){
            mouse_pressed = false;
        }
    }
    return QWidget::event(event);
}

///总初始化函数
void Widget::Init()
{
    Widget::widgetInit();
    Widget::trayIconInit();
    Widget::rightMenuInit();
}

///widget初始化
void Widget::widgetInit()
{
    /*      窗体样式     */
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Tool|Qt::WindowStaysOnTopHint); //无边框
    this->setAttribute(Qt::WA_TranslucentBackground, true); //透明背景
    this->setMouseTracking(true);
    //this->showFullScreen();
    //cout<<this->width()<<' '<<this->height();
    pixmap = QGuiApplication::primaryScreen()->grabWindow(0);   //获取全局截图

    labels = new InfoLabel(this);
    labels->move(-1,-1);
}

///托盘图标初始化
void Widget::trayIconInit()
{
    /*      托盘图标     */
    trayIcon = new QSystemTrayIcon(QIcon(":/new/prefix1/icon/ICON.ico"),this);
    option = new QAction(QIcon(":/new/prefix1/icon/option.ico"),"截图",this);
    exit = new QAction(QIcon(":/new/prefix1/icon/exit.ico"),"退出",this);
    tips = new QAction("提示",this);
    connect(option,&QAction::triggered,this,static_cast<void (Widget::*)()>(&Widget::startCapture));
    connect(exit,&QAction::triggered,this,&QApplication::quit);
    connect(trayIcon,&QSystemTrayIcon::activated,this,static_cast<void (Widget::*)(QSystemTrayIcon::ActivationReason)>(&Widget::startCapture));
    connect(tips,&QAction::triggered,this,&Widget::aboutMessage);
    trayIcon->setToolTip("截图工具0.1");
    menu = new QMenu(this);
    menu->addAction(option);
    menu->addAction(exit);
    menu->addAction(tips);
    trayIcon->setContextMenu(menu);
    trayIcon->show();
    Widget::aboutMessage();
}

///右键菜单初始化
void Widget::rightMenuInit()
{
    rightMenu = new QMenu(this);
    rightMenu->setWindowOpacity(0.8);
    rightMenu->setFont(QFont("华文新魏",12));
    QAction *action1 = new QAction("另存为",this);
    QAction *action2 = new QAction("保存到桌面",this);
    QAction *action3 = new QAction("钉在桌面",this);
    QAction *action4 = new QAction("完成截图",this);
    rightMenu->addAction(action1);
    rightMenu->addAction(action2);
    rightMenu->addAction(action3);
    rightMenu->addAction(action4);
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
    rightMenu->addAction(new QAction("完成截图",this));
}

///重选截图区 或 退出截图
void Widget::exitScreenshot(bool exit)
{
    if(!exit){  //不直接退出就看是否返回截图 不然还是直接退
        if(CaptureRect.isValid()) CaptureRect = QRect();
        else this->hide();
    } else{
        CaptureRect = QRect();
        this->hide();
    }
    labels->hide();
    repaint();
}

///更新调节区域
void Widget::updateAdjustAreaRects()
{
    adjustAreaRects[0] = QRect(QPoint(CaptureRect.topLeft()-QPoint(Marge/2,Marge/2)),QSize(Marge,Marge));
    adjustAreaRects[1] = adjustAreaRects[0].translated(CaptureRect.width()/2,0);
    adjustAreaRects[2] = adjustAreaRects[1].translated((CaptureRect.width()+1)/2,0);
    adjustAreaRects[3] = adjustAreaRects[0].translated(0,CaptureRect.height()/2);
    adjustAreaRects[4] = adjustAreaRects[2].translated(0,CaptureRect.height()/2);
    adjustAreaRects[5] = adjustAreaRects[3].translated(0,(CaptureRect.height()+1)/2);
    adjustAreaRects[6] = adjustAreaRects[5].translated(CaptureRect.width()/2,0);
    adjustAreaRects[7] = adjustAreaRects[4].translated(0,(CaptureRect.height()+1)/2);
}

///鼠标所点击的区域
int8_t Widget::pressAreaContains(QPoint p)
{
    for(int i=0;i<8;i++){
        if(adjustAreaRects[i].contains(p))
            return i+1;
    }
    if(CaptureRect.contains(p)) return 0;
    return 9;
}

///标记CaptureRect四个角
void Widget::markCurrentRect()
{
    LeftTop = CaptureRect.topLeft();
    LeftBottom = QPoint(CaptureRect.x(),CaptureRect.y()+CaptureRect.height());
    RightTop = QPoint(CaptureRect.x()+CaptureRect.width(),CaptureRect.y());
    RightBottom = LeftTop + QPoint(CaptureRect.width(),CaptureRect.height());
}

///调节当前窗口大小或位置
void Widget::adjustCurrentRect(QPoint now)
{
    switch(mouseArea)               //检测按下区域
    {
    case 0://中间
        now = LeftTop+now-PressPoint;  //全局左上角坐标
        Widget::edgeCheck(now);
        CaptureRect.moveTo(now);
        break;
    case 1://左上
        //CaptureRect = QRect(now.x(),now.y(),RightBottom.x()-now.x(),RightBottom.y()-now.y()).normalized();
        CaptureRect = QRect(now,RightBottom).normalized();
        break;
    case 8://右下
        //CaptureRect = QRect(LeftTop.x(),LeftTop.y(),now.x()-LeftTop.x(),now.y()-LeftTop.y()).normalized();
        CaptureRect = QRect(LeftTop,now).normalized();
        break;
    case 2://上
        //CaptureRect = QRect(LeftTop.x(),now.y(),RightBottom.x()-LeftTop.x(),RightBottom.y()-now.y()).normalized();
        CaptureRect = QRect(QPoint(LeftTop.x(),now.y()),RightBottom).normalized();
        break;
    case 7://下
        //CaptureRect = QRect(LeftTop.x(),LeftTop.y(),RightBottom.x()-LeftTop.x(),now.y()-LeftTop.y()).normalized();
        CaptureRect = QRect(LeftTop,QPoint(RightBottom.x(),now.y())).normalized();
        break;
    case 3://右上
        //CaptureRect = QRect(LeftBottom.x(),now.y(),now.x()-LeftBottom.x(),LeftBottom.y()-now.y()).normalized();
        CaptureRect = QRect(QPoint(LeftBottom.x(),now.y()),QPoint(now.x(),RightBottom.y())).normalized();
        break;
    case 6://左下
        //CaptureRect = QRect(now.x(),RightTop.y(),RightTop.x()-now.x(),now.y()-RightTop.y()).normalized();
        CaptureRect = QRect(QPoint(now.x(),LeftTop.y()),QPoint(RightBottom.x(),now.y())).normalized();
        break;
    case 4://左
        //CaptureRect = QRect(now.x(),LeftTop.y(),RightTop.x()-now.x(),RightBottom.y()-RightTop.y()).normalized();
        CaptureRect = QRect(QPoint(now.x(),LeftTop.y()),RightBottom).normalized();
        break;
    case 5://右
        //CaptureRect = QRect(LeftTop.x(),LeftTop.y(),now.x()-LeftTop.x(),RightBottom.y()-RightTop.y()).normalized();
        CaptureRect = QRect(LeftTop,QPoint(now.x(),RightBottom.y())).normalized();
        break;
    }
}

///显示截图窗口信息到Label上
void Widget::displayCaptureInfo(QPoint &now)
{
    QPoint m = CaptureRect.topLeft();
    if(m.y()>=labels->height()) labels->move(m.x(),m.y()-labels->height());
    else labels->move(m.x(),m.y()+CaptureRect.height());
    QColor color = pixmap.toImage().pixelColor(now);
    QString str1 = QString("长×宽:%1×%2\n").arg(CaptureRect.width()).arg(CaptureRect.height());
    QString str2 = QString("RGB:%1,%2,%3\n按C复制RGB色号\n右键返回或退出截图").arg(color.red()).arg(color.green()).arg(color.blue());
    labels->setTextLabel(str1+str2);
    labels->setPixmapLabel(pixmap.copy(now.x()-20,now.y()-20,40,40));
}

///Capture窗口的边界检测
uint8_t Widget::edgeCheck(QPoint &nowPoint)
{
    uint8_t r = 0;
    if(nowPoint.x()<0) {nowPoint.rx()=0;r = 1;}  //左超界
    if(nowPoint.x()+CaptureRect.width()>this->width()) {nowPoint.rx()=this->width()-CaptureRect.width();r = 2;}    //右超界
    if(nowPoint.y()<0) {nowPoint.ry()=0;r = 3;}  //上
    if(nowPoint.y()+CaptureRect.height()>this->height()) {nowPoint.ry()=this->height()-CaptureRect.height();r = 4;}//下
    return r;   //没超界
}

//点击右键托盘菜单上的截图 开启截图
void Widget::startCapture()
{
    pixmap = QGuiApplication::primaryScreen()->grabWindow(0);
    this->showFullScreen();
}

//弹出软件信息
void Widget::aboutMessage()
{
    trayIcon->showMessage("截图工具0.1",
                          "仿QQ截图0.1\n选区外右键 取消选区或取消截屏\n"
                          "双击保存截图到剪切板\n"
                          "ctrl+c复制十六进制RGB颜色值\n"
                          "功能暂且较弱 仅供学习娱乐",
                          QIcon(":/new/prefix1/icon/ICON.ico"),
                          2000);
}
//双击托盘图标 开启截图
void Widget::startCapture(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick){
        Widget::startCapture();
    }
}

///设置当前光标样式
void Widget::setCurrentCursor(uint8_t p)
{
    switch(p){
    case 1:
    case 8:
        this->setCursor(Qt::SizeFDiagCursor);
        break;
    case 2:
    case 7:
        this->setCursor(Qt::SizeVerCursor);
        break;
    case 3:
    case 6:
        this->setCursor(Qt::SizeBDiagCursor);
        break;
    case 4:
    case 5:
        this->setCursor(Qt::SizeHorCursor);
        break;
    case 9:
        this->setCursor(Qt::ArrowCursor);
        break;
    default:
        this->setCursor(Qt::SizeAllCursor);
    }
}

