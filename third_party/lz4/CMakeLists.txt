include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(lz4_dir ./lib/)
set(lz4_srcs
    ${lz4_dir}lz4.c
    ${lz4_dir}lz4hc.c
    ${lz4_dir}lz4.h
    ${lz4_dir}lz4hc.h
    ${lz4_dir}lz4frame.c
    ${lz4_dir}lz4frame.h
    ${lz4_dir}xxhash.c
    )

add_definitions("-Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -fvisibility=hidden -fPIC")

## make the library, it's going to go into libtokudb.so so it needs
## to be PIC
add_library(lz4 STATIC ${lz4_srcs})
set_target_properties(lz4 PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(
  TARGETS lz4
  DESTINATION lib
  )
install(FILES
  ${lz4_dir}/lz4.h
  ${lz4_dir}/lz4hc.h
  ${lz4_dir}/lz4frame.h
  DESTINATION include
  )
