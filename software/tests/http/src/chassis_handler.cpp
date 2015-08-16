
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
    bool powerEn;
    QMutex mutex;

    int newCamX,
        newCamY,
        newLight;
    QTime powerTimeout;
    QTimer * timer;

    bool setPower( bool en );
};

bool ChassisHandler::PD::setPower( bool en )
{
    bool res = io.isOpen();
    if ( !res )
    {
        res = io.open();
        if ( !res )
            return false;
    }
    if ( en )
    {
        if ( !powerEn )
        {
            io.setPower( true );
            io.setServoEn( true );
        }
    }
    else
    {
        if ( powerEn )
        {
            io.setServoEn( false );
            io.setLed( false );
            io.setPower( false );
            light = false;
        }
    }
    return true;
}


ChassisHandler::ChassisHandler( QObject * parent )
:   QObject( parent )
{
    pd = new PD();
    pd->camX = pd->camY = pd->newCamX = pd->newCamY = 0;
    pd->light = pd->newLight = false;
    pd->powerEn = false;

    pd->powerTimeout.start();

    pd->timer = new QTimer( this );
    pd->timer->setInterval( 1000 );
    connect( pd->timer, SIGNAL(timeout()), this, SLOT( slotTimeout() ) );
    pd->timer->start();
}

ChassisHandler::~ChassisHandler()
{
    delete pd;
}

bool ChassisHandler::service( HttpRequest& request, HttpResponse& response )
{
    if ( request.getPath() != "/chassis" )
        return false;

    QMutex & mutex = pd->mutex;

    // Parse request and perform appropriate actions.
    QString data = QString::fromAscii( request.getBody() );
    QRegExp re( "(\\w{1,})\\s{0,}=\\s{0,}(-{0,1}\\d{1,})" );
    int pos = 0;
    int & camX = pd->newCamX;
    int & camY = pd->newCamY;
    while ( ( pos = re.indexIn( data, pos ) ) != -1 )
    {
        QString key   = re.cap( 1 );
        QString value = re.cap( 2 );
        pos += re.matchedLength();
        if ( key == "camX" )
        {
            QMutexLocker lock( &mutex );
                camX = value.toInt();
        }
        else if ( key == "camY" )
        {
            QMutexLocker lock( &mutex );
                camY = value.toInt();
        }
        else if ( key == "light" )
        {
            QMutexLocker lock( &mutex );
                pd->newLight = ( value.toInt() > 0 );
        }
    }

    return true;
}

void ChassisHandler::slotTimeout()
{
    QMutex & mutex   = pd->mutex;
    CtrlboardIo & io = pd->io;

    mutex.lock();
        int newCamX = pd->newCamX;
        bool updateCamX = ( newCamX != pd->camX );
        int newCamY = pd->newCamY;
        bool updateCamY = ( newCamY != pd->camY );
    mutex.unlock();

    if ( ( updateCamX ) || ( updateCamY ) )
    {
        pd->powerTimeout.restart();
        if ( pd->setPower( true ) )
        {
            io.setServo( newCamX, newCamY );
            pd->camX = newCamX;
            pd->camY = newCamY;
        }
    }


    mutex.lock();
        bool newLight = pd->newLight;
        bool updateLight = ( newLight != pd->light );
    mutex.unlock();

    if ( updateLight )
    {
        pd->powerTimeout.restart();
        if ( pd->setPower( true ) )
        {
            io.setLed( newLight );
            pd->light = newLight;
        }
    }

    if ( pd->powerTimeout.elapsed() > 10000 )
    {
        pd->setPower( false );
    }
}





