
#include "cam_handler.h"
#include "videoInput.h"

#include <QtCore>
#include <QImage>

class CamHandler::PD
{
public:

    QStringList devices();
    bool        setDevice( int index );
    const QImage & image();

    videoInput vi;
    int dev;
    QImage           imageRaw;
    QByteArray       data;

    PD()
    {
        dev = -1;
    }

    ~PD()
    {
    }

private:
    bool updateDevice();
};

QStringList CamHandler::PD::devices()
{
    int cnt = vi.listDevices();	
    QStringList l;
    l.reserve( cnt );
    for ( int i=0; i<cnt; i++ )
    {
        QString stri = vi.getDeviceName( i );
        l << stri;
    }
    return l;
}

bool CamHandler::PD::setDevice( int index )
{
    dev = index;
    bool res = updateDevice();
    return res;
}

const QImage & CamHandler::PD::image()
{
	int size = vi.getSize( dev );
    if ( size > 0 )
    {
	    int w = vi.getWidth( dev );
	    int h = vi.getHeight( dev );
    	
	    if ( data.size() != size )
            data.resize( size );
    	
	    //to get the data from the device first check if the data is new
	    if ( vi.isFrameNew( dev ) )
        {
		    vi.getPixels( dev, reinterpret_cast<unsigned char *>( data.data() ), true, true );	//fills pixels as a BGR (for openCV) unsigned char array - no flipping

            QImage img( reinterpret_cast<unsigned char *>( data.data() ), w, h, QImage::Format_RGB888 );
            imageRaw = img;
        }
    }
    return imageRaw;
}

bool CamHandler::PD::updateDevice()
{
    bool res = vi.setupDevice( dev );
    return res;
}


CamHandler::CamHandler( QObject * parent )
    : QObject( parent )
{
    CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
    pd = new PD();
    QStringList list = pd->devices();
    if ( list.size() > 0 )
        pd->setDevice( 0 );
}

CamHandler::~CamHandler()
{
    if ( pd->dev >= 0 )
        pd->vi.stopDevice( pd->dev );
}

bool CamHandler::service( HttpRequest& request, HttpResponse& response )
{
    if ( request.getPath() != "/jpg-image" )
        return false;
    
    QImage image = pd->image();

    QBuffer buffer( &pd->data );
    QSize sz = image.size();
    buffer.open(QIODevice::WriteOnly);
    bool res = image.save( &buffer, "JPG", 70 );

    response.setHeader("Content-Type", "image/JPG");
    response.write( pd->data, true );

    return true;
}


