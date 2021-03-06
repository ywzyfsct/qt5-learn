#include "nativeEventFilter.h"
#include "systemShortCut.h"

NativeEventFilter::NativeEventFilter(QObject *shortcut)
{
    myshortcut = static_cast<SystemShortCut *>(shortcut);
}

bool NativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result){
    if(eventType == "windows_generic_MSG")
     {
         MSG *msg = static_cast<MSG *>(message);

         if(msg->message == WM_HOTKEY)
         {
             const quint32 key = HIWORD(msg->lParam);
             const quint32 mod = LOWORD(msg->lParam);

             if(key==myshortcut->nKey && mod==myshortcut->nMod)
             {
                 myshortcut->activated();
                 return true;
             }
         }

     }
     return false;
}
