
#ifndef __PROCESS_TOR_H_
#define __PROCESS_TOR_H_

#include <QtCore>

class ProcessTor: public QObject
{
	Q_OBJECT
public:
	ProcessTor( QObject * parent = 0, bool host = true );
	~ProcessTor();

    QString hiddenAddr() const;
private:
    bool createConfig();

	class PD;
	PD * pd;
};

#endif

