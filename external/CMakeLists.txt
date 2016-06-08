
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    range-v3
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/range-v3
    GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )

# Expose required variable (RANGE_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(range-v3 source_dir)
set(RANGE_INCLUDE_DIR ${source_dir}/include CACHE INTERNAL "Path to include folder for range-v3")
