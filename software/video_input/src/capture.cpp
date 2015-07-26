
#include "capture.h"
#include "videoInput.h"

#include <cvconfig.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/bind.hpp>
#include <boost/bind/arg.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/function.hpp>


class Capture::PD
{
public:
    videoInput vi;
    int dev;
    int conn;
    qreal fps;
    QSize res;
    QVarLengthArray<unsigned char> data;
    
    // Данные для работы.
    cv::Mat              orig, 
                         dest;
    std::vector<cv::Mat> rgb;
    QVector<qreal>       accum;
    QImage               img;
    // Настройки коррекции яркости.
    QPointF              brightness;
    
    QStringList activeFilters;
    void process( int w, int h, Capture * owner );
    // Обработки картинок.
    void equalizeHist();
    void fullContrast();
    void sobel();
    void median();
    void initTimeSmooth();
    void timeSmooth();
    void highPass();
    void surfaceSubtract();
    void brightnessRange();
};

void Capture::PD::process( int w, int h, Capture * owner )
{
    cv::Mat & orig = this->orig;
    if ( ( orig.rows != h ) || ( orig.cols != w ) )
    {
        orig = cv::Mat( h, w, CV_8UC3 );
        std::vector< cv::Mat > & channels = this->rgb;
        channels.clear();
        cv::Mat r( h, w, CV_8UC1 );
        channels.push_back( r );
        cv::Mat g( h, w, CV_8UC1 );
        channels.push_back( g );
        cv::Mat b( h, w, CV_8UC1 );
        channels.push_back( b );

        dest = cv::Mat( h, w, CV_8UC4 );
    }
    memcpy( orig.ptr(), data.data(), data.size() );

    std::vector< cv::Mat > & channels = this->rgb;
    cv::split( orig, channels );

    for ( int i=0; i<activeFilters.size(); i++ )
    {
        QString name = activeFilters.at( i );
        if ( name == "equalizeHist" )
            equalizeHist();
        else if ( name == "fullContrast" )
            fullContrast();
        else if ( name == "sobel" )
            sobel();
        else if ( name == "median" )
            median();
        else if ( name == "timeSmooth" )
            timeSmooth();
        else if ( name == "highPass" )
            highPass();
        else if ( name == "surfaceSubtract" )
            surfaceSubtract();
        else if ( name == "brightnessRange" )
            brightnessRange();
    }

    cv::Mat & dest = this->dest;
    cv::merge( channels, dest );

    QImage img( dest.data, w, h, QImage::Format_RGB888 );
    this->img = img;
}

void Capture::PD::equalizeHist()
{
    cv::Mat & orig = this->orig;
    std::vector<cv::Mat> & rgb = this->rgb;

    cv::Mat dest0 = rgb.at( 0 );
    cv::equalizeHist( rgb[ 0 ], dest0 );
    rgb[0] = dest0;

    cv::Mat dest1 = rgb.at( 1 );
    cv::equalizeHist( rgb[ 1 ], dest1 );
    rgb[1] = dest1;

    cv::Mat dest2 = rgb.at( 2 );
    cv::equalizeHist( rgb[ 2 ], dest2 );
    rgb[2] = dest2;
}

void Capture::PD::fullContrast()
{
    cv::Mat & orig = this->orig;
    std::vector<cv::Mat> & rgb = this->rgb;

    cv::Mat & r = rgb.at( 0 );
    cv::Mat & g = rgb.at( 1 );
    cv::Mat & b = rgb.at( 2 );
    int rows = r.rows;
    int cols = r.cols;

    int rmin = r.data[0], rmax = r.data[0];
    int gmin = g.data[0], gmax = g.data[0];
    int bmin = b.data[0], bmax = b.data[0];
    int ind = 0;
    for ( int i=0; i<rows; i++ )
    {
        for ( int j=0; j<cols; j++ )
        {
            uchar v = r.data[ ind ];
            rmin = (rmin > v) ? v : rmin;
            rmax = (rmax < v) ? v : rmax;

            v = g.data[ ind ];
            gmin = (gmin > v) ? v : gmin;
            gmax = (gmax < v) ? v : gmax;

            v = b.data[ ind ];
            bmin = (bmin > v) ? v : bmin;
            bmax = (bmax < v) ? v : bmax;

            ind ++;
        }
    }
    if ( (rmin != rmax) && (gmin != gmax) && (bmin != bmax) )
    {
        ind = 0;
        for ( int i=0; i<rows; i++ )
        {
            for ( int j=0; j<cols; j++ )
            {
                int v = static_cast<int>( r.data[ ind ] );
                v = 255 * (v - rmin) / (rmax - rmin);
                r.data[ ind ] = static_cast<uchar>( v );
                
                v = static_cast<int>( g.data[ ind ] );
                v = 255 * (v - gmin) / (gmax - gmin);
                g.data[ ind ] = static_cast<uchar>( v );

                v = static_cast<int>( b.data[ ind ] );
                v = 255 * (v - bmin) / (bmax - bmin);
                b.data[ ind ] = static_cast<uchar>( v );

                ind ++;
            }
        }
    }
}

void Capture::PD::sobel()
{
    cv::Mat & orig = this->orig;
    std::vector<cv::Mat> & rgb = this->rgb;

    int dx = 1, dy = 1;

    int rows = rgb.at( 0 ).rows;
    int cols = rgb.at( 0 ).cols;

    std::vector<int> mean;
    mean.resize( 3 );
    for ( int k=0; k<3; k++ )
    {
        int m = 0;
        int ind = 0;
        cv::Mat & val = rgb[k];
        for ( int i=0; i<rows; i++ )
        {
            for ( int j=0; j<cols; j++ )
            {
                m += val.data[ ind++ ];
            }
        }
        mean[k] = m / ind;
    }



    cv::Mat dest0 = rgb.at( 0 );
    cv::Sobel( rgb[0], dest0, CV_8UC1, dx, dy );
    //rgb[0] = dest0;
    {
        cv::Mat & src = rgb[0];
        cv::Mat & dest = dest0;
        int ind = 0;
        int m = mean.at( 0 );
        for ( int i=0; i<rows; i++ )
        {
            for ( int j=0; j<cols; j++ )
            {
                uchar s = src.data[ ind ];
                uchar d = dest.data[ ind ];
                src.data[ ind ] = (( s > d ) ? s : d) + m;
                ind ++;
            }
        }
    }

    cv::Mat dest1 = rgb.at( 1 );
    cv::Sobel( rgb[1], dest1, CV_8UC1, dx, dy );
    //rgb[1] = dest1;
    {
        cv::Mat & src = rgb[1];
        cv::Mat & dest = dest1;
        int ind = 0;
        int m = mean.at( 1 );
        for ( int i=0; i<rows; i++ )
        {
            for ( int j=0; j<cols; j++ )
            {
                uchar s = src.data[ ind ];
                uchar d = dest.data[ ind ];
                src.data[ ind ] = (( s > d ) ? s : d) + m;
                ind ++;
            }
        }
    }

    cv::Mat dest2 = rgb.at( 2 );
    cv::Sobel( rgb[2], dest2, CV_8UC1, dx, dy );
    //rgb[2] = dest2;
    {
        cv::Mat & src = rgb[2];
        cv::Mat & dest = dest2;
        int ind = 0;
        int m = mean.at( 2 );
        for ( int i=0; i<rows; i++ )
        {
            for ( int j=0; j<cols; j++ )
            {
                uchar s = src.data[ ind ];
                uchar d = dest.data[ ind ];
                src.data[ ind ] = (( s > d ) ? s : d) + m;
                ind ++;
            }
        }
    }
}

void Capture::PD::median()
{
    cv::Mat & orig = this->orig;
    std::vector<cv::Mat> & rgb = this->rgb;

    int ksize = 3;
    //QVariant arg = args[ "ksize" ];
    //if ( !arg.isNull() )
    //    ksize = arg.toInt();

    cv::Mat dest0 = rgb.at( 0 );
    medianBlur( rgb[0], dest0, ksize );
    rgb[0] = dest0;

    cv::Mat dest1 = rgb.at( 1 );
    medianBlur( rgb[1], dest1, ksize );
    rgb[1] = dest1;

    cv::Mat dest2 = rgb.at( 2 );
    medianBlur( rgb[2], dest2, ksize );
    rgb[2] = dest2;
}

void Capture::PD::initTimeSmooth()
{
    std::vector<cv::Mat> & rgb = this->rgb;

    qreal alpha = 0.1;
    //QVariant arg = args[ "alpha" ];
    //if ( !arg.isNull() )
    //    alpha = arg.toReal();
    qreal beta = 1.0 - alpha;

    int w = rgb[0].cols;
    int h = rgb[0].rows;
    int sz = w * h * 3;
    QVector<qreal> & acc = this->accum;
    if ( acc.size() < sz )
    {
        acc.clear();
        acc.resize( sz );
        acc.fill( 0.0 );
    }
    for ( int i=0; i<3; i++ )
    {
        cv::Mat & img = rgb[i];
        int ind = 0;
        for ( int ky=0; ky<h; ky++ )
        {
            for ( int kx=0; kx<w; kx++ )
            {
                int absInd = i * w * h + ind;
                qreal v = static_cast<qreal>( img.data[ ind ] );
                img.data[ ind ] = static_cast<uchar>( v );
                acc[ absInd ] = v;
                ind ++;
            }
        }
    }
}

void Capture::PD::timeSmooth()
{
    std::vector<cv::Mat> & rgb = this->rgb;

    qreal alpha = 0.1;
    //QVariant arg = args[ "alpha" ];
    //if ( !arg.isNull() )
    //    alpha = arg.toReal();
    qreal beta = 1.0 - alpha;

    int w = rgb[0].cols;
    int h = rgb[0].rows;
    int sz = w * h * 3;
    QVector<qreal> & acc = this->accum;
    if ( acc.size() < sz )
    {
        acc.clear();
        acc.resize( sz );
        acc.fill( 0.0 );
    }
    for ( int i=0; i<3; i++ )
    {
        cv::Mat & img = rgb[i];
        int ind = 0;
        for ( int ky=0; ky<h; ky++ )
        {
            for ( int kx=0; kx<w; kx++ )
            {
                int absInd = i * w * h + ind;
                qreal v = static_cast<qreal>( img.data[ ind ] );
                v = acc.at( absInd ) * beta + v * alpha;
                img.data[ ind ] = static_cast<uchar>( v );
                acc[ absInd ] = v;
                ind ++;
            }
        }
    }
}

void Capture::PD::highPass()
{
    std::vector<cv::Mat> & rgb = this->rgb;

    int cols = rgb[0].cols;
    int rows = rgb[0].rows;
    const qint32 factor = 23;
    const qint32 ax0 = 8224268;
    const qint32 ax1 = -8224268;
    const qint32 ax2 = 0;
    const qint32 ax3 = 0;
    const qint32 ay1 = 8059927;
    const qint32 ay2 = 0;
    const qint32 ay3 = 0;
    qint32 x, y;
    qint32 x1, x2, x3, y1, y2, y3;

    unsigned cnt = rgb.size();
    for ( unsigned k=0; k<cnt; k++ )
    {
        uchar * data = rgb[k].data;
        // Находим среднее по всему полю.
        quint32 m = 0;
        int ind = 0;
        for ( int row=0; row<rows; row++ )
            for ( int col=0; col<cols; col++ )
                m += static_cast<quint32>( data[ind++] );
        m /= ind;

        // Фильрация по горизонтали.
        for ( int row=0; row<rows; row++ )
        {
            x = y = x1 = x2 = x3 = y1 = y2 = y3 = 0;
            // Предварительный разгон.
            const int nn = 16;
            for ( int col=nn; col>0; col-- )
            {
                int ind = row * cols + col;
                x = static_cast<qint32>( data[ind] );
                y = ax0 * x + 
                    ax1 * x1 + ay1 * y1 + 
                    ax2 * x2 + ay2 * y2 + 
                    ax3 * x3 + ay3 * y3;
                y >>= 23;

                y3 = y2;
                y2 = y1;
                y1 = y;

                x3 = x2;
                x2 = x1;
                x1 = x;
            }
            // Фильтрация по горизонтали.
            for ( int col=0; col<cols; col++ )
            {
                int ind = row * cols + col;
                x = static_cast<qint32>( data[ind] );
                y = ax0 * x + 
                    ax1 * x1 + ay1 * y1 + 
                    ax2 * x2 + ay2 * y2 + 
                    ax3 * x3 + ay3 * y3;
                y >>= 23;

                y3 = y2;
                y2 = y1;
                y1 = y;

                x3 = x2;
                x2 = x1;
                x1 = x;

                y += m;
                y = ( y<=255 ) ? y : 255;
                y = ( y>=0 ) ? y : 0;
                data[ind] = static_cast<uchar>( y );
            }
        }


        // Фильрация по вертикали.
        for ( int col=0; col<cols; col++ )
        {
            x = y = x1 = x2 = x3 = y1 = y2 = y3 = 0;
            // Предварительный разгон.
            const int nn = 16;
            for ( int row=nn; row>0; row-- )
            {
                int ind = row * cols + col;
                x = static_cast<qint32>( data[ind] );
                y = ax0 * x + 
                    ax1 * x1 + ay1 * y1 + 
                    ax2 * x2 + ay2 * y2 + 
                    ax3 * x3 + ay3 * y3;
                y >>= 23;

                y3 = y2;
                y2 = y1;
                y1 = y;

                x3 = x2;
                x2 = x1;
                x1 = x;
            }
            // Фильтрация по горизонтали.
            for ( int row=0; row<rows; row++ )
            {
                int ind = row * cols + col;
                x = static_cast<qint32>( data[ind] );
                y = ax0 * x + 
                    ax1 * x1 + ay1 * y1 + 
                    ax2 * x2 + ay2 * y2 + 
                    ax3 * x3 + ay3 * y3;
                y >>= 23;

                y3 = y2;
                y2 = y1;
                y1 = y;

                x3 = x2;
                x2 = x1;
                x1 = x;

                y += m;
                y = ( y<=255 ) ? y : 255;
                y = ( y>=0 ) ? y : 0;
                data[ind] = static_cast<uchar>( y );
            }
        }
    }
}

void Capture::PD::surfaceSubtract()
{
    std::vector<cv::Mat> & rgb = this->rgb;

    int cols = rgb[0].cols;
    int rows = rgb[0].rows;

    unsigned cnt = rgb.size();
    for ( unsigned k=0; k<cnt; k++ )
    {
        uchar * data = rgb[k].data;

        const int ORDER = 3;
        const int MATRIX_SZ = (ORDER+1)*(ORDER+2)/2;
        double powsX[MATRIX_SZ], powsY[MATRIX_SZ];
        int ind = 0;
        for ( int ox=0; ox<=ORDER; ox++ )
        {
            for ( int oy=0; oy<=ORDER-ox; oy++ )
            {
                powsX[ind] = ox;
                powsY[ind] = oy;
                ind++;
            }
        }
        // Матрица и столбец.
        double a[ MATRIX_SZ * MATRIX_SZ ], 
               b[ MATRIX_SZ ];
        // Обнуление элементов.
        ind = 0;
        for ( int j=0; j<MATRIX_SZ; j++ )
        {
            b[j] = 0.0;
            for ( int i=0; i<MATRIX_SZ; i++ )
                a[ j * MATRIX_SZ + i ] = 0.0;
        }
        // Суммирование элементов.
        const int GRID_STEP = 16;
        for ( int iy=GRID_STEP; iy<rows; iy+=GRID_STEP )
        {
            double y = static_cast<double>(iy);
            for ( int ix=GRID_STEP; ix<cols; ix+=GRID_STEP )
            {
                double x = static_cast<double>(ix);
                double v = static_cast<double>( data[ ix + cols * iy ] );
                // Заполнение матрицы.
                // Цикл по вертикали.
                for ( int my=0; my<MATRIX_SZ; my++ )
                {
                    // Коэффициент по вертикали.
                    double coefY = 1.0;
                    int pX = powsX[my];
                    int pY = powsY[my];
                    for ( int i=0; i<pX; i++ )
                        coefY *= x;
                    for ( int i=0; i<pY; i++ )
                        coefY *= y;
                    // Цикл по горизонтели.
                    for ( int mx=0; mx<MATRIX_SZ; mx++ )
                    {
                        // Коэффициент по горизонтали.
                        double coefX = 1.0;
                        int pX = powsX[mx];
                        int pY = powsY[mx];
                        for ( int i=0; i<pX; i++ )
                            coefX *= x;
                        for ( int i=0; i<pY; i++ )
                            coefX *= y;
                        a[ my*MATRIX_SZ+mx ] += coefX * coefY;
                    }
                    b[my] += v * coefY;
                }
            }
        }

        // Обращение матрицы.
        double ia[ MATRIX_SZ * MATRIX_SZ ];
        for ( int i=0; i<(MATRIX_SZ); i++ )
        {
            for ( int j=0; j<MATRIX_SZ; j++ )
            {
                ia[j+i*MATRIX_SZ] = ( i == j ) ? 1.0 : 0.0;
            }
        }
        // Обнуление под главной диагональю.
        for ( int n=0; n<MATRIX_SZ; n++ )
        {
            // Если элемент нулевой, то добавить сумму остальных строк.
            int ind = n * MATRIX_SZ + n;
            double v = a[ ind ];
            if ( fabs( v ) == 0.0 )
            {
                // К каждому элементу добавляем.
                for ( int iy=0; iy<MATRIX_SZ; iy++ )
                {
                    if ( iy == n )
                        continue;
                    for ( int ix=0; ix<MATRIX_SZ; ix++ )
                    {
                        int indTo = ix + MATRIX_SZ * n;
                        int indFrom = ix + MATRIX_SZ * iy;
                        a[ indTo ] += a[ indTo ];
                        ia[ indTo ] += ia[ indFrom ];
                    }
                    v = a[ ind ];
                    if ( fabs( v ) != 0.0 )
                        break;
                }
            }
            // Делим строку на v.
            for ( int ix=0; ix<MATRIX_SZ; ix++ )
            {
                int ind = ix + MATRIX_SZ * n;
                a[ ind ] /= v;
                ia[ ind ] /= v;
            }
            // Обнуляем элементы под текущим элементом n.
            for ( int iy=n+1; iy<MATRIX_SZ; iy++ )
            {
                int indDec = n + MATRIX_SZ * iy;
                double coef = a[ indDec ];
                // Применяем для всех элементов строки матрицы.
                for ( int ix=0; ix<MATRIX_SZ; ix++ )
                {
                    int indSub = ix + MATRIX_SZ * n;
                    int indDec = ix + MATRIX_SZ * iy;
                    a[ indDec ] -= coef * a[ indSub ];
                    ia[ indDec ] -= coef * ia[ indSub ];
                }
            }
            // Обнуляем элементы над текущим элементом.
            for ( int iy=0; iy<=(n-1); iy++ )
            {
                int indDec = n + MATRIX_SZ * iy;
                double coef = a[ indDec ];
                // Применяем для всех элементов матриц.
                for ( int ix=0; ix<MATRIX_SZ; ix++ )
                {
                    int indSub = ix + MATRIX_SZ * n;
                    int indDec = ix + MATRIX_SZ * iy;
                    a[ indDec ] -= coef * a[ indSub ];
                    ia[ indDec ] -= coef * ia[ indSub ];
                }
            }
        }
        // В ia должна быть обратная матрица. Хотя вряд ли.
        double aa[ MATRIX_SZ ];
        for ( int iy=0; iy<MATRIX_SZ; iy++ )
        {
            aa[ iy ] = 0.0;
            for ( int ix=0; ix<MATRIX_SZ; ix++ )
                aa[ iy ] += b[ ix ] * ia[ ix + MATRIX_SZ * iy ];
        }

        // Вычитание этой поверхности.
        for ( int iy=0; iy<rows; iy+=1 )
        {
            double y = static_cast<double>(iy);
            for ( int ix=0; ix<cols; ix+=1 )
            {
                double x = static_cast<double>(ix);
                ind = ix + cols * iy;
                double v = static_cast<double>( data[ ind ] );
                double sub = 0.0;
                for ( int i=0; i<MATRIX_SZ; i++ )
                {
                    double s = aa[ i ];
                    int p = powsX[i];
                    for ( int j=0; j<p; j++ )
                        s *= x;
                    p = powsY[i];
                    for ( int j=0; j<p; j++ )
                        s *= y;
                    sub += s;
                }
                v = v - sub + 127.0;
                v = ( v < 255.0 ) ? v : 255.0;
                v = ( v >= 0.0 ) ? v : 0.0;
                data[ ind ] = static_cast<uchar>( v );
            }
        }
    }
}

void Capture::PD::brightnessRange()
{
    std::vector<cv::Mat> & rgb = this->rgb;

    int cols = rgb[0].cols;
    int rows = rgb[0].rows;
    int vmin = brightness.x();
    int vmax = brightness.y();

    unsigned cnt = rgb.size();
    for ( unsigned k=0; k<cnt; k++ )
    {
        uchar * data = rgb[k].data;
        int ind = 0;
        for ( int iy=0; iy<rows; iy++ )
        {
            for ( int ix=0; ix<cols; ix++ )
            {
                int val = data[ ind ];
                val = 255 * ( val - vmin ) / (vmax - vmin);
                val = ( val < 255 ) ? val : 255;
                val = ( val >= 0 )  ? val : 0;
                data[ ind++ ] = val;
            }
        }
    }
}






Capture::Capture( QObject * parent )
: QTimer( parent )
{
    CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );

    pd = new PD();
    pd->dev  = -1;
    pd->conn = -1;
    pd->fps  = 10.0;
    pd->brightness = QPointF( 0.0, 255.0 );

    QTimer::setInterval( 67 );
    connect( this, SIGNAL(timeout()), this, SLOT(slotTimeout()) );
}

Capture::~Capture()
{
    delete pd;
}

QStringList Capture::devices()
{
    int cnt = pd->vi.listDevices();	
    QStringList l;
    l.reserve( cnt );
    for ( int i=0; i<cnt; i++ )
    {
        QString stri = pd->vi.getDeviceName( i );
        l << stri;
    }
    return l;
}

bool Capture::setDevice( int index )
{
    pd->dev = index;
    bool res = updateDevice();
    return res;
}

bool Capture::setFramerate( qreal fps )
{
    if ( fps > 0.0 )
    {
        int interval = static_cast<int>( 1000.0 / fps );
        pd->fps = fps;
        QTimer::setInterval( interval );
        bool res = updateDevice();
        return res;
    }
    return false;
}

bool Capture::setResolution( const QSize & sz )
{
    pd->res = sz;
    bool res = updateDevice();
    return res;
}

bool Capture::start()
{
	if ( pd->dev >= 0 ) {
		QTimer::start();
		return true;
	}
	else
		return false;
}

bool Capture::stop()
{
    QTimer::stop();
    return true;
}

bool Capture::settingsWindow()
{
    int dev = pd->dev;
    pd->vi.showSettingsWindow( dev );
    return true;
}

void Capture::setInput( TInput input )
{
    switch ( input )
    {
    case TSVideo:
        pd->conn = VI_S_VIDEO;
        break;
    case TComposite:
        pd->conn = VI_COMPOSITE;
        break;
    case TUsb:
        pd->conn = VI_USB;
        break;
    default:
        pd->conn = -1;
    }
    updateDevice();
}

QSize Capture::resolution()
{
    //int w = pd->vi.getWidth( pd->dev );
	//int h = pd->vi.getHeight( pd->dev );
    //return QSize( w, h );
    return pd->img.size();
}

QStringList Capture::filters()
{
    QStringList l;
    l << "equalizeHist" 
      << "fullContrast" 
      << "sobel" 
      << "median" 
      << "timeSmooth" 
      << "highPass" 
      << "surfaceSubtract" 
      << "brighntessRange";
    return l;
}

void Capture::addFilter( const QString & stri )
{
    pd->activeFilters << stri;
    if ( stri == "timeSmooth" )
        pd->initTimeSmooth();
}

void Capture::removeFilter( const QString & stri )
{
    bool cont;
    do {
        cont = false;
        for ( QStringList::iterator i=pd->activeFilters.begin(); 
              i!=pd->activeFilters.end(); 
              i++ )
        {
            if ( *i == stri )
            {
                pd->activeFilters.erase( i );
                cont = true;
                break;
            }
        }
    } while ( cont );
}

void Capture::clearFilters()
{
    pd->activeFilters.clear();
}

void Capture::setBrightnessRange( const QPointF & range )
{
    pd->brightness = range;
}

void Capture::slotTimeout()
{
    int dev = pd->dev;
	int size = pd->vi.getSize( dev );
    if ( size > 0 )
    {
	    int w = pd->vi.getWidth( dev );
	    int h = pd->vi.getHeight( dev );
    	
	    if ( pd->data.size() != size )
            pd->data.resize( size );
    	
	    //to get the data from the device first check if the data is new
	    if ( pd->vi.isFrameNew( dev ) )
        {
		    pd->vi.getPixels( dev, pd->data.data(), true, true );	//fills pixels as a BGR (for openCV) unsigned char array - no flipping

            pd->process( w, h, this );
            emit frame( pd->img );
        }
    }
}

bool Capture::updateDevice()
{
    if ( pd->dev >= 0 )
        pd->vi.stopDevice( pd->dev );
	pd->vi.setIdealFramerate( pd->dev, static_cast<int>(pd->fps) );
    bool res;
    if ( pd->conn >= 0 )
    {
        if ( !( pd->res.width() > 0 ) || !( pd->res.height() > 0 ) )
	        res = pd->vi.setupDevice( pd->dev, pd->conn );
        else
            res = pd->vi.setupDevice( pd->dev, pd->res.width(), pd->res.height(), pd->conn );
    }
    else
    {
        if ( !( pd->res.width() > 0 ) || !( pd->res.height() > 0 ) )
            res = pd->vi.setupDevice( pd->dev );
        else
            res = pd->vi.setupDevice( pd->dev, pd->res.width(), pd->res.height() );
    }
    return res;
}





