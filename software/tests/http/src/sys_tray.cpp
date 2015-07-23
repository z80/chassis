
#include "sys_tray.h"

#include <httpserver/httplistener.h>
#include "root_handler.h"
#include "settings.h"
#include "process_tor.h"

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

    ProcessTor * tor;

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
    pd->tor = new ProcessTor( this, true );

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

    QSettings sss( PD::settingsFile, QSettings::IniFormat, this );
    bool startTor = sss.value( "tor", false ).toBool();
    if ( startTor )
        pd->tor->start();
}

SysTray::~SysTray()
{
    pd->tor->deleteLater();
    delete pd;
    //pd->listener->deleteLater();
    //pd->handler->deleteLater();
}

bool SysTray::runTor()
{
    bool res = pd->tor->start();
    return res;
}

void SysTray::stopTor()
{
    pd->tor->stop();
}

void SysTray::slotSettings()
{
    QSettings s( PD::settingsFile, QSettings::IniFormat, this );
    Settings dlg( 0 );
    dlg.setPort( s.value( "port", 8080 ).toInt() );
    dlg.setTor( s.value( "tor", false ).toBool() );
    QString torAddr = ProcessTor::hiddenAddr();
    dlg.setTorAddress( torAddr );
    int res = dlg.exec();
    if ( res == QDialog::Accepted )
    {
        s.setValue( "port", dlg.port() );
        bool tor = dlg.tor();
        s.setValue( "tor", tor );
        if ( tor )
        {
            if ( !pd->tor->isRunning() )
                pd->tor->start();
        }
        else
        {
            if ( pd->tor->isRunning() )
            {
                pd->tor->stop();
            }
        }
    }
}

void SysTray::slotQuit()
{
    pd->listener->close();
    this->deleteLater();
    qApp->quit();
}





