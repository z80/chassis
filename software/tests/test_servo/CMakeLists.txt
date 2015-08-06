
set( PROJ_NAME test_servo )

find_package( Qt4 )
if ( WIN32 )
    set( QT_USE_QTMAIN     True )
endif ( WIN32 )
set( QT_USE_QTGUI      True )
set( QT_USE_QTNETWORK  True )
set( QT_USE_QTXML      True )
include( ${QT_USE_FILE} )

include_directories( "./src"
                     ${CMAKE_CURRENT_BINARY_DIR} )

file( GLOB src     "./src/*.c" )

Qt4_WRAP_CPP( mocSrc ${headers} )
set( src ${src} ${mocSrc} )

QT4_WRAP_UI( uiSrc ${ui} )
set( src ${src} ${uiSrc} )

QT4_ADD_RESOURCES( qrcSrc ${qrc} )
set( src ${src} ${qrcSrc} )

add_executable( ${PROJ_NAME} ${src} )
set( libs ${QT_LIBRARIES} common )

target_link_libraries( ${PROJ_NAME} ${QT_LIBRARIES} )
install( TARGETS ${PROJ_NAME} DESTINATION "./" )
