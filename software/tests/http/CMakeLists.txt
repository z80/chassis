
set( PROJ_NAME test_http )

if ( USE_OPENCV )
    find_package( OpenCV REQUIRED )
endif ( USE_OPENCV )

find_package( Qt4 )
if ( WIN32 )
    set( QT_USE_QTMAIN     True )
endif ( WIN32 )
set( QT_USE_QTGUI      True )
set( QT_USE_QTNETWORK  True )
set( QT_USE_QTXML      True )
include( ${QT_USE_FILE} )

include_directories( "./src"
                     "${ROOT}/3dparty/qxmpp/src/base"
                     "${ROOT}/3dparty/qxmpp/src/client"
                     "${ROOT}/3dparty/qxmpp/src/server"
                     "${ROOT}/xmpp_io/src"
                     "${ROOT}/common/src"
                     ${CMAKE_CURRENT_BINARY_DIR}
                     ${ROOT}/3dparty/QtWebApp-src/QtWebApp
                     ${ROOT}/3dparty/QtWebApp-src/QtWebApp/httpserver
                     ${ROOT}/3dparty/QtWebApp-src/QtWebApp/logging
                     ${ROOT}/3dparty/QtWebApp-src/QtWebApp/qtservice
                     ${ROOT}/3dparty/QtWebApp-src/QtWebApp/templateengine )

if ( USE_OPENCV )
    include_directories( ${OPENCV_INCLUDE_DIRS} )
endif ( USE_OPENCV )


file( GLOB src     "./src/*.cpp" )
file( GLOB win_src "./src/windows/*.cpp" )
file( GLOB lin_src "./src/linux/*.cpp" )
file( GLOB headers "./src/*.h" )
file( GLOB win_headers "./src/windows/*.h" )
file( GLOB lin_headers "./src/linux/*.h" )
file( GLOB ui      "./src/*.ui" )
file( GLOB qrc     "./src/*.qrc" )
if ( WIN32 )
    set( src ${src} ${win_src} )
    set( headers ${headers} ${win_headers} )
    include_directories( "./src/windows"
                         "${ROOT}/video_input/src" )
else ( WIN32 )
    set( src ${src} ${lin_src} )
    set( headers ${headers} ${lin_headers} )
    include_directories( "./src/linux" )
endif ( WIN32 )
#message( STATUS "SRC = ${src}" )
#message( STATUS "HEADERS = ${headers}" )

Qt4_WRAP_CPP( mocSrc ${headers} )
set( src ${src} ${mocSrc} )

QT4_WRAP_UI( uiSrc ${ui} )
set( src ${src} ${uiSrc} )

QT4_ADD_RESOURCES( qrcSrc ${qrc} )
set( src ${src} ${qrcSrc} )

if ( WIN32 )
    list( APPEND src "./src/icon.rc" )
    set( BLD_TYPE WIN32 )
else ( WIN32 )
	set( BLD_TYPE "" )
endif ( WIN32 )



add_executable( ${PROJ_NAME} ${BLD_TYPE} ${src} )
set( libs ${QT_LIBRARIES} )
if ( USE_OPENCV )
    set( libs ${libs} ${OpenCV_LIBS} )
endif ( USE_OPENCV )

if ( WIN32 )
    include( "./win32_opencv.cmake" )
    set( libs ${libs} videoInput )
endif ( WIN32 )



target_link_libraries( ${PROJ_NAME} ${libs} qtwebapp ${QT_LIBRARIES} )
install( TARGETS ${PROJ_NAME} DESTINATION "./" )
install( FILES "./etc/Demo2.ini" DESTINATION "./" )

