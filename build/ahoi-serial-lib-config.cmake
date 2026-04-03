########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(ahoi-serial-lib_FIND_QUIETLY)
    set(ahoi-serial-lib_MESSAGE_MODE VERBOSE)
else()
    set(ahoi-serial-lib_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/ahoi-serial-libTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${ahoi-serial-lib_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(ahoi-serial-lib_VERSION_STRING "1.0.0")
set(ahoi-serial-lib_INCLUDE_DIRS ${ahoi-serial-lib_INCLUDE_DIRS_RELEASE} )
set(ahoi-serial-lib_INCLUDE_DIR ${ahoi-serial-lib_INCLUDE_DIRS_RELEASE} )
set(ahoi-serial-lib_LIBRARIES ${ahoi-serial-lib_LIBRARIES_RELEASE} )
set(ahoi-serial-lib_DEFINITIONS ${ahoi-serial-lib_DEFINITIONS_RELEASE} )


# Definition of extra CMake variables from cmake_extra_variables


# Only the last installed configuration BUILD_MODULES are included to avoid the collision
foreach(_BUILD_MODULE ${ahoi-serial-lib_BUILD_MODULES_PATHS_RELEASE} )
    message(${ahoi-serial-lib_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


