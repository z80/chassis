/**
  @file
  @author Stefan Frings
*/

#include <logging/filelogger.h>
#include "requesthandler.h"

#include <opencv2/opencv.hpp>

cv::VideoCapture webcam;

/** Logger class */
extern FileLogger* logger;

RequestHandler::RequestHandler(QObject* parent)
    :HttpRequestHandler(parent) {
    webcam.open( 0 );
}

RequestHandler::~RequestHandler()
{
    webcam.release();
}

void RequestHandler::service(HttpRequest& request, HttpResponse& response) {
    QByteArray path=request.getPath();
    qDebug("Conroller: path=%s",path.data());

    cv::Mat m;
    webcam >> m;

    if ( path == "/" )
        serviceRoot( request, response );
    else if ( path == "/video.asx" )
        serviceStream( request, response );
}

void RequestHandler::serviceRoot( HttpRequest& request, HttpResponse& response )
{
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");


    QString page = QString(     "<!DOCTYPE html>\n"
                                "<html>\n"
                                "<title>VLC Mozilla plugin test page</title>\n"
                                "<body>\n"
                                "<embed type=\"application/x-vlc-plugin\" pluginspage=\"http://www.videolan.org\" target=\"http://localhost:1234\"\n"
                                "width=\"640\"\n"
                                "height=\"480\"\n"
                                "id=\"vlc\" />\n"
                                "<script type=\"text/javascript\">\n" );

    //page = page + "var vlc = document.getElementById(\"vlc\");\n";
    //page = page + "var options = new Array(\":aspect-ratio=4:3\", \"--rtsp-tcp\");\n";
    //page = page + "var id = vlc.playlist.add(\"rtsp://localhost:1234\", \"fancy name\", options);\n";
    //page = page + "vlc.playlist.playItem(id);\n";

    page = page + "</script></body></html>\n";
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

