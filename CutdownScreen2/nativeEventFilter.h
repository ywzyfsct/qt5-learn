#ifndef NATIVEEVENTFILTER_H
#define NATIVEEVENTFILTER_H
#include <QAbstractNativeEventFilter>
#include <QObject>
#include <windows.h>
class SystemShortCut;

class NativeEventFilter : public QAbstractNativeEventFilter
{
public:
    NativeEventFilter(QObject *shortcut);
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
private:
    SystemShortCut *myshortcut;
};

#endif // NATIVEEVENTFILTER_H
