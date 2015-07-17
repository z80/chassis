
#ifndef __ROOT_HANDLER_H_
#define __ROOT_HANDLER_H_

#include <httpserver/httprequesthandler.h>

class RootHandler: public HttpRequestHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(RootHandler)
public:

    RootHandler(QObject* parent=0);
    ~RootHandler();

    void service(HttpRequest& request, HttpResponse& response);

private:
    class PD;
    PD * pd;
};

#endif

