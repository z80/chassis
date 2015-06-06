
#include <QtGui>
#include "process_xmpp_server.h"
#include "process_tor.h"

int main( int argc, char * argv[] )
{
    QApplication app( argc, argv );

    ProcessXmppServer a( 0 );
    ProcessTor t( 0 );

    int res = app.exec();
    return res;
}

