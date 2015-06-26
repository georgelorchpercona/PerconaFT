
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(snappy_srcs
    snappy
    snappy-c
    snappy-sinksource
    snappy-stubs-internal
    )

## make the shared library for tests
add_library(snappy SHARED ${snappy_srcs})
add_dependencies(snappy install_tdb_h)

## make the real library, it's going to go into libtokudb.so so it needs
## to be PIC
add_definitions("-Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -fPIC")
add_library(snappy_static STATIC ${snappy_srcs})
add_space_separated_property(TARGET snappy_static COMPILE_FLAGS "-fvisibility=hidden -fPIC")
add_dependencies(snappy_static install_tdb_h)

maybe_add_gcov_to_libraries(snappy snappy_static)
