
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <httpserver/httplistener.h>
#include <logging/filelogger.h>
#include "root_handler.h"
#include "sys_tray.h"

int main(int argc, char *argv[]) {

    // Initialize the core application
    QApplication* app = new QApplication(argc, argv);
    app->setQuitOnLastWindowClosed( false );
    SysTray * st = new SysTray( app );
    st->show();

    return app->exec();
}
