################################################################################
project( "twist-your-pollutants" C CXX )

cmake_minimum_required( VERSION 2.8.3 )

set( CMAKE_ECLIPSE_VERSION 3.8 )
set( _ECLIPSE_VERSION 3.8 )

set( CMAKE_CXX_COMPILER_ARG1 -std=c++11 ) ### for Eclipse's discovery extra arguments


#add_definitions( -O0 -ggdb -unroll-loops -std=c++14 -Wall -pedantic -mmmx -msse -msse2 -msse3 -s -pipe -Wno-sign-compare -march=native )
#add_definitions( -O2 -ggdb -unroll-loops -std=c++14 -Wall -pedantic -mmmx -msse -msse2 -msse3 -s -pipe -Wno-sign-compare -march=native )
add_definitions( -O3 -unroll-loops -std=c++14 -Wall -pedantic -mmmx -msse -msse2 -msse3 -s -pipe -Wno-sign-compare -march=native )

################################################################################

#find_package( Boost REQUIRED COMPONENTS spirit )
find_package( Boost REQUIRED )
find_package( OpenSSL REQUIRED )

################################################################################

include_directories(
    ${Boost_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

################################################################################


add_executable( main
    main.cpp
)

target_link_libraries( main ${OPENSSL_LIBRARIES} )

################################################################################
