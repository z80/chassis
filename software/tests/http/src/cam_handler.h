
#ifndef __CAM_HANDLER_H_
#define __CAM_HANDLER_H_

#include <QtCore>
#include "httpserver/httprequesthandler.h"

class CamHandler: public QObject
{
    Q_OBJECT
public:
    CamHandler( QObject * parent = 0 );
    ~CamHandler();

    bool service( HttpRequest& request, HttpResponse& response );

private:
    class PD;
    PD * pd;
};

#endif
