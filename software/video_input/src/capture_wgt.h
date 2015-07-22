
#ifndef __CAPTURE_WGT_H_
#define __CAPTURE_WGT_H_

#include <QtGui>
#include "capture.h"

class CaptureWgt: public QMainWindow
{
    Q_OBJECT
public:
    CaptureWgt( QWidget * parent = 0 );
    virtual ~CaptureWgt();
    
    bool eventFilter( QObject * o, QEvent * e );
    void setDrawArrow( bool en );
    void setArrow( const QPointF & from, const QPointF & to );
    void setLightConfig( const QString & fname );
    void setCameraIndex( int index );
    void setVideoFlip( bool x, bool y );
    void setLaserConfig( const QString & fname );
signals:
    void mouseEvent( const QPointF & at );
protected:
    void resizeEvent( QResizeEvent * e );
    void showEvent( QShowEvent * w );
public slots:
    void slotCapture();
    void slotDevice();
    void slotInput();
    void slotResolution();
    void slotSettings();
    void slotFilter();
    void slotPixmap();
    void slotFrame( const QImage & img );
    void slotBrightness();
    void slotBrightnessChanged( const QPointF & range );
    void slotFlipX();
    void slotFlipY();
    // Laser spot configuration.
    void slotSetLaserSpot();
private slots:
    void slotCrossEmphasis();
protected:
    void emitMouseEvent();
public:
    QString savePixmap( const QString & stri );
    void image( QImage & img );
    bool isMarker() const;
    const QPointF & marker() const;
private:
    void listDevices();
    void listResolutions();
    void updatePixmap( const QImage & img = QImage() );
    void drawArrow();
    void drawLaserPos();
    void updateCrossPos( bool write = false );
    
    class PD;
    friend class PD;
    PD * pd;
};


#endif



