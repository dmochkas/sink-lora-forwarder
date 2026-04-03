########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(ascon-c_COMPONENT_NAMES "")
if(DEFINED ascon-c_FIND_DEPENDENCY_NAMES)
  list(APPEND ascon-c_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES ascon-c_FIND_DEPENDENCY_NAMES)
else()
  set(ascon-c_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(ascon-c_PACKAGE_FOLDER_RELEASE "/home/joako/.conan2/p/b/asconef8fc08ff9594/p")
set(ascon-c_BUILD_MODULES_PATHS_RELEASE )


set(ascon-c_INCLUDE_DIRS_RELEASE )
set(ascon-c_RES_DIRS_RELEASE )
set(ascon-c_DEFINITIONS_RELEASE )
set(ascon-c_SHARED_LINK_FLAGS_RELEASE )
set(ascon-c_EXE_LINK_FLAGS_RELEASE )
set(ascon-c_OBJECTS_RELEASE )
set(ascon-c_COMPILE_DEFINITIONS_RELEASE )
set(ascon-c_COMPILE_OPTIONS_C_RELEASE )
set(ascon-c_COMPILE_OPTIONS_CXX_RELEASE )
set(ascon-c_LIB_DIRS_RELEASE "${ascon-c_PACKAGE_FOLDER_RELEASE}/lib")
set(ascon-c_BIN_DIRS_RELEASE "${ascon-c_PACKAGE_FOLDER_RELEASE}/bin")
set(ascon-c_LIBRARY_TYPE_RELEASE SHARED)
set(ascon-c_IS_HOST_WINDOWS_RELEASE 0)
set(ascon-c_LIBS_RELEASE )
set(ascon-c_SYSTEM_LIBS_RELEASE )
set(ascon-c_FRAMEWORK_DIRS_RELEASE )
set(ascon-c_FRAMEWORKS_RELEASE )
set(ascon-c_BUILD_DIRS_RELEASE )
set(ascon-c_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(ascon-c_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${ascon-c_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${ascon-c_COMPILE_OPTIONS_C_RELEASE}>")
set(ascon-c_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${ascon-c_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${ascon-c_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${ascon-c_EXE_LINK_FLAGS_RELEASE}>")


set(ascon-c_COMPONENTS_RELEASE )