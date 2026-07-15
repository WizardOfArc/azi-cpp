# Create the target
add_executable(wizard_game) #insert WIN32 to not show terminal
target_sources(wizard_game PRIVATE 
   main.cpp
   )

target_compile_features(wizard_game PRIVATE cxx_std_23)

target_compile_options(wizard_game PRIVATE
    -Werror          # Treat al warnings as errors
    -Wall            # Enable all most common warnings
    -Wextra
    -Wconversion
    -Wshadow
)

target_link_libraries(wizard_game PRIVATE wizard_game_deps)
