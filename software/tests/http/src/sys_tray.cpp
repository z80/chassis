
#include "sys_tray.h"

#include <httpserver/httplistener.h>
#include "root_handler.h"
#include "settings.h"

class SysTray::PD
{
public:
    PD() {}
    ~PD() {}

    QPointer<HttpListener> listener;
    QPointer<RootHandler> handler;
    QMenu        * menu;
    QAction      * actionQuit;
    QAction      * actionSettings;

    static const QString configFile;
    static const QString settingsFile;
};

const QString SysTray::PD::configFile = "config.ini";
const QString SysTray::PD::settingsFile = "settings.ini";


SysTray::SysTray( QObject * parent )
    : QSystemTrayIcon( parent )
{
    // Check for config file.
    QString fname = QString( "./%1" ).arg( PD::configFile );
    QFile file( fname );
    // If it doesn't exist copy from resource.
    if ( !file.exists() )
    {
        QFile resConfig( QString(":res/%1").arg( PD::configFile ) );
        bool res = resConfig.copy( fname );
        qWarning() << QString( "Copying result is %1" ).arg( res ? "true" : "false" );
    }

    pd = new PD();
    pd->handler = new RootHandler( this );

    QSettings * s = new QSettings( fname, QSettings::IniFormat, this );
    s->beginGroup( "listener" );
    pd->listener = new HttpListener( s, pd->handler, this );

    pd->menu = new QMenu( 0 );

    pd->actionSettings = new QAction( this );
    pd->actionSettings->setText( "settings" );
    pd->actionSettings->setIcon( QPixmap( ":res/icon-dark.png" ) );
    pd->menu->addAction( pd->actionSettings );

    pd->actionQuit = new QAction( this );
    pd->actionQuit->setText( "quit" );
    pd->menu->addAction( pd->actionQuit );

    this->setContextMenu( pd->menu );

    connect( pd->actionSettings, SIGNAL(triggered()), this, SLOT(slotSettings()) );
    connect( pd->actionQuit,     SIGNAL(triggered()), this, SLOT(slotQuit()) );

    this->setIcon( QIcon( ":res/icon-dark.png" ) );
}

SysTray::~SysTray()
{
    delete pd;
    //pd->listener->deleteLater();
    //pd->handler->deleteLater();
}

void SysTray::slotSettings()
{
    QSettings s( PD::settingsFile, QSettings::IniFormat, this );
    Settings dlg( 0 );
    dlg.setPort( s.value( "port", 8080 ).toInt() );
    dlg.setTor( s.value( "tor", false ).toBool() );
    dlg.setTorAddress( s.value( "addr", "" ).toString() );
    int res = dlg.exec();
    if ( res )
    {
        s.setValue( "port", dlg.port() );
        s.setValue( "tor", dlg.tor() );
    }
}

void SysTray::slotQuit()
{
    pd->listener->close();
    this->deleteLater();
    qApp->quit();
}





