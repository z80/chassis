
#ifndef __SETTINGS_H_
#define __SETTINGS_H_

#include <QtGui>
#include "ui_settings.h"

class Settings: public QDialog
{
    Q_OBJECT
public:
    Settings( QWidget * parent = 0 );
    ~Settings();

    void setPort( int v );
    int port() const;

    void setTor( bool en );
    bool tor() const;

    void setTorAddress( const QString & addr );
private:
    Ui_Settings ui;
};



#endif



