cmake_minimum_required(VERSION 3.28)

project(AziTree VERSION 1.0.0 LANGUAGES CXX)

# Create the target
add_executable(crann)
target_sources(crann PRIVATE 
   main.cpp
   )

# Set C++23 standard with proper target properties
target_compile_features(crann PRIVATE cxx_std_23)

target_compile_options(crann PRIVATE
    -Werror          # Treat al warnings as errors
    -Wall            # Enable all most common warnings
    -Wextra
    -Wconversion
    -Wshadow
)

# Ensure the standard is required, not optional
set_target_properties(crann PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
