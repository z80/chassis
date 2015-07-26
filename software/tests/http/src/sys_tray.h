
#ifndef __SYS_TRAY_H_
#define __SYS_TRAY_H_

#include <QtGui>

class SysTray: public QSystemTrayIcon
{
    Q_OBJECT
public:
    SysTray( QObject * parent = 0 );
    ~SysTray();

    bool runTor();
    void stopTor();
private slots:
    void slotSettings();
    void slotQuit();
private:
    class PD;
    PD * pd;
};


#endif



