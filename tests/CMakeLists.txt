# tests
enable_testing()

# executable test 1
add_executable(test1 test1.cpp)
target_link_libraries(test1 PRIVATE ${PROJECT_NAME})
add_test(NAME test1 COMMAND test1)

# executable test 2
add_executable(test2 test2.cpp)
target_link_libraries(test2 PRIVATE ${PROJECT_NAME})
add_test(NAME test2 COMMAND test2)