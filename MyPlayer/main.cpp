#include <QApplication>
#include <QSharedMemory>
#include <QLocalSocket>
#include <QMessageBox>

#include "MainWidget.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/app_icon.png")); // 设置任务栏图标

    QSharedMemory sharedMemory;
    sharedMemory.setKey("UniqueApplicationKey"); // 设置唯一的键值

    if (!sharedMemory.create(1)) {
        QLocalSocket socket;
        socket.connectToServer("UniqueApplicationKey:MyPlayer^...^");
        if (socket.waitForConnected(1000)) {
            socket.write("restore");
            socket.waitForBytesWritten();
            socket.close();
        }
        return 0; // 退出应用程序
    }

    MainWidget w;
    w.setWindowTitle("MyPlayer^"); // 确保窗口标题唯一
    w.show();

    return a.exec();
}
