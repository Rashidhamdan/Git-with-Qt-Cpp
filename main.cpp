#include "MainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("git-gui");
    app.setDesktopFileName("git-gui");
    app.setApplicationDisplayName("Git GUI");

    QIcon appIcon(":/icons/icons/app_icon.png");
    app.setWindowIcon(appIcon);

    MainWindow window(nullptr, "0.8");
    window.setWindowIcon(appIcon); //to fix
    window.show();

    return app.exec();
}
