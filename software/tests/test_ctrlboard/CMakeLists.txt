
find_package( Qt4 )
include( ${QT_USE_FILE} )

include_directories( "./src"
                     ${ROOT}/ctrlboard/src
                     ${CMAKE_CURRENT_BINARY_DIR} )
if ( WIN32 )
    include_directories( "${ROOT}/ctrlboard/src/win32" )
else ( WIN32 )
    include_directories( "${ROOT}/ctrlboard/src/linux" )
endif ( WIN32 )


aux_source_directory( "./src" src )

add_executable( test_ctrlboard ${src} )
target_link_libraries( test_ctrlboard ctrlboard ${QT_LIBRARIES} )
install( TARGETS test_ctrlboard DESTINATION ./ )



