include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # MSVC: match runtime
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# One test executable per module: a module that does not compile
# does not block the others.
function(workbench_add_test module)
    add_executable(${module}_test ${module}_test.cpp)
    target_link_libraries(${module}_test PRIVATE workbench::workbench GTest::gtest_main)
    target_compile_options(${module}_test PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)
    gtest_discover_tests(${module}_test)
endfunction()

workbench_add_test(string_view)
