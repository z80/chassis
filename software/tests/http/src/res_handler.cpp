
#include "res_handler.h"


class ResSingleton
{
public:
    ResSingleton()
    {
        Q_INIT_RESOURCE( res );
    }

    ~ResSingleton()
    {
    }
};

class ResHandler::PD
{
public:
    PD()
    {
    }

    ~PD()
    {
    }

    static void setContentType(QString fileName, HttpResponse & response);

    QCache< QString, const QByteArray > cache;
};

void ResHandler::PD::setContentType(QString fileName, HttpResponse& response)
{
    //encoding=settings->value("encoding","UTF-8").toString();
    const QString  encoding = "UTF-8";

    if (fileName.endsWith(".png")) {
        response.setHeader("Content-Type", "image/png");
    }
    else if (fileName.endsWith(".jpg")) {
        response.setHeader("Content-Type", "image/jpeg");
    }
    else if (fileName.endsWith(".gif")) {
        response.setHeader("Content-Type", "image/gif");
    }
    else if (fileName.endsWith(".pdf")) {
        response.setHeader("Content-Type", "application/pdf");
    }
    else if (fileName.endsWith(".txt")) {
        response.setHeader("Content-Type", qPrintable("text/plain; charset="+encoding));
    }
    else if (fileName.endsWith(".html") || fileName.endsWith(".htm")) {
        response.setHeader("Content-Type", qPrintable("text/html; charset="+encoding));
    }
    else if (fileName.endsWith(".css")) {
        response.setHeader("Content-Type", "text/css");
    }
    else if (fileName.endsWith(".js")) {
        response.setHeader("Content-Type", "text/javascript");
    }
    // Todo: add all of your content types
}



ResHandler::ResHandler( QObject * parent )
    : QObject( parent )
{
    static ResSingleton res;
    pd = new PD();
}

ResHandler::~ResHandler()
{
    delete pd;
}

bool ResHandler::service( HttpRequest& request, HttpResponse& response )
{
    QCache< QString, const QByteArray > & c = pd->cache;
    QString resName = QString( ":%1" ).arg( QString::fromAscii( request.getPath() ) );

    if ( c.contains( resName ) )
    {
        //response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
        const QByteArray & res = *c[resName];
        PD::setContentType( resName, response );
        response.write( res, true );
        return true;
    }

    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString fname = it.next();
        if ( fname == resName )
        {
            QByteArray * pa = new QByteArray();
            QByteArray & a = *pa;
            QFile f( fname );
            if ( !f.open( QIODevice::ReadOnly ) )
                return false;
            a = f.readAll();
            c.insert( fname, pa );

            PD::setContentType( resName, response );
            response.write( a, true );
            return true;
        }
    }
    return false;
}





