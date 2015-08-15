
#include "chassis_handler.h"
#include "ctrlboard_io.h"

class ChassisHandler::PD
{
public:
    PD() {}
    ~PD() {}

    CtrlboardIo io;
    int camX;
    int camY;
    bool light;
};


ChassisHandler::ChassisHandler( QObject * parent )
:   QObject( parent )
{
    pd = new PD();
    pd->camX = pd->camY = 0;
    pd->light = false;
}

ChassisHandler::~ChassisHandler()
{
    delete pd;
}

bool ChassisHandler::service( HttpRequest& request, HttpResponse& response )
{
    if ( request.getPath() != "/chassis" )
        return false;
    CtrlboardIo & io = pd->io;
    bool res = io.isOpen();
    if ( !res )
    {
        res = io.open();
        if ( res )
        {
            io.setPower( true );
            io.setServoEn( true );
        }
    }
    if ( !res )
        return true;    // True means this request belongs to this handler.

    // Parse request and perform appropriate actions.
    QString data = QString::fromAscii( request.getBody() );
    QRegExp re( "(\\w{1,})\\s{0,}=\\s{0,}(-{0,1}\\d{1,})" );
    int pos = 0;
    int camX = pd->camX;
    int camY = pd->camY;
    while ( ( pos = re.indexIn( data, pos ) ) != -1 )
    {
        QString key   = re.cap( 1 );
        QString value = re.cap( 2 );
        pos += re.matchedLength();
        if ( key == "camX" )
            camX = value.toInt();
        else if ( key == "camY" )
            camY = value.toInt();
        else if ( key == "light" )
        {
            pd->light = ( value.toInt() > 0 );
            io.setLed( pd->light );
        }
    }
    if ( ( camX != pd->camX ) || 
         ( camY != pd->camY ) )
    {
        io.setServo( camX, camY );
        pd->camX = camX;
        pd->camY = camY;
    }
    return true;
}





