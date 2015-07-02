cmake_minimum_required(VERSION 2.8)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/lzhamcomp
  ${CMAKE_CURRENT_SOURCE_DIR}/lzhamdecomp
)

set(lzham_srcs
  lzhamdecomp/lzham_assert.cpp
  lzhamdecomp/lzham_checksum.cpp
  lzhamdecomp/lzham_huffman_codes.cpp
  lzhamdecomp/lzham_lzdecompbase.cpp
  lzhamdecomp/lzham_lzdecomp.cpp
  lzhamdecomp/lzham_mem.cpp
  lzhamdecomp/lzham_platform.cpp
  lzhamdecomp/lzham_prefix_coding.cpp
  lzhamdecomp/lzham_symbol_codec.cpp
  lzhamdecomp/lzham_timer.cpp
  lzhamdecomp/lzham_vector.cpp
	lzhamcomp/lzham_lzbase.cpp
	lzhamcomp/lzham_lzcomp.cpp
	lzhamcomp/lzham_lzcomp_internal.cpp
	lzhamcomp/lzham_lzcomp_state.cpp
	lzhamcomp/lzham_match_accel.cpp
	lzhamcomp/lzham_pthreads_threading.cpp
  lzhamlib/lzham_lib.cpp
)

add_definitions("-fno-strict-aliasing -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64 -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -fvisibility=hidden -fPIC")

## make the library, it's going to go into libtokudb.so so it needs
## to be PIC
add_library(lzham STATIC ${lzham_srcs})
set_target_properties(lzham PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(
  TARGETS lzham
  DESTINATION lib
)
install(
  FILES include/lzham.h
  DESTINATION include
)
