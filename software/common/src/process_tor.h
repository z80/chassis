

#ifndef __PROCESS_TOR_H_
#define __PROCESS_TOR_H_

#include <QtCore>

class ProcessTor: public QObject
{
	Q_OBJECT
public:
	ProcessTor( QObject * parent = 0, bool host = true );
	~ProcessTor();

    bool start();
    bool stop();
    bool isRunning() const;

    static QString hiddenAddr();
private:
    bool createConfig();
private slots:
    void slotReadyRead();

private:
	class PD;
	PD * pd;
};

#endif

