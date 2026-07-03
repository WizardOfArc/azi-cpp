# Create the target
add_executable(azi_game_of_life) #insert WIN32 to not show terminal
target_sources(azi_game_of_life PRIVATE 
   main.cpp
   )

target_compile_features(azi_game_of_life PRIVATE cxx_std_23)

target_compile_options(azi_game_of_life PRIVATE
    -Werror          # Treat al warnings as errors
    -Wall            # Enable all most common warnings
    -Wextra
    -Wconversion
    -Wshadow
)

target_link_libraries(azi_game_of_life PRIVATE game_of_life_deps)
