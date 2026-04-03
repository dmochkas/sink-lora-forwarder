# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/ascon-c-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${ascon-c_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${ascon-c_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET ascon-c::ascon-c)
    add_library(ascon-c::ascon-c INTERFACE IMPORTED)
    message(${ascon-c_MESSAGE_MODE} "Conan: Target declared 'ascon-c::ascon-c'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/ascon-c-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()