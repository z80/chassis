
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
    static const QString TOR_CONFIG_CLIENT;
    static const QString TOR_CONFIG_SERVER;
    static const QString TOR_ARGS;
};

const QString ProcessTor::PD::TOR_EXEC = "./tor";
const QString ProcessTor::PD::TOR_CONFIG_CLIENT = "./tor-cfg/torrc.client";
const QString ProcessTor::PD::TOR_CONFIG_SERVER = "./tor-cfg/torrc.server";
const QString ProcessTor::PD::TOR_ARGS = "./tor-cfg/torrc";

ProcessTor::ProcessTor( QObject * parent, bool host )
{
    pd = new PD();
    pd->ps = new QProcess( this );
    connect( pd->ps, SIGNAL(readyRead()), this, SLOT(slotReadyRead()) );


    if ( ( !( (host) && ( QFile::exists( PD::TOR_CONFIG_SERVER ) ) ) ) ||
         ( !( (!host) && ( QFile::exists( PD::TOR_CONFIG_CLIENT ) ) ) ) )
        createConfig();

    QString args = QString( "%1%2" ).arg( PD::TOR_ARGS ).arg( host ? ".serv" : ".client" );
    qDebug() << args;
    pd->ps->start( PD::TOR_EXEC, QStringList() << "-f" << args );

    bool res = pd->ps->waitForStarted();

    qDebug() << "run res: " << (res ? "true" : "false");

    res = (pd->ps->state() == QProcess::Running);

    qDebug() << "running: " << (res ? "true" : "false");
}

bool ProcessTor::createConfig()
{
    QDir dir;
    if ( ( !dir.exists( "./tor-cfg" ) ) && ( !dir.mkdir( "./tor-cfg" ) ) )
        return false;

    QStringList l;
    l << "cached-certs";
    l << "cached-microdesc-consensus";
    l << "cached-microdescs";
    l << "cached-microdescs.new";
    l << "control_auth_cookie";
    l << "geoip";
    l << "geoip6";
    l << "state";

    foreach( QString stri, l )
    {
        QString src  = QString( ":/files/%1" ).arg( stri );
        QString dest = QString( "./tor-cfg/%1" ).arg( stri );
        QFile f( src );
        f.copy( dest );
    }

    QFile fin( ":/files/torrc.template" );
    fin.copy( "./tor-cfg/torrc.client" );
    if ( !fin.open( QIODevice::ReadOnly ) )
        return false;
    QTextStream in( &fin );

    QFile fout( "./tor-cfg/torrc.serv" );
    if ( !fout.open( QIODevice::WriteOnly ) )
        return false;
    QTextStream out( &fout );

    while ( !in.atEnd() )
    {
        QString torrc_stri = in.readLine();

        QRegExp rx( "{port}" );
        rx.setPatternSyntax( QRegExp::FixedString );
        if ( rx.indexIn( torrc_stri ) >= 0 )
        {
            torrc_stri = torrc_stri.replace( rx, "8080" );
            torrc_stri = torrc_stri.replace( "#", "" );
        }

        rx.setPattern( "{host}" );
        if ( rx.indexIn( torrc_stri ) >= 0 )
        {
            torrc_stri = torrc_stri.replace( rx, "localhost" );
            torrc_stri = torrc_stri.replace( "#", "" );
        }

        rx.setPattern( "{servicedir}" );
        if ( rx.indexIn( torrc_stri ) >= 0 )
        {
            torrc_stri = torrc_stri.replace( rx, "./tor-service" );
            torrc_stri = torrc_stri.replace( "#", "" );
        }
        out << torrc_stri << "\r\n";
    }
    out.flush();
    fout.close();
    fin.close();

    return true;
}

void ProcessTor::slotReadyRead()
{
    QByteArray data = pd->ps->readAll();
    QString stri = QString::fromAscii( data.data(), data.size() );
    qDebug() << stri;
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

