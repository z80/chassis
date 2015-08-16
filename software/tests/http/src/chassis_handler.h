
#ifndef __CHASSIS_HANDLER_H_
#define __CHASSIS_HANDLER_H_

#include <QtCore>
#include "httpserver/httprequesthandler.h"

class ChassisHandler: public QObject
{
    Q_OBJECT
public:
    ChassisHandler( QObject * parent = 0 );
    ~ChassisHandler();

    bool service( HttpRequest& request, HttpResponse& response );

private slots:
    void slotTimeout();
private:
    class PD;
    PD * pd;
};



#endif




