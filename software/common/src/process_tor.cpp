
#include "process_tor.h"

class CInitResource
{
public:
    CInitResource()
    {
        Q_INIT_RESOURCE( files );
    }

    ~CInitResource()
    {
        Q_CLEANUP_RESOURCE( files );
    }
};


class ProcessTor::PD
{
public:
    PD()
    {
    	static CInitResource a;
    }
    ~PD() {}
    QProcess * ps;
    static const QString TOR_EXEC;
    static const QString TOR_CONFIG;
    static const QString TOR_ARGS;
};

const QString ProcessTor::PD::TOR_EXEC = "./tor";
const QString ProcessTor::PD::TOR_CONFIG = "./tor-cfg/torrc";
const QString ProcessTor::PD::TOR_ARGS = "-f ./tor-cfg/torrc";

ProcessTor::ProcessTor( QObject * parent, bool host )
{
    pd = new PD();
    pd->ps = new QProcess( this );

    if ( !QFile::exists( PD::TOR_CONFIG ) )
        createConfig();

    pd->ps->start( PD::TOR_EXEC, QStringList() << PD::TOR_ARGS );

}

bool ProcessTor::createConfig()
{
    QDir dir;
    if ( ( !dir.mkdir( "./tor-cfg" ) ) && ( !dir.exists( "./tor-cfg" ) ) )
        return false;
    QFile f( ":/files/cached-certs" );
    if ( !f.open( QIODevice::ReadOnly ) )
    	return false;
    if ( !f.copy( "./tor-cfg/cached-certs" ) )
        return false;
    f.close();

    f.setFileName( ":/files/cached-micro~sc-consensus" );
    if ( !f.open( QIODevice::ReadOnly ) )
    	return false;
    if ( !f.copy( "./tor-cfg/cached-micro~sc-consensus" ) )
        return false;
    f.close();

    f.setFileName( ":/files/cached-microdescs" );
    if ( !f.open( QIODevice::ReadOnly ) )
    	return false;
    if ( !f.copy( "./tor-cfg/cached-microdescs" ) )
        return false;
    f.close();

    f.setFileName( ":/files/cached-microdescs.new" );
    if ( !f.open( QIODevice::ReadOnly ) )
    	return false;
    if ( !f.copy( "./tor-cfg/cached-microdescs.new" ) )
        return false;
    f.close();

    f.setFileName( ":/files/control_auth_cookie" );
    if ( !f.open( QIODevice::ReadOnly ) )
    	return false;
    if ( !f.copy( "./tor-cfg/control_auth_cookie" ) )
        return false;
    f.close();

    f.setFileName( ":/files/geoip" );
    if ( !f.open( QIODevice::ReadOnly ) )
    	return false;
    if ( !f.copy( "./tor-cfg/geoip" ) )
        return false;
    f.close();

    f.setFileName( ":/files/geoip6" );
    if ( !f.open( QIODevice::ReadOnly ) )
    	return false;
    if ( !f.copy( "./tor-cfg/geoip6" ) )
        return false;
    f.close();

    f.setFileName( ":/files/lock" );
    if ( !f.open( QIODevice::ReadOnly ) )
    	return false;
    if ( !f.copy( "./tor-cfg/lock" ) )
        return false;
    f.close();

    f.setFileName( ":/files/state" );
    if ( !f.open( QIODevice::ReadOnly ) )
    	return false;
    if ( !f.copy( "./tor-cfg/state" ) )
        return false;
    f.close();

    f.setFileName( ":/files/torrc.template" );
    if ( !f.open( QIODevice::ReadOnly ) )
        return false;
    QByteArray torrc = f.readAll();
    QString torrc_stri = QString::fromAscii( torrc );
    QRegExp rx( "{port}" );
    torrc_stri = torrc_stri.replace( rx, "7100" );
    rx.setPattern( "{host}" );
    torrc_stri = torrc_stri.replace( rx, "localhost" );
    rx.setPattern( "{servicedir}" );
    torrc_stri = torrc_stri.replace( rx, "./tor-service" );

    f.setFileName( "./tor-cfg/torrc" );
    if ( !f.open( QIODevice::WriteOnly ) )
        f.write( torrc_stri.toAscii() );
    f.close();

    return true;
}

ProcessTor::~ProcessTor()
{
    pd->ps->close();
    pd->ps->waitForFinished();
    pd->ps->deleteLater();
    delete pd;
}

QString ProcessTor::hiddenAddr() const
{
    return "QString()";
}

