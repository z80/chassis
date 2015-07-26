
set( STRM_PATH "./src" )
include_directories( ${STRM_PATH} )
aux_source_directory( ${STRM_PATH} src )

add_library( video_strmbase STATIC ${src} )
find_library( strmiids NAMES "strmiids.lib" "strmiids.o" "strmiids.so.o" PATHS "${PL_SDK}/lib" )
find_library( winmm    NAMES "winmm.lib"    "winmm.o"    "winmm.so.o"    PATHS "${PL_SDK}/lib" )
target_link_libraries( video_strmbase ${strmiids} ${winmm} )
set_property( TARGET video_strmbase PROPERTY COMPILE_DEFINITIONS _LIB WIN32 )
