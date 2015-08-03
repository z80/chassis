
#include <QtGui>
#include <QApplication>
#include "ctrlboard_io.h"



int main( int argc, char * argv[] )
{
    QApplication app( argc, argv );

    CtrlboardIo io;
    bool res = io.open();
    if ( !res )
        return -1;
    std::string stri;
    //res = io.firmware( stri );
    //if ( !res )
    //    return -2;
    
    res = io.setDbgLed( true );
    res = io.setDbgLed( false );

    return 0;
}

