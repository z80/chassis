
#ifndef __SYS_TRAY_H_
#define __SYS_TRAY_H_

#include <QtGui>

class SysTray: public QSystemTrayIcon
{
    Q_OBJECT
public:
    SysTray( QObject * parent = 0 );
    ~SysTray();
private slots:
    void slotQuit();
private:
    class PD;
    PD * pd;
};


#endif



