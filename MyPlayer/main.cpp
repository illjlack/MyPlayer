#include <QApplication>
#include <QSharedMemory>
#include <QLocalSocket>
#include <QMessageBox>

#include "MainWidget.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/app_icon.png")); // ����������ͼ��

    QSharedMemory sharedMemory;
    sharedMemory.setKey("UniqueApplicationKey"); // ����Ψһ�ļ�ֵ

    if (!sharedMemory.create(1)) {
        QLocalSocket socket;
        socket.connectToServer("UniqueApplicationKey:MyPlayer^...^");
        if (socket.waitForConnected(1000)) {
            socket.write("restore");
            socket.waitForBytesWritten();
            socket.close();
        }
        return 0; // �˳�Ӧ�ó���
    }

    MainWidget w;
    w.setWindowTitle("MyPlayer^"); // ȷ�����ڱ���Ψһ
    w.show();

    return a.exec();
}
