
#include "root_handler.h"
#include "res_handler.h"

class RootHandler::PD
{
public:
    PD() {}
    ~PD() {}

    ResHandler * resHandler;
};

RootHandler::RootHandler( QObject * parent )
    : HttpRequestHandler( parent )
{
    pd = new PD();
    pd->resHandler = new ResHandler( this );
}

RootHandler::~RootHandler()
{
    pd->resHandler->deleteLater();
    delete pd;
}

void RootHandler::service( HttpRequest & request, HttpResponse & response )
{
    if ( pd->resHandler->service( request, response ) )
        return;

}

