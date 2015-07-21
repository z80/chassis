
#include "sys_tray.h"

#include <httpserver/httplistener.h>
#include "root_handler.h"

class SysTray::PD
{
public:
    PD() {}
    ~PD() {}

    QPointer<HttpListener> listener;
    QPointer<RootHandler> handler;
    QMenu        * menu;
    QAction      * actionQuit;

    static const QString configFile;
};

const QString SysTray::PD::configFile = "config.ini";


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
    pd->actionQuit = new QAction( this );
    pd->actionQuit->setText( "quit" );
    pd->menu->addAction( pd->actionQuit );

    this->setContextMenu( pd->menu );

    connect( pd->actionQuit, SIGNAL(triggered()), this, SLOT(slotQuit()) );

    this->setIcon( QIcon( ":res/icon-dark.png" ) );
}

SysTray::~SysTray()
{
    delete pd;
    //pd->listener->deleteLater();
    //pd->handler->deleteLater();
}

void SysTray::slotQuit()
{
    pd->listener->close();
    this->deleteLater();
    qApp->quit();
}





