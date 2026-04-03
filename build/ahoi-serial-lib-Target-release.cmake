# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(ahoi-serial-lib_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(ahoi-serial-lib_FRAMEWORKS_FOUND_RELEASE "${ahoi-serial-lib_FRAMEWORKS_RELEASE}" "${ahoi-serial-lib_FRAMEWORK_DIRS_RELEASE}")

set(ahoi-serial-lib_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET ahoi-serial-lib_DEPS_TARGET)
    add_library(ahoi-serial-lib_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET ahoi-serial-lib_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${ahoi-serial-lib_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${ahoi-serial-lib_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:ascon-c::ascon-c>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### ahoi-serial-lib_DEPS_TARGET to all of them
conan_package_library_targets("${ahoi-serial-lib_LIBS_RELEASE}"    # libraries
                              "${ahoi-serial-lib_LIB_DIRS_RELEASE}" # package_libdir
                              "${ahoi-serial-lib_BIN_DIRS_RELEASE}" # package_bindir
                              "${ahoi-serial-lib_LIBRARY_TYPE_RELEASE}"
                              "${ahoi-serial-lib_IS_HOST_WINDOWS_RELEASE}"
                              ahoi-serial-lib_DEPS_TARGET
                              ahoi-serial-lib_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "ahoi-serial-lib"    # package_name
                              "${ahoi-serial-lib_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${ahoi-serial-lib_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET ahoi-serial-lib::ahoi-serial-lib
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${ahoi-serial-lib_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${ahoi-serial-lib_LIBRARIES_TARGETS}>
                 )

    if("${ahoi-serial-lib_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET ahoi-serial-lib::ahoi-serial-lib
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     ahoi-serial-lib_DEPS_TARGET)
    endif()

    set_property(TARGET ahoi-serial-lib::ahoi-serial-lib
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${ahoi-serial-lib_LINKER_FLAGS_RELEASE}>)
    set_property(TARGET ahoi-serial-lib::ahoi-serial-lib
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${ahoi-serial-lib_INCLUDE_DIRS_RELEASE}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET ahoi-serial-lib::ahoi-serial-lib
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${ahoi-serial-lib_LIB_DIRS_RELEASE}>)
    set_property(TARGET ahoi-serial-lib::ahoi-serial-lib
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${ahoi-serial-lib_COMPILE_DEFINITIONS_RELEASE}>)
    set_property(TARGET ahoi-serial-lib::ahoi-serial-lib
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${ahoi-serial-lib_COMPILE_OPTIONS_RELEASE}>)

########## For the modules (FindXXX)
set(ahoi-serial-lib_LIBRARIES_RELEASE ahoi-serial-lib::ahoi-serial-lib)
