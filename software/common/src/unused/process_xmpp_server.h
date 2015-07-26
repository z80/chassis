
#ifndef __PROCESS_XMPP_SERVER_H_
#define __PROCESS_XMPP_SERVER_H_

#include <QtCore>

class ProcessXmppServer: public QObject
{
	Q_OBJECT
public:
	ProcessXmppServer( QObject * parent = 0 );
	~ProcessXmppServer();

private:
	class PD;
	PD * pd;
};

#endif


