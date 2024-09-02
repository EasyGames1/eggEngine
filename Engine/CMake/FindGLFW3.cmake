set(GLFW3_HEADER_SEARCH_DIRS
        "/usr/include"
        "/usr/local/include"
        "${CMAKE_SOURCE_DIR}/include"
        "C:/Program Files (x86)/glfw/include"
        "C:/Program Files (x86)/glfw3/include"
        "${CMAKE_PREFIX_PATH}/glfw/include"
        "${CMAKE_PREFIX_PATH}/glfw3/include"
)

set(GLFW_LIBRARY_SEARCH_DIRS
        "/usr/lib"
        "/usr/local/lib"
        "${CMAKE_SOURCE_DIR}/lib"
        "C:/Program Files (x86)/glfw/lib-msvc110"
        "C:/Program Files (x86)/glfw3/lib-msvc110"
        "${CMAKE_PREFIX_PATH}/glfw/lib"
        "${CMAKE_PREFIX_PATH}/glfw3/lib"
)

set(GLFW3_ENV_ROOT $ENV{GLFW3_ROOT})
if (NOT GLFW3_ROOT AND GLFW3_ENV_ROOT)
    set(GLFW3_ROOT ${GLFW3_ENV_ROOT})
endif ()

if (GLFW3_ROOT)
    list(INSERT GLFW3_HEADER_SEARCH_DIRS 0 "${GLFW3_ROOT}/include")
    list(INSERT GLFW_LIBRARY_SEARCH_DIRS 0 "${GLFW3_ROOT}/lib")
endif ()

find_path(GLFW3_INCLUDE_DIR "GLFW/glfw3.h"
        PATHS ${GLFW3_HEADER_SEARCH_DIRS})
if (GLFW3_INCLUDE_DIR)
    set(GLFW3_INCLUDE_DIR ${GLFW3_INCLUDE_DIR})
endif ()

find_library(GLFW3_LIBRARY NAMES glfw3 glfw
        PATHS ${GLFW_LIBRARY_SEARCH_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(glfw3 DEFAULT_MSG GLFW3_INCLUDE_DIR GLFW3_LIBRARY)
if (glfw3_FOUND)
    add_library(glfw3::glfw3 UNKNOWN IMPORTED)
    set_target_properties(glfw3::glfw3 PROPERTIES
            IMPORTED_LOCATION ${GLFW3_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${GLFW3_INCLUDE_DIR})

endif ()