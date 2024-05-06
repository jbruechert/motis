option(NO_BUILDCACHE "Disable build caching using buildcache" Off)

# PDB debug information is not supported by buildcache.
# Store debug info in the object files.
if (DEFINED ENV{GITHUB_ACTIONS})
    string(REPLACE "/Zi" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "/Zi" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
else ()
    string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif ()

set(buildcache-bin ${CMAKE_CURRENT_BINARY_DIR}/buildcache/bin/buildcache)
get_property(rule-launch-set GLOBAL PROPERTY RULE_LAUNCH_COMPILE SET)
if (NO_BUILDCACHE)
    message(STATUS "NO_BUILDCACHE set, buildcache disabled")
elseif (rule-launch-set)
    message(STATUS "Global property RULE_LAUNCH_COMPILE already set - skipping buildcache")
else ()
    find_program(buildcache_program buildcache HINTS ${CMAKE_CURRENT_BINARY_DIR}/buildcache/bin)
    if (buildcache_program)
        message(STATUS "Using buildcache: ${buildcache_program}")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${buildcache_program}")
    else ()
        message(STATUS "buildcache not found - downloading")
        if (APPLE)
            set(buildcache-archive "buildcache-macos.zip")
        elseif (UNIX)
            set(buildcache-archive "buildcache-linux.tar.gz")
        elseif (WIN32)
            set(buildcache-archive "buildcache-windows.zip")
        else ()
            message(FATAL "Error: NO_BUILDCACHE was not set but buildcache was not in path and system OS detection failed")
        endif ()

        set(buildcache-url "https://github.com/mbitsnbites/buildcache/releases/download/v0.28.2/${buildcache-archive}")
        message(STATUS "Downloading buildcache binary from ${buildcache-url}")
        file(DOWNLOAD "${buildcache-url}" ${CMAKE_CURRENT_BINARY_DIR}/${buildcache-archive})
        execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_BINARY_DIR}/${buildcache-archive}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        message(STATUS "using buildcache: ${buildcache-bin}")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${buildcache-bin})
    endif ()
endif ()