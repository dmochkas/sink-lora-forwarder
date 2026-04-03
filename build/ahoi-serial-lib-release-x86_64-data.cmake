########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(ahoi-serial-lib_COMPONENT_NAMES "")
if(DEFINED ahoi-serial-lib_FIND_DEPENDENCY_NAMES)
  list(APPEND ahoi-serial-lib_FIND_DEPENDENCY_NAMES ascon-c)
  list(REMOVE_DUPLICATES ahoi-serial-lib_FIND_DEPENDENCY_NAMES)
else()
  set(ahoi-serial-lib_FIND_DEPENDENCY_NAMES ascon-c)
endif()
set(ascon-c_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(ahoi-serial-lib_PACKAGE_FOLDER_RELEASE "/home/joako/.conan2/p/b/ahoi-68efb783e6fc1/p")
set(ahoi-serial-lib_BUILD_MODULES_PATHS_RELEASE )


set(ahoi-serial-lib_INCLUDE_DIRS_RELEASE "${ahoi-serial-lib_PACKAGE_FOLDER_RELEASE}/include")
set(ahoi-serial-lib_RES_DIRS_RELEASE )
set(ahoi-serial-lib_DEFINITIONS_RELEASE )
set(ahoi-serial-lib_SHARED_LINK_FLAGS_RELEASE )
set(ahoi-serial-lib_EXE_LINK_FLAGS_RELEASE )
set(ahoi-serial-lib_OBJECTS_RELEASE )
set(ahoi-serial-lib_COMPILE_DEFINITIONS_RELEASE )
set(ahoi-serial-lib_COMPILE_OPTIONS_C_RELEASE )
set(ahoi-serial-lib_COMPILE_OPTIONS_CXX_RELEASE )
set(ahoi-serial-lib_LIB_DIRS_RELEASE "${ahoi-serial-lib_PACKAGE_FOLDER_RELEASE}/lib")
set(ahoi-serial-lib_BIN_DIRS_RELEASE "${ahoi-serial-lib_PACKAGE_FOLDER_RELEASE}/bin")
set(ahoi-serial-lib_LIBRARY_TYPE_RELEASE SHARED)
set(ahoi-serial-lib_IS_HOST_WINDOWS_RELEASE 0)
set(ahoi-serial-lib_LIBS_RELEASE ahoi-serial-lib zlog zlog)
set(ahoi-serial-lib_SYSTEM_LIBS_RELEASE )
set(ahoi-serial-lib_FRAMEWORK_DIRS_RELEASE )
set(ahoi-serial-lib_FRAMEWORKS_RELEASE )
set(ahoi-serial-lib_BUILD_DIRS_RELEASE )
set(ahoi-serial-lib_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(ahoi-serial-lib_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${ahoi-serial-lib_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${ahoi-serial-lib_COMPILE_OPTIONS_C_RELEASE}>")
set(ahoi-serial-lib_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${ahoi-serial-lib_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${ahoi-serial-lib_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${ahoi-serial-lib_EXE_LINK_FLAGS_RELEASE}>")


set(ahoi-serial-lib_COMPONENTS_RELEASE )