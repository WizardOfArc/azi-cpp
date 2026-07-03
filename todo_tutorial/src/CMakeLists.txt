# Create the target
add_executable(azi_todo WIN32)
target_sources(azi_todo PRIVATE 
   main.cpp
   TaskList.cpp
   )

target_compile_features(azi_todo PRIVATE cxx_std_23)

target_compile_options(azi_todo PRIVATE
    -Werror          # Treat al warnings as errors
    -Wall            # Enable all most common warnings
    -Wextra
    -Wconversion
    -Wshadow
)

target_link_libraries(azi_todo PRIVATE todo_list)
# third party libs
target_link_libraries(azi_todo PRIVATE imgui)
target_link_libraries(azi_todo PRIVATE SQLiteCpp)