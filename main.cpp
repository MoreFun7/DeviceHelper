#include "devicehelper.h"

#include <QScreen>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("DeviceHelper");  //设置应用程序名称
    a.setWindowIcon(QIcon(":/ui/res/ProgramIcon.png"));  //设置应用程序图标
    QApplication::setApplicationVersion(QString(APP_VERSION));  //设置应用程序版本号
    DeviceHelper w;
    w.resize(QSize(450, 500));
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - w.width()) / 2;
    int y = (screenGeometry.height() - w.height()) / 2;
    w.move(x, y);
    w.show();
    return a.exec();
}
