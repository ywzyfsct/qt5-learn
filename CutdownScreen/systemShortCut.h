#ifndef SYSTEMSHORTCUT_H
#define SYSTEMSHORTCUT_H

#include <QObject>
#include <QApplication>
#include <windows.h>
#include <QKeySequence>
#include <QMessageBox>
#include "nativeEventFilter.h"

class SystemShortCut : public QObject
{
    Q_OBJECT
public:
    explicit SystemShortCut(QString key, QObject *parent = nullptr);
    ~SystemShortCut();
    quint32 nKey=0,nMod=0;
private:
    NativeEventFilter *filter=nullptr;
    QKeySequence m_key;
    Qt::Key key;
    Qt::KeyboardModifiers mod;
    quint32 nativeKey(Qt::Key keycode);
    quint32 nativeMod(Qt::KeyboardModifiers modifiers);
    bool unregisterHotKey();
    bool registerHotKey();
    HWND nHandle = 0;
signals:
    void activated();
};

#endif // SYSTEMSHORTCUT_H
