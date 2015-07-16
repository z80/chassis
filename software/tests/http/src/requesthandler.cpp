/**
  @file
  @author Stefan Frings
*/

#include <logging/filelogger.h>
#include "requesthandler.h"

/** Logger class */
extern FileLogger* logger;

RequestHandler::RequestHandler(QObject* parent)
    :HttpRequestHandler(parent) {}

void RequestHandler::service(HttpRequest& request, HttpResponse& response) {
    QByteArray path=request.getPath();
    qDebug("Conroller: path=%s",path.data());

    if ( path == "/" )
        serviceRoot( request, response );
    else if ( path == "/video.asx" )
        serviceStream( request, response );
}

void RequestHandler::serviceRoot( HttpRequest& request, HttpResponse& response )
{
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");


    QString page = QString( "<html><body>Hello World!!!<br> <embed src=\"http://localhost:8080/video.asx\" height=\"450\" width=\"512\"> <br>Good-bye World!</body></html>" );
    // Return a simple HTML document
    //response.write("<html><body>Hello World</body></html>",true);
    response.write( page.toAscii(), true );

    qDebug("Conroller: finished request");
    logger->clear();

}

void RequestHandler::serviceStream( HttpRequest& request, HttpResponse& response )
{
    QString stream = "<ASX version =\"3.0\">"
                     "<TITLE>Stream1234</TITLE>"
                     "<ENTRY>"
                     "<REF HREF=\"http://localhost:1234\" />"
                     "</ENTRY>"
                     "</ASX>";

    response.write( stream.toAscii(), true );

    qDebug("Conroller: finished request");
    logger->clear();
}

