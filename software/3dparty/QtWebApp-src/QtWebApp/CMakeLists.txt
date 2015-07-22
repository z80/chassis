
set( proj "qtwebapp" )

find_package( Qt4 REQUIRED )
set( QT_USE_QTGUI     true )
set( QT_USE_QTSVG     true )
set( QT_USE_QTNETWORK true )
include( ${QT_USE_FILE} )

include_directories( "./httpserver"
                     "./logging"
                     "./qtservice"
                     "./remplateengine"
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/qtservice )

file( GLOB hs_src     "./httpserver/*.cpp" )
file( GLOB lg_src     "./logging/*.cpp" )
file( GLOB qs_src     "./qtservice/qtservice.cpp" )
if ( WIN32 )
    #set( qs_src  ${qs_src} "./qtservice/qtservice_win.cpp" )
else ( WIN32 )
    set( qs_src  ${qs_src} "./qtservice/qtservice_unix.cpp" "./qtservice/qtunixserversocket.cpp" "./qtservice/qtunixsocket.cpp" )
endif ( WIN32 )
file( GLOB te_src     "./templateengine/*.cpp" )

set( src ${hs_src} ${lg_src} ${te_src} )
if ( UNIX )
set( src ${src} ${qs_src} )
endif ( UNIX )
file( GLOB hs_headers "./httpserver/*.h" )
file( GLOB lg_headers "./logging/*.h" )
if ( UNIX )
    file( GLOB qs_headers "./qtservice/*.h" )
endif ( UNIX )
file( GLOB te_headers "./templateengine/*.h" )
set( headers ${hs_headers} ${lg_headers} ${qs_headers} ${te_headers} )
message( STATUS "SRC = ${src}" )
message( STATUS "HEADERS = ${headers}" )

qt4_wrap_cpp( moc_src ${headers} )
set( src ${src} ${moc_src} )

add_library( ${proj} STATIC ${src} )
target_link_libraries( ${proj} ${QT_LIBRARIES} )
