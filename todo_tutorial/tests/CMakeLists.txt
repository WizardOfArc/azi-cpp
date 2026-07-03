# ------ Testing
enable_testing()
add_executable(tests 
   tests.cpp
)
target_compile_features(tests PRIVATE cxx_std_23)

add_test(NAME UnitTests COMMAND tests)

target_link_libraries(tests PRIVATE todo_list)