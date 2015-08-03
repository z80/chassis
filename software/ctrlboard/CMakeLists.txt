
find_package( Qt4 )
set( QT_USE_QTCORE     True )
include( ${QT_USE_FILE} )

aux_source_directory( "./src" src )
aux_source_directory( "./src/libusb" lsrc )
set( src ${src} ${lsrc} )
message( STATUS "src: \"${src}\"" )

if ( UNIX )
    include( "../cmake/findlibusb.cmake" )
    message( STATUS ${LIBUSB_1_DEFINITIONS} )
endif ( UNIX )

include_directories( "./src" 
                     "${ROOT}/../common" 
                     ${GLOB_BIN_DIR} )
if ( UNIX )
    include_directories( "${LIBUSB_INCLUDE_DIRS}" )
elseif ( WIN32 )
    include_directories( "${LIBUSB_INC_PATH}" 
                         "./src/libusb" )
endif ( UNIX )
set( src ${src} ${s} )

add_library( ctrlboard STATIC ${src} )
if ( UNIX )
    target_link_libraries( ctrlboard ${LIBUSB_LIBRARIES} ${QT_LIBRARIES} )
elseif ( WIN32 )
    find_library( libusb NAMES libusb.lib PATHS "${LIBUSB_LIB_PATH}" )
    target_link_libraries( ctrlboard ${libusb} ${QT_LIBRARIES} )
endif ( UNIX )




