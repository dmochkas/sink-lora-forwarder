message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(ahoi-serial-lib)
find_package(zlog)

set(CONANDEPS_LEGACY  ahoi-serial-lib::ahoi-serial-lib  zlog::zlog )