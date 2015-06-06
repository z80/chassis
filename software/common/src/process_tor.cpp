
#include "process_tor.h"

class ProcessTor::PD
{
public:
    PD() {}
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
    if ( !dir.mkdir( "./tor-cfg" ) )
        return false;
    QFile f( ":/cached-certs" );
    if ( !f.copy( "./tor-cfg/cached-certs" ) )
        return false;

    f.setFileName( ":/cached-micro~sc-consensus" );
    if ( !f.copy( "./tor-cfg/cached-micro~sc-consensus" ) )
        return false;

    f.setFileName( ":/cached-microdescs" );
    if ( !f.copy( "./tor-cfg/cached-microdescs" ) )
        return false;

    f.setFileName( ":/cached-microdescs.new" );
    if ( !f.copy( "./tor-cfg/cached-microdescs.new" ) )
        return false;

    f.setFileName( ":/control_auth_cookie" );
    if ( !f.copy( "./tor-cfg/control_auth_cookie" ) )
        return false;

    f.setFileName( ":/geoip" );
    if ( !f.copy( "./tor-cfg/geoip" ) )
        return false;

    f.setFileName( ":/geoip6" );
    if ( !f.copy( "./tor-cfg/geoip6" ) )
        return false;

    f.setFileName( ":/lock" );
    if ( !f.copy( "./tor-cfg/lock" ) )
        return false;

    f.setFileName( ":/state" );
    if ( !f.copy( "./tor-cfg/state" ) )
        return false;

    f.setFileName( ":/torrc.template" );
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

