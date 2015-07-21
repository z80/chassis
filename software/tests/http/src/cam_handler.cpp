
#include "cam_handler.h"
#include "opencv2/opencv.hpp"
#include <QtCore>
#include <QImage>

class CamHandler::PD
{
public:
    PD() {}
    ~PD() {}

    cv::VideoCapture webcam;
    cv::Mat          frame;
    QImage           imageRaw, image;
    QByteArray       data;
};

CamHandler::CamHandler( QObject * parent )
    : QObject( parent )
{
    pd = new PD();
    pd->webcam.open( 0 );
}

CamHandler::~CamHandler()
{
    pd->webcam.release();
}

bool CamHandler::service( HttpRequest& request, HttpResponse& response )
{
    if ( request.getPath() != "/jpg-image" )
        return false;
    cv::VideoCapture & webcam = pd->webcam;
    cv::Mat & frame           = pd->frame;
    QImage & imageRaw         = pd->imageRaw;
    //QImage & image            = pd->image;
    webcam >> frame;

    imageRaw = QImage( (const uchar *)frame.data, frame.cols, frame.rows, QImage::Format_RGB888 );

    // OpenCV swaps Red and Blue components.
    QImage image = imageRaw.rgbSwapped();

    QBuffer buffer( &pd->data );
    buffer.open(QIODevice::WriteOnly);
    image.save( &buffer, "JPG", 70 );

    response.setHeader("Content-Type", "image/jpg");
    response.write( pd->data, true );

    return true;
}


