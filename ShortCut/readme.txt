使用步骤：

一、程序中包含systemShortCut.h（系统热键）

二、创建对象：
参数一、快捷键的字符串 ctrl/shift/alt + 键盘按键的字符(如F2等)
参数二、接收信息的窗体对象，需要传入一个窗体类
SystemShortCut *shortcut = new SystemShortCut("ctrl+n", this);

三、绑定事件：
信号：只有actived一个信号
connect(shortcut,&SystemShortCut::actived,this,&Widget::close);

若要修改快捷方式 需要删除 重新创建对象 并重新绑定事件
delete shortcut;
shortcut = new SystemShortCut("ctrl+p", this);
connect(shortcut,&SystemShortCut::actived,this,&Widget::close);