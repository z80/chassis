

#include "usb_io.h"
#include <string.h>
extern "C"
{
#ifdef WIN32
    #include "lusb0_usb.h"
#else
    #include "libusb.h"
#endif
#include "opendevice.h"
}

inline static bool LSB()
{
    int i = 1;
    unsigned char * p = reinterpret_cast<unsigned char *>( &i );
    bool res = (p[0] != 0);
    return res;
}

class UsbIo::PD
{
public:
    PD() {}
    ~PD() {}

    usb_dev_handle  * handle;
    std::string err;
    std::string res;
    std::string output;
    int timeout;
    std::basic_string<unsigned char> dataTo,
                                     dataFrom;
    static const int VENDOR_ID;
    static const int PRODUCT_ID;
    static const int TIMEOUT;
};

const int UsbIo::PD::VENDOR_ID  = 0x16C0;
const int UsbIo::PD::PRODUCT_ID = 0x05DD;
const int UsbIo::PD::TIMEOUT    = 1000;

static const int CONTROL_REQUEST_TYPE_IN = USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN;
static const int CONTROL_REQUEST_TYPE_OUT = USB_TYPE_VENDOR | USB_RECIP_ENDPOINT | USB_ENDPOINT_OUT;

static const int HID_GET_REPORT = 0x01;
static const int HID_SET_REPORT = 0x09;

UsbIo::UsbIo()
{
    pd = new PD();
    pd->handle = 0;
    pd->timeout = PD::TIMEOUT;
    usb_init();
    usb_set_debug( 3 );
    pd->res.resize( 8 );
}

UsbIo::~UsbIo()
{
    if ( isOpen() )
        close();
    //libusb_exit( pd->cxt );
    delete pd;
}

bool UsbIo::open()
{
    QMutexLocker lock( &m_openMutex );

    pd->handle = 0;
    
    int res = usbOpenDevice( &(pd->handle),
                             PD::VENDOR_ID, NULL,
                             PD::PRODUCT_ID, NULL,
                             NULL, NULL, NULL );
    return (res == 0);
}

void UsbIo::close()
{
    if ( isOpen() )
    {
    	QMutexLocker lock( &m_openMutex );

        usb_close( pd->handle );
        pd->handle = 0;
    }
}

bool UsbIo::isOpen() const
{
	QMutexLocker lock( &m_openMutex );

    return (pd->handle != 0);
}

int UsbIo::maxPacketSize() const
{
    return 254;
}

int UsbIo::write( const std::basic_string<unsigned char> & to )
{
    if ( !isOpen() )
    {
        if ( !open() )
            return -1;
    }

    /*
    int res = libusb_control_transfer(
                  pd->handle,
                  CONTROL_REQUEST_TYPE_OUT,
                  HID_SET_REPORT,
                  0, 0,
                  const_cast<unsigned char *>( to.data() ), to.size(), pd->timeout );
    */
    int res =  usb_control_msg( pd->handle, CONTROL_REQUEST_TYPE_OUT, 
                                HID_SET_REPORT,
                                0, 
                                0, 
                                (char *)( to.data() ), 
                                to.size(),
                                pd->timeout );



    if ( res < 0 )
    {
        close();
        open();
        return res;
    }
    return res;
}

int UsbIo::read( std::basic_string<unsigned char> & from ,int size )
{
    if ( !isOpen() )
    {
        if ( !open() )
            return -1;
    }
    from.resize( size );
    /*
    int res = libusb_control_transfer(
                  pd->handle,
                  CONTROL_REQUEST_TYPE_IN,
                  HID_GET_REPORT,
                  0, 0,
                  const_cast<unsigned char *>( from.data() ), from.size(), pd->timeout );
    */

    int res =  usb_control_msg( pd->handle, CONTROL_REQUEST_TYPE_IN, 
                                HID_GET_REPORT,
                                0, 
                                0, 
                                (char *)( from.data() ), 
                                from.size(),
                                pd->timeout );



    if ( res < 0 )
    {
        close();
        return res;
    }
    return res;
}



int UsbIo::setTimeout( int ms )
{
    pd->timeout = ms;
    return 0;
}

std::basic_string<unsigned char> & UsbIo::dataTo()
{
    return pd->dataTo;
}

std::basic_string<unsigned char> & UsbIo::dataFrom()
{
    return pd->dataFrom;
}









