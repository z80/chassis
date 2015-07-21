/**
  @file
  @author Stefan Frings
*/

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <httpserver/httplistener.h>
#include <logging/filelogger.h>
#include "root_handler.h"
#include "sys_tray.h"


/**
  Entry point of the program.
*/
int main(int argc, char *argv[]) {

    // Initialize the core application
    QApplication* app = new QApplication(argc, argv);
    SysTray * st = new SysTray( app );
    st->show();

    return app->exec();
}
