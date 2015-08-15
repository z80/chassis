
#include "root_handler.h"
#include "res_handler.h"
#include "cam_handler.h"
#include "chassis_handler.h"

class RootHandler::PD
{
public:
    PD() {}
    ~PD() {}

    ResHandler * resHandler;
    CamHandler * camHandler;
    ChassisHandler * chHandler;
};

RootHandler::RootHandler( QObject * parent )
    : HttpRequestHandler( parent )
{
    pd = new PD();
    pd->resHandler = new ResHandler( this );
    pd->camHandler = new CamHandler( this );
    pd->chHandler  = new ChassisHandler( this );
}

RootHandler::~RootHandler()
{
    pd->resHandler->deleteLater();
    pd->camHandler->deleteLater();
    pd->chHandler->deleteLater();
    delete pd;
}

void RootHandler::service( HttpRequest & request, HttpResponse & response )
{
    QString path = request.getPath();
    if ( pd->camHandler->service( request, response ) )
        return;
    if ( pd->chHandler->service( request, response ) )
        return;
    if ( pd->resHandler->service( request, response ) )
        return;
    if ( path == "/" )
    {
        bool res = pd->resHandler->service( request, response, "index.html" );
        return;
    }
}

