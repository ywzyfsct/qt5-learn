#include "widget.h"
#include <QtDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint); //无边框
    this->setAttribute(Qt::WA_TranslucentBackground, true); //透明背景
    this->resize(400,250);
    QScreen *screen = QGuiApplication::primaryScreen();
    this->move((screen->virtualGeometry().width()-400)/2,(screen->virtualGeometry().height()-400)/2);
    this->setFont(QFont("楷体",16,QFont::Bold));
    color = QColor(255,255,255);
    this->setAcceptDrops(true);
}

void Widget::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.fillRect(this->rect(),QColor(0,0,0,60));
    painter.setFont(this->font());
    painter.setPen(color);
    painter.drawText(this->rect(),Qt::AlignCenter,this->text);
}

bool Widget::event(QEvent *event){
    QMouseEvent *e = static_cast<QMouseEvent*>(event);
    if(event->type()==QEvent::MouseButtonPress){
        if(e->button()==Qt::LeftButton){
            Pressed = true;
            LeftTop = this->pos();
            PressPoint = e->globalPos();
        } else if(e->button()==Qt::RightButton){
            if(0==QMessageBox::information(this,"贤者退场","确定用完了?","是的呢","再想想")){
                this->close();
            }
        }
    }
    else if(event->type()==QEvent::MouseMove){
        if(Pressed)
            this->move(LeftTop+e->globalPos()-PressPoint);
    }
    else if(event->type()==QEvent::MouseButtonRelease){
        if(e->button()==Qt::LeftButton) Pressed = false;
    }
    if(event->type()==QEvent::Enter){
        color = QColor(0,180,180);
        repaint();
    }
    else if(event->type()==QEvent::Leave){
        color = QColor(255,255,255);
        text = "请拖入需要打包的exe文件";
        repaint();

    }
    QDropEvent *de = static_cast<QDragEnterEvent*>(event);
    if(event->type()==QEvent::DragEnter){
        if( de->mimeData()->hasUrls() ){
            QString url = de->mimeData()->urls().first().toLocalFile().replace("/","\\");
            if(url.endsWith("exe")){
                de->acceptProposedAction();
                color = QColor(0,255,0);
                text = "放下文件以开始生成依赖库";
            }
            else{
                color = QColor(255,0,0);
                text = "请拖入release生成的exe文件";
            }
                repaint();
        }
    }
    if(event->type()==QEvent::DragLeave){
        text = "请拖入需要打包的exe文件";
        color = QColor(255,255,255);
        repaint();
    }
    if(event->type() ==QEvent::Drop){
        text = "您想打包到哪个文件夹？";
        repaint();
        QString url = de->mimeData()->urls().first().toLocalFile().replace("/","\\");
        qDebug()<<url;
        QString filename = QFileDialog::getExistingDirectory(this,"打包到...","../").replace("/","\\");
        text = "正在打包...";
        repaint();
        qDebug()<<filename;
        if(!filename.isEmpty()){
            filename.replace("/","\\");
            QString exename = url.mid(url.lastIndexOf('\\')+1);
            system(QString("copy /Y %1 %2").arg(url).arg(filename).toLatin1());
            system(QString("cd /d %1 && windeployqt %2").arg(filename).arg(exename).toLatin1());
            text = "打包完成";
            color = QColor(0,255,0);
            repaint();
        } else{
            text = "已取消操作";
            repaint();
        }
    }
    return QWidget::event(event);
}










