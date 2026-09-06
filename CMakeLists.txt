cmake_minimum_required(VERSION 3.7)
project(Filters)

include(filters.cmake)

add_definitions(-fpic)

SET(LIBXCF_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/dec_xcf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/xcf_decode.c
        ${CMAKE_CURRENT_SOURCE_DIR}/xcf_stubs.c
)

# The xcftools core only: xcf-general, pixels, flatten, flatspec, enums,
# palette, scaletab, table, utils and nlsini. io-unix.c is deliberately left
# out - it is the mmap/open/unzip layer, and the filter feeds the document
# straight from the packet instead.
SET(LIBXCF_LIBS
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libxcftools.a
)

SET(LIBXCF_INC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_filter(libxcf
        "${LIBXCF_SRC}"
        "${LIBXCF_LIBS}"
        []
        ""
        "${LIBXCF_INC}"
        ""
        "1")

# xcftools reaches for the GNU extensions and for endian.h without including
# it, the same relaxations build_thirdparties.sh applies when building the
# converters. Only the file that includes xcftools.h is concerned.
set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/xcf_decode.c
        PROPERTIES COMPILE_FLAGS "-D_GNU_SOURCE -include endian.h")
