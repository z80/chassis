
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
    
    res = io.setPower( true );
    res = io.setLed( true );
    res = io.setLed( false );
    res = io.setLed( false );

    io.close();

    return 0;
}

