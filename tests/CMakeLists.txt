# tests
enable_testing()

# executable test 1
add_executable(mongoose_test1 test1.cpp)
target_link_libraries(mongoose_test1 PRIVATE ${PROJECT_NAME})
add_test(NAME test1 COMMAND mongoose_test1)

# executable test 2
add_executable(mongoose_test2 test1.cpp)
target_link_libraries(mongoose_test2 PRIVATE ${PROJECT_NAME})
add_test(NAME test2 COMMAND mongoose_test2)