
#include "capture_wgt.h"
#include "ui_capture_wgt.h"
#include "capture.h"
#include "brightness_wgt.h"
//#include "tray_icon.h"

class CInitResource
{
public:
    CInitResource()
    {
        Q_INIT_RESOURCE( images );
    }

    ~CInitResource()
    {
        Q_CLEANUP_RESOURCE( images );
    }
};

class CaptureWgt::PD
{
public:
    PD()
    {
        static CInitResource resource;
    }

    ~PD() {}

    Ui_CaptureWgt ui;
    QImage        img;
    Capture       * cap;
    BrightnessWgt * brightness;

    QPointF imgAt;
    QPointF imgPos;
    QPointF markerPos;
    qreal   imgScale;
    QPointF arrowFrom, 
            arrowTo;
    bool drawArrow, 
         flipX, 
         flipY, 
         resolutionAcquired;
    QSize imgSize;

    QMenu * devMenu;
    QList<QAction *> devList;

    QMenu * inMenu;
    QList<QAction *> inList;

    QMenu * resMenu;
    QList<QAction *> resList;

    QList<QAction *> filterList;

    QGraphicsScene      * scene;
    QGraphicsPixmapItem * image, 
                        * laserPosImage;
    QGraphicsLineItem   * lineX1, 
                        * lineY1, 
                        * lineX2, 
                        * lineY2;
    QGraphicsPolygonItem * arrow;

    //LightIcon * usbLight;

    // Feature emphasizing cross.
    QString               crossFname;

    // Modifying laser position.
    bool                  modifyLaserPos, 
                          drawLaserPos;
    QPointF               laserSpotPos;

    void crossPos( CaptureWgt * owner, const QPointF & at );
    void laserPos( CaptureWgt * owner, const QPointF & at );
};

void CaptureWgt::PD::crossPos( CaptureWgt * owner, const QPointF & at )
{
    QPointF imgAt = ui.view->mapToScene( at.x(), at.y() );
    qreal xx = ( imgAt.x() - imgPos.x() ) / imgScale;
    qreal yy = ( imgAt.y() - imgPos.y() ) / imgScale;
    this->imgAt = QPointF( xx, yy );
    if ( ui.cross->isChecked() )
        markerPos = QPointF( xx, yy );
    owner->emitMouseEvent();
}

void CaptureWgt::PD::laserPos( CaptureWgt * owner, const QPointF & at )
{
    if ( modifyLaserPos )
    {
        modifyLaserPos = false;
        //imgPos;
        //imgScale = sc;
        QSizeF imgSz = img.size();
        QPointF imgAt = ui.view->mapToScene( at.x(), at.y() );
        qreal xx = ( imgAt.x() - imgPos.x() );
        xx /= (imgSz.width() * imgScale);
        qreal yy = ( imgAt.y() - imgPos.y() );
        yy /= (imgSz.height() * imgScale);
        laserSpotPos = QPointF( xx, yy );
        owner->emitMouseEvent();
    }
}

CaptureWgt::CaptureWgt( QWidget * parent )
: QMainWindow( parent )
{
    pd = new PD();
    pd->ui.setupUi( this );
    pd->crossFname = "./video.ini";
    pd->modifyLaserPos = false;
    pd->devMenu = 0;
    pd->resMenu = 0;
    pd->imgScale = 1.0;
    pd->drawArrow = false;
    pd->flipX = false;
    pd->flipY = false;
    pd->resolutionAcquired = false;
    pd->cap = new Capture( this );
    pd->ui.brightnessDw->setVisible( false );
    pd->brightness = new BrightnessWgt( this );
    pd->ui.brightnessDw->setWidget( pd->brightness );
    pd->cap->setBrightnessRange( pd->brightness->range() );
    connect( pd->brightness, SIGNAL(rangeChanged(const QPointF &)), 
             this,           SLOT(slotBrightnessChanged(const QPointF &)) );
    connect( pd->ui.brightnessRange, SIGNAL(triggered()), 
             this,                   SLOT(slotBrightness()) );

    Ui_CaptureWgt & ui = pd->ui;
    connect( ui.capture,  SIGNAL(triggered()), this, SLOT(slotCapture()) );
    connect( ui.settings, SIGNAL(triggered()), this, SLOT(slotSettings()) );
    connect( ui.cross,    SIGNAL(triggered()), this, SLOT(slotCrossEmphasis()) );

    pd->scene = new QGraphicsScene( pd->ui.view );
    pd->scene->setBackgroundBrush( QBrush( Qt::gray ) );
    pd->ui.view->setScene( pd->scene );

    pd->image = new QGraphicsPixmapItem( 0, pd->scene );
    pd->image->setPos( 0.0, 0.0 );
    pd->lineX1 = new QGraphicsLineItem( pd->image, pd->scene );
    pd->lineY1 = new QGraphicsLineItem( pd->image, pd->scene );
    pd->lineX2 = new QGraphicsLineItem( pd->image, pd->scene );
    pd->lineY2 = new QGraphicsLineItem( pd->image, pd->scene );
    pd->arrow = new QGraphicsPolygonItem( pd->image, pd->scene );
    // This is for laser spot position.
    pd->laserPosImage = new QGraphicsPixmapItem( 0, pd->scene );
    pd->laserPosImage->setPixmap( QPixmap( ":/images/laser.png" ) );
    pd->laserPosImage->setPos( -pd->laserPosImage->pixmap().width()/2, -pd->laserPosImage->pixmap().height()/2 );
    pd->laserPosImage->setVisible( false );

    connect( pd->ui.flipX, SIGNAL(triggered()), this, SLOT(slotFlipX()) );
    connect( pd->ui.flipY, SIGNAL(triggered()), this, SLOT(slotFlipY()) );

    pd->cap = new Capture( this );
    listDevices();
    connect( pd->cap, SIGNAL(frame(const QImage &)), this, SLOT(slotFrame(const QImage &)) );

    ui.view->installEventFilter( this );

    pd->inMenu = new QMenu( this );
    ui.input->setMenu( pd->inMenu );
    QAction * a = new QAction( "Default", this );
    a->setCheckable( true );
    a->setChecked( true );
    pd->inMenu->addAction( a );
    pd->inList << a;

    a = new QAction( "Usb", this );
    a->setCheckable( true );
    pd->inMenu->addAction( a );
    pd->inList << a;

    a = new QAction( "S-Video", this );
    a->setCheckable( true );
    pd->inMenu->addAction( a );
    pd->inList << a;

    a = new QAction( "Composite", this );
    a->setCheckable( true );
    pd->inMenu->addAction( a );
    pd->inList << a;

    for ( int i=0; i<pd->inList.size(); i++ )
    {
        QAction * a = pd->inList[i];
        connect( a, SIGNAL(triggered()), this, SLOT(slotInput()) );
    }

    // ƒействи€ соответствующие фильтрам.
    pd->filterList << ui.equalizeHist;
    pd->filterList << ui.fullContrast;
    pd->filterList << ui.sobel;
    pd->filterList << ui.median;
    pd->filterList << ui.timeSmooth;
    pd->filterList << ui.highPass;
    pd->filterList << ui.surface;
    pd->filterList << ui.brightnessRange;
    for ( int i=0; i<pd->filterList.size(); i++ )
    {
        QAction * a = pd->filterList[i];
        connect( a, SIGNAL(triggered()), this, SLOT(slotFilter()) );
    }

    connect( ui.pixmap, SIGNAL(triggered()), this, SLOT(slotPixmap()) );


	QToolButton * btn = qobject_cast<QToolButton *>(ui.toolBar->widgetForAction(ui.input));
	if ( btn )
		btn->setPopupMode(QToolButton::InstantPopup);

	btn = qobject_cast<QToolButton *>(ui.toolBar->widgetForAction(ui.device));
	if ( btn )
		btn->setPopupMode(QToolButton::InstantPopup);
	
	btn = qobject_cast<QToolButton *>(ui.toolBar->widgetForAction(ui.resolution));
	if ( btn )
		btn->setPopupMode(QToolButton::InstantPopup);

    //pd->usbLight = new LightIcon( this );
    //ui.toolBar->addWidget( pd->usbLight );

    // Laser spot handling.
    QMenu * m = new QMenu( this );
    a = m->addAction( "Set laser spot position" );
    connect( a, SIGNAL(triggered()), this, SLOT(slotSetLaserSpot()) );
    btn = qobject_cast<QToolButton *>(ui.toolBar->widgetForAction(ui.laser));
    btn->setPopupMode( QToolButton::MenuButtonPopup );
    ui.laser->setMenu( m );

    updateCrossPos();
}

CaptureWgt::~CaptureWgt()
{
    pd->cap->stop();
    delete pd;
}

bool CaptureWgt::eventFilter( QObject * o, QEvent * e )
{
    if ( e->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent * m = reinterpret_cast<QMouseEvent *>( e );
        if ( pd->ui.cross->isChecked() )
        {
            pd->crossPos( this, m->posF() );
            // Save to file if checked.
            if ( pd->ui.cross->isChecked() )
                updateCrossPos( true );
        }
        if ( pd->modifyLaserPos )
        {
            pd->laserPos( this, m->posF() );
            // Always save it to a file.
            updateCrossPos( true );
        }
    }
    return false;
}

void CaptureWgt::setDrawArrow( bool en )
{
    pd->drawArrow = en;
}

void CaptureWgt::setArrow( const QPointF & from, const QPointF & to )
{
    pd->arrowFrom = from;
    pd->arrowTo   = to;
}

void CaptureWgt::setLightConfig( const QString & fname )
{
    //pd->usbLight->setCfgFileName( fname );
}

void CaptureWgt::setCameraIndex( int index )
{
    pd->cap->setDevice( index );
}

void CaptureWgt::setVideoFlip( bool x, bool y )
{
    Ui_CaptureWgt & ui = pd->ui;
    ui.flipX->setChecked( x );
    slotFlipX();
    ui.flipY->setChecked( y );
    slotFlipY();
}

void CaptureWgt::setLaserConfig( const QString & fname )
{
    pd->crossFname = fname;
}

void CaptureWgt::resizeEvent( QResizeEvent * e )
{
    QSizeF sz = pd->ui.view->size();
    pd->ui.view->setSceneRect( 0.0, 0.0, sz.width() - 10, sz.height() - 10 );
    updatePixmap();
}

void CaptureWgt::showEvent( QShowEvent * w )
{
    resizeEvent( 0 );
}

void CaptureWgt::slotCapture()
{
    Ui_CaptureWgt & ui = pd->ui;
    bool en = ui.capture->isChecked();

	if ( en ) {
        en = pd->cap->start();
		if ( !en )
			ui.capture->setChecked(false);
	}
    else
        pd->cap->stop();

	pd->ui.device->setEnabled( !en );
    pd->ui.input->setEnabled( !en );
    pd->ui.settings->setEnabled( !en );
}

void CaptureWgt::slotDevice()
{
    QObject * o = sender();
    QAction * a = qobject_cast<QAction *>( o );
    for ( int i=0; i<pd->devList.size(); i++ )
    {
        QAction * d = pd->devList[i];
        if ( a == d )
        {
            d->setChecked( true );
            pd->cap->setDevice( i );
        }
        else
            d->setChecked( false );
    }
    pd->resolutionAcquired = false;
}

void CaptureWgt::slotInput()
{
    QObject * o = sender();
    QAction * a = qobject_cast<QAction *>( o );
    for ( int i=0; i<pd->inList.size(); i++ )
    {
        QAction * d = pd->inList[i];
        if ( a == d )
        {
            d->setChecked( true );
            if ( a->text() == "Default" )
                pd->cap->setInput( Capture::TDefault );
            if ( a->text() == "Usb" )
                pd->cap->setInput( Capture::TUsb );
            else if ( a->text() == "S-Video" )
                pd->cap->setInput( Capture::TSVideo );
            else if ( a->text() == "Composite" )
                pd->cap->setInput( Capture::TComposite );
        }
        else
            d->setChecked( false );
    }
}

void CaptureWgt::slotResolution()
{
    QObject * o = sender();
    QAction * a = dynamic_cast<QAction *>( o );
    if ( !a )
        return;
    for ( int i=0; i<pd->resList.size(); i++ )
    {
        QAction * n = dynamic_cast<QAction *>( pd->resList[i] );
        if ( ( n ) && ( a == n ) )
            n->setChecked( true );
        else
            n->setChecked( false );
    }
    QString stri = a->text();
    QRegExp reg( "\\d{1,}\\s{0,}x" );
    QString sw( "640 x" );
    if ( reg.indexIn( stri ) != -1 )
        sw = reg.cap( 0 );
    QString sh( "x 480" );
    reg.setPattern( "x\\s{0,}\\d{1,}" );
    if ( reg.indexIn( stri ) != -1 )
        sh = reg.cap( 0 );
    int w = sw.left( sw.length() - 1 ).toInt();
    int h = sh.right( sh.length() - 1 ).toInt();
    pd->cap->setResolution( QSize( w, h ) );
}

void CaptureWgt::slotSettings()
{
    pd->cap->settingsWindow();
}

void CaptureWgt::slotFilter()
{
    QObject * o = sender();
    QAction * a = qobject_cast<QAction *>( o );
    QString name;
    if ( a == pd->ui.equalizeHist )
        name = "equalizeHist";
    else if ( a == pd->ui.fullContrast )
        name = "fullContrast";
    else if ( a == pd->ui.sobel )
        name = "sobel";
    else if ( a == pd->ui.median )
        name = "median";
    else if ( a == pd->ui.timeSmooth )
        name = "timeSmooth";
    else if ( a == pd->ui.highPass )
        name = "highPass";
    else if ( a == pd->ui.surface )
        name = "surfaceSubtract";
    else if ( a == pd->ui.brightnessRange )
        name = "brightnessRange";
    bool checked = a->isChecked();
    if ( checked )
        pd->cap->addFilter( name );
    else
        pd->cap->removeFilter( name );
}

void CaptureWgt::slotPixmap()
{
    QFileDialog d( this );
    d.setWindowTitle( "Save pixmap as" );
    d.setNameFilter( "PNG files (*.png)" );
    d.setFileMode( QFileDialog::AnyFile );
    d.setAcceptMode( QFileDialog::AcceptSave );
    d.setOptions( 0 );
    if ( d.exec() )
    {
        QStringList fileNames = d.selectedFiles();
        if ( fileNames.size() < 1 )
            return;
        QString fileName = fileNames.at( 0 );
        savePixmap( fileName );
    }
}

void CaptureWgt::slotFrame( const QImage & img )
{
    updatePixmap( img );
    listResolutions();
}

void CaptureWgt::slotBrightness()
{
    pd->ui.brightnessDw->setVisible( pd->ui.brightnessRange->isChecked() );
}

void CaptureWgt::slotBrightnessChanged( const QPointF & range )
{
    pd->cap->setBrightnessRange( range );
}

void CaptureWgt::slotFlipX()
{
    pd->flipX = pd->ui.flipX->isChecked();
}

void CaptureWgt::slotFlipY()
{
    pd->flipY = pd->ui.flipY->isChecked();
}

void CaptureWgt::slotCrossEmphasis()
{
    Ui_CaptureWgt & ui = pd->ui;
    updateCrossPos( !ui.cross->isChecked() );
}

void CaptureWgt::slotSetLaserSpot()
{
    QAction * a = qobject_cast<QAction *>( sender() );
    if ( a )
    {
        // And when it is set it becomes false.
        pd->modifyLaserPos = true;
        pd->ui.laser->setChecked( true );
    }
}


void CaptureWgt::emitMouseEvent()
{
    emit mouseEvent( pd->imgAt );
}

QString CaptureWgt::savePixmap( const QString & stri )
{
    QString fileName = stri;
    if ( !fileName.endsWith( ".png", Qt::CaseInsensitive ) )
        fileName.append( ".png" );
    
    QImage pm;
    image( pm );
    pm.save( fileName, "PNG", 100 );
    return fileName;
}

void CaptureWgt::image( QImage & img )
{
    QTransform tr;
    qreal scX, scY, trX, trY;
    if ( pd->flipY )
    {
        scX = -1.0;
        trX = static_cast<qreal>( pd->img.width() );
    }
    else
    {
        scX = 1.0;
        trX = 0.0;
    }
    if ( pd->flipX )
    {
        scY = -1.0;
        trY = static_cast<qreal>( pd->img.height() );
    }
    else
    {
        scY = 1.0;
        trY = 0.0;
    }
    tr.scale( scX, scY );
    tr.translate( trX, trY );
    QImage image = pd->img.transformed( tr, Qt::SmoothTransformation );
    img = image;
}

bool CaptureWgt::isMarker() const
{
    bool en = pd->ui.cross->isChecked();
    return en;
}

const QPointF & CaptureWgt::marker() const
{
    return pd->imgAt;
}

void CaptureWgt::listDevices()
{
    QStringList l = pd->cap->devices();
    if ( pd->devMenu )
        pd->devMenu->deleteLater();
    pd->devList.clear();
    pd->devMenu = new QMenu();
    for ( int i=0; i<l.size(); i++ )
    {
        QAction * a = new QAction( 0 );
        a->setText( l.at( i ) );
        a->setCheckable( true );
        connect( a, SIGNAL(triggered()), this, SLOT(slotDevice()) );
        pd->devList << a;
        pd->devMenu->addAction( a );
    }
    Ui_CaptureWgt & ui = pd->ui;
    ui.device->setMenu( pd->devMenu );
    if ( l.size() > 0 )
    {
        pd->cap->setDevice( 0 );
        pd->devList[0]->setChecked( true );
        pd->resolutionAcquired = false;
    }
}

void CaptureWgt::listResolutions()
{
    if ( pd->resolutionAcquired )
        return;
    pd->resolutionAcquired = true;
    if ( pd->resMenu )
        pd->resMenu->deleteLater();
    pd->resMenu = new QMenu();
    pd->resList.clear();
    const qreal factor = 0.8;
    QSize sz = pd->cap->resolution();
    if ( sz.width() < 800 )
    {
        sz.setWidth( 800 );
        sz.setHeight( 600 );
    }
    while ( ( sz.width() > 320 ) && ( sz.height() > 240 ) )
    {
        QString stri = QString( "%1 x %2" ).arg( sz.width() ).arg( sz.height() );
        QAction * a = new QAction( stri, this );
        pd->resMenu->addAction( a );
        pd->resList << a;
        a->setCheckable( true );
        connect( a, SIGNAL(triggered()), this, SLOT(slotResolution()) );
        sz.setWidth( static_cast<int>( static_cast<qreal>( sz.width() ) * factor ) );
        sz.setHeight( static_cast<int>( static_cast<qreal>( sz.height() ) * factor ) );
    }
    Ui_CaptureWgt & ui = pd->ui;
    ui.resolution->setMenu( pd->resMenu );
}

void CaptureWgt::updatePixmap( const QImage & img )
{
    if ( !img.isNull() )
        pd->img = img;
    Ui_CaptureWgt & ui = pd->ui;
    QSizeF imgSz = pd->img.size();
    QSizeF wndSz = ui.view->size();
    
    if ( !pd->img.isNull() )
    {
        qreal sc;
        qreal x, y;
        if ( imgSz.width() * wndSz.height() > imgSz.height() * wndSz.width() )
        {
            sc = wndSz.width() / imgSz.width();
            x = 0.0;
            y = ( static_cast<qreal>( wndSz.height() ) - 
                  static_cast<qreal>( imgSz.height() ) * sc ) * 0.5;
        }
        else
        {
            sc = wndSz.height() / imgSz.height();
            x = ( static_cast<qreal>( wndSz.width() ) - 
                  static_cast<qreal>( imgSz.width() ) * sc ) * 0.5;
            y = 0.0;
        }
        pd->imgPos = QPointF( x, y );
        pd->imgScale = sc;
        
        pd->scene->setSceneRect( QRectF( 0.0, 0.0, wndSz.width(), wndSz.height() ) );
        pd->image->setPixmap( QPixmap::fromImage( pd->img ) );

        QTransform scaleX;
        if ( pd->flipX )
        {
            scaleX.scale( 1.0, -1.0 );
            scaleX.translate( 0.0, -static_cast<qreal>( imgSz.height() ) );
        }
        QTransform scaleY;
        if ( pd->flipY )
        {
            scaleY.scale( -1.0, 1.0 );
            scaleY.translate( -static_cast<qreal>( imgSz.width() ), 0.0 );
        }
        QTransform scale;
        scale.scale( sc, sc );
        QTransform tran;
        tran.translate( x, y );

        tran = scaleX * scaleY * scale * tran;
        pd->image->setTransform( tran );

        bool lineVisible = ( ( ui.cross->isChecked() ) && ( !pd->markerPos.isNull() ) );
        pd->lineX1->setVisible( lineVisible );
        pd->lineY1->setVisible( lineVisible );
        pd->lineX2->setVisible( lineVisible );
        pd->lineY2->setVisible( lineVisible );
        if ( lineVisible )
        {
            qreal w = static_cast<qreal>( pd->img.size().width() );
            qreal h = static_cast<qreal>( pd->img.size().height() );
            qreal xx = static_cast<qreal>( pd->markerPos.x() );
            if ( pd->flipY )
                xx = w - xx;
            qreal yy = static_cast<qreal>( pd->markerPos.y() );
            if ( pd->flipX )
                yy = h - yy;
            QPen penW( Qt::white );
            pd->lineX1->setPen( penW );
            pd->lineY1->setPen( penW );
            pd->lineX1->setLine( 0, yy, w, yy );
            pd->lineY1->setLine( xx, 0, xx, h );

            QPen penB( Qt::black );
            pd->lineX2->setPen( penB );
            pd->lineY2->setPen( penB );
            xx+=1.5;
            yy+=1.5;
            pd->lineX2->setLine( 0, yy, w, yy );
            pd->lineY2->setLine( xx, 0, xx, h );
        }

        drawArrow();

        drawLaserPos();
    }
}

void CaptureWgt::drawArrow()
{
    pd->arrow->setVisible( pd->drawArrow );
    if ( pd->drawArrow )
    {
        QPointF back  = pd->arrowFrom;
        QPointF front = pd->arrowTo;
        if ( front == back )
            return;
        qreal iw = static_cast<qreal>( pd->img.size().width() );
        qreal ih = static_cast<qreal>( pd->img.size().height() );
        if ( pd->flipX )
        {
            front.setY( ih - front.y() );
            back.setY( ih - back.y() );
        }
        if ( pd->flipY )
        {
            front.setX( iw - front.x() );
            back.setX( iw - back.x() );
        }

        // ≈диничный сонаправленный вектор.
        QPointF a = front - back;
        qreal l = sqrt( a.x() * a.x() + a.y() * a.y () );
        a.setX( a.x() / l );
        a.setY( a.y() / l );
        // ≈диничный перпендикул€рный сектор.
        QPointF b = QPointF( a.y(), -a.x() );
        // ѕараметры стрелки.
        const qreal w = 2.0; // Ўирина линии.
        const qreal h = 7.0; // Ўирина собственно треугольничка.

        QPolygonF path;
        path.reserve( 8 );
        QPen pen = QPen( Qt::darkBlue );
        pd->arrow->setPen( pen );
        QBrush brush = QBrush( QColor( 150, 150, 200, 100 ) );
        pd->arrow->setBrush( brush );
        path << ( back + w * b );
        path << ( front - h * a + w * b );
        path << ( front + h * ( b - a ) );
        path << ( front );
        path << ( front - h * ( b + a ) );
        path << ( front - h * a - w * b );
        path << ( back - w * b );
        path << ( back + w * b );
        pd->arrow->setPolygon( path );
    }
}

void CaptureWgt::drawLaserPos()
{
    bool visible = ( pd->ui.laser->isChecked() && !pd->laserSpotPos.isNull() );
    pd->laserPosImage->setVisible( visible );
    if ( visible )
    {
        qreal w = static_cast<qreal>( pd->img.size().width() );
        qreal h = static_cast<qreal>( pd->img.size().height() );
        qreal xx = static_cast<qreal>( pd->laserSpotPos.x() );
        qreal yy = static_cast<qreal>( pd->laserSpotPos.y() );
        xx *= ( w * pd->imgScale );
        yy *= ( h * pd->imgScale );
        xx += pd->imgPos.x();
        yy += pd->imgPos.y();

        QTransform scale;
        scale.scale( 0.15, 0.15 );
        QTransform tran;
        tran.translate( xx, yy );

        pd->laserPosImage->setTransform( tran );
    }
}

void CaptureWgt::updateCrossPos( bool write )
{
    if ( write )
    {
        QSettings s( pd->crossFname, QSettings::IniFormat, this );
        s.setValue( "x", pd->markerPos.x() );
        s.setValue( "y", pd->markerPos.y() );
        s.setValue( "laser_x", pd->laserSpotPos.x() );
        s.setValue( "laser_y", pd->laserSpotPos.y() );
    }
    else
    {
        QSettings s( pd->crossFname, QSettings::IniFormat, this );
        qreal x = s.value( "x", 0.0 ).toDouble();
        qreal y = s.value( "y", 0.0 ).toDouble();
        pd->markerPos = QPointF( x, y );
        x = s.value( "laser_x", 0.0 ).toDouble();
        y = s.value( "laser_y", 0.0 ).toDouble();
        pd->laserSpotPos = QPointF( x, y );
    }
}




