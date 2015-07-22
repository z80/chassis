
#ifndef __CAPTURE_H_
#define __CAPTURE_H_

#include <QtGui>

class Capture: public QTimer
{
    Q_OBJECT
public:
    enum TInput { TDefault, TUsb, TSVideo, TComposite };

    Capture( QObject * parent = 0 );
    virtual ~Capture();

    QStringList devices();
    bool setFramerate( qreal fps );
    bool setResolution( const QSize & sz );
    bool setDevice( int index );
    bool start();
    bool stop();
    bool settingsWindow();
    void setInput( TInput input );
    QSize resolution();
    
    QStringList filters();
    void addFilter( const QString & stri );
    void removeFilter( const QString & stri );
    void clearFilters();

    void setBrightnessRange( const QPointF & range );
signals:
    void frame( const QImage &  );
private slots:
    void slotTimeout();
private:
    bool updateDevice();
    class PD;
    PD * pd;
};





#endif

