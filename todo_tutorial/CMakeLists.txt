cmake_minimum_required(VERSION 3.28)
project(AziTODO VERSION 1.0.0 LANGUAGES CXX)

# Ensure the standard is required, not optional
set(CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

# --- add CPM ----
IF (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake")
  file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.3/CPM.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
    EXPECTED_HASH SHA256=:a609e875fd532b067174250f6abbc3dac22fe2d64869783fb1e80bda1625c844
    STATUS status
    SHOW_PROGRESS
  )    
  list(GET status 0 status_code)
  if(NOT status_code EQUAL 0)
     message(FATAL_ERROR "Could not download CPM.cmake")
  endif()
endif()

include(cmake/CPM.cmake)

# ---- add TPLs ----
# SDL3
CPMAddPackage(
  NAME SDL3
  GITHUB_REPOSITORY "libsdl-org/SDL"
  GIT_TAG "main"
)

# ImGUI
CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY "ocornut/imgui"
  GIT_TAG "docking"
)

# Set Up ImGui OpenGL3 and SDL3 backends
add_library(imgui STATIC
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
 
CPMAddPackage(
  NAME SQLiteCpp
  GITHUB_REPOSITORY SRombauts/SQLiteCpp
  GIT_TAG 3.3.3
  OPTIONS
    "SQLITECPP_INTERNAL_SQLITE ON"
    "SQLITECPP_ENABLE_TESTS OFF"
)

# Find OpenGL
find_package(OpenGL REQUIRED)

target_include_directories(imgui PUBLIC
    ${SDL3_INCLUDE_DIRS}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC SDL3::SDL3 OpenGL::GL)
# ------- Our Code ----------

add_subdirectory(src bin)
add_subdirectory(include)

# -------- Copy Resources ---
file(COPY "res/" DESTINATION "${CMAKE_BINARY_DIR}/bin")


# ------ Testing
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
