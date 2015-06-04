
class ProcessXmppServer::PD
{
public:
	QProcess * ps;
	static const QString SERV_FILE;
	static const QString SERV_ARGS;
};

const QString ProcessXmppServer::PD::SERV_FILE = "./bin/xmpp_server";
const QString ProcessXmppServer::PD::SERV_ARGS = "";

ProcessXmppServer::ProcessXmppServer( QObject * parent )
: QObject( parent )
{
	pd = new PD();
	pd->ps = new QProcess( this );
	pd->ps->start( PD::SERV_FILE, QStringList() << PD::SERV_ARGS );
}

ProcessXmppServer::~ProcessXmppServer()
{
	pd->ps->close();
	pd->ps->waitForFinished();
	pd->ps->deleteLater();
	delete pd;
}


