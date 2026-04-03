# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/ahoi-serial-lib-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${ahoi-serial-lib_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${ahoi-serial-lib_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET ahoi-serial-lib::ahoi-serial-lib)
    add_library(ahoi-serial-lib::ahoi-serial-lib INTERFACE IMPORTED)
    message(${ahoi-serial-lib_MESSAGE_MODE} "Conan: Target declared 'ahoi-serial-lib::ahoi-serial-lib'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/ahoi-serial-lib-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()