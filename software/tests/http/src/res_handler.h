
#ifndef __RES_HANDLER_H_
#define __RES_HANDLER_H_

#include <QtCore>
#include "httpserver/httprequesthandler.h"

class ResHandler : public QObject
{
    Q_OBJECT
public:
    ResHandler( QObject * parent = 0 );
    ~ResHandler();

    bool service( HttpRequest& request, HttpResponse& response, const QString & override = "" );

private:
    class PD;
    PD * pd;
};

#endif

