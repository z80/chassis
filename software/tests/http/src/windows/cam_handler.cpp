
#include "cam_handler.h"
#include "videoInput.h"

#include <QtCore>
#include <QImage>
#include <QPixmap>

class CamHandler::PD
{
public:

    QStringList devices();
    bool        setDevice( int index );
    const QImage & image();

    videoInput vi;
    int dev;
    int quality;
    QImage           imageRaw;
    QByteArray       data;
    QMutex mutex;
    QTime time;

    PD()
    {
        dev = -1;
        quality = 70;
        time.start();
    }

    ~PD()
    {
    }

private:
    bool updateDevice();
};

QStringList CamHandler::PD::devices()
{
    QMutexLocker lock( &mutex );

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
    QMutexLocker lock( &mutex );

    dev = index;
    bool res = updateDevice();
    return res;
}

class Sleep: public QThread
{
public:
    static void msleep( int ms )
    {
        QThread::msleep( ms );
    }
};

const QImage & CamHandler::PD::image()
{
    QMutexLocker lock( &mutex );

    int elapsed = time.elapsed();
    if ( elapsed > 50 )
    {
        time.restart();
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
                imageRaw = img.scaled( w+1, h+1 );
            }
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
    pd->mutex.lock();
    if ( pd->dev >= 0 )
        pd->vi.stopDevice( pd->dev );
    pd->mutex.unlock();

    delete pd;
}

bool CamHandler::service( HttpRequest& request, HttpResponse& response )
{
    if ( request.getPath() != "/jpg-image" )
        return false;

    QString data = QString::fromAscii( request.getBody() );
    QRegExp re( "(\\w{1,})\\s{0,}=\\s{0,}(-{0,1}\\d{1,})" );
    int pos = 0;
    while ( ( pos = re.indexIn( data, pos ) ) != -1 )
    {
        QString key   = re.cap( 1 );
        QString value = re.cap( 2 );
        pos += re.matchedLength();
        if ( key == "quality" )
        {
            pd->quality = value.toInt();
        }
    }
    
    QImage image = pd->image();

    QBuffer buffer( &pd->data );
    QSize sz = image.size();
    buffer.open(QIODevice::WriteOnly);
    bool res = image.save( &buffer, "JPG", pd->quality );

    response.setHeader("Content-Type", "image/JPG");
    response.write( pd->data, true );

    Sleep::msleep( 100 );

    return true;
}


