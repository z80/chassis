
#include "settings.h"

Settings::Settings( QWidget * parent )
: QDialog( parent )
{
    ui.setupUi( this );
}

Settings::~Settings()
{
}

void Settings::setPort( int v )
{
    ui.port->setText( QString( "%1" ).arg( v ) );
}

int Settings::port() const
{
    QString p = ui.port->text();
    int v = p.toInt();
    return v;
}

void Settings::setTor( bool en )
{
    ui.runTor->setChecked( en );
}

bool Settings::tor() const
{
    bool res = ui.runTor->isChecked();
    return res;
}

void Settings::setTorAddress( const QString & addr )
{
    ui.torAddress->setText( addr );
}


