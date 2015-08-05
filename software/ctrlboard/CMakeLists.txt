
find_package( Qt4 )
set( QT_USE_QTCORE     True )
include( ${QT_USE_FILE} )

aux_source_directory( "./src" src )
aux_source_directory( "./src/libusb" lsrc )
set( src ${src} ${lsrc} )
message( STATUS "src: \"${src}\"" )

if ( UNIX )
    include( "${ROOT}/cmake/findlibusb-1.0.cmake" )
    message( STATUS ${LIBUSB_DEFINITIONS} )
endif ( UNIX )

include_directories( "./src"
                     "${ROOT}/../common"
                     ${GLOB_BIN_DIR} )
if ( UNIX )
    include_directories( "${LIBUSB_1_INCLUDE_DIRS}" )
    include_directories( "./src/linux" )
    aux_source_directory( "./src/linux" platSrc )
elseif ( WIN32 )
    include_directories( "${LIBUSB_INC_PATH}"
                         "./src/libusb" )
    include_directories( "./src/win32" )
    aux_source_directory( "./src/win32" platSrc )
endif ( UNIX )
set( src ${src} ${platSrc} )

add_library( ctrlboard STATIC ${src} )
set( defs "" )
if ( UNIX )
    set( defs ${defs} ${LIBUSB_1_DEFINITIONS} )
    target_link_libraries( ctrlboard ${LIBUSB_LIBRARIES} ${QT_LIBRARIES} ${LIBUSB_1_LIBRARIES} )
elseif ( WIN32 )
    find_library( libusb NAMES libusb.lib PATHS "${LIBUSB_LIB_PATH}" )
    target_link_libraries( ctrlboard ${libusb} ${QT_LIBRARIES} )
endif ( UNIX )
