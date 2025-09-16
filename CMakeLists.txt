cmake_minimum_required(VERSION 3.10)
project(UIFramework VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Check if Catch2 header is available
set(CATCH2_HEADER "${CMAKE_SOURCE_DIR}/lib/catch2/catch.hpp")
if(EXISTS ${CATCH2_HEADER})
    set(CATCH2_AVAILABLE TRUE)
    message(STATUS "Catch2 header found at ${CATCH2_HEADER}")
else()
    set(CATCH2_AVAILABLE FALSE)
    message(STATUS "Catch2 header not found - tests disabled")
endif()


# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add Catch2 include directory if available
if(CATCH2_AVAILABLE)
    include_directories(${CMAKE_SOURCE_DIR}/lib)
endif()

# UI Framework source files
set(UI_SOURCES
    src/Interface/ui/UIComponent.cpp
    src/Interface/ui/UIButton.cpp
    src/Interface/ui/UICard.cpp
    src/Interface/ui/UILabel.cpp
    src/Interface/ui/UITextInput.cpp
    src/Interface/ui/UIProgressBar.cpp
    src/Interface/ui/UIManager.cpp
    src/Interface/ui/UIContainer.cpp
    src/Interface/ui/UILayoutContainer.cpp
    src/Interface/ui/Layout.cpp
    src/Interface/ui/UITooltip.cpp
    src/Interface/ui/UIDataBinding.cpp
    src/Interface/ui/UISceneManager.cpp
    src/Interface/ui/FocusableButton.cpp
    src/Interface/ui/FocusableButtonTestWindow.cpp
    src/Systems/SDLManager.cpp
)

# Create static library
add_library(UIFramework STATIC ${UI_SOURCES})

# Create shared library option
add_library(UIFramework_shared SHARED ${UI_SOURCES})

# Set library properties
set_target_properties(UIFramework_shared PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    OUTPUT_NAME UIFramework
)

# Link libraries
target_link_libraries(UIFramework SDL2::SDL2 SDL2_ttf::SDL2_ttf)
target_link_libraries(UIFramework_shared SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# Install targets
install(TARGETS UIFramework UIFramework_shared
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY include/ DESTINATION include/UIFramework)

# Add tests if Catch2 is available and tests are enabled
if(BUILD_UI_FRAMEWORK_TESTS AND CATCH2_AVAILABLE)
    enable_testing()

    # Test source files
    set(TEST_SOURCES
        tests/main.cpp
        tests/test_ui_manager.cpp
        tests/test_ui_container.cpp
        tests/test_ui_integration.cpp
        tests/test_ui_layout.cpp
        tests/test_ui_layouts.cpp
        tests/test_ui_focus.cpp
        tests/test_ui_label.cpp
        tests/test_ui_textinput.cpp
        tests/test_ui_scenemanager.cpp
    )

    # Create test executable
    add_executable(UIFrameworkTests ${TEST_SOURCES})
    target_link_libraries(UIFrameworkTests PRIVATE UIFramework_shared)

    # Add compile flags for threading support (required for threading tests)
    target_compile_options(UIFrameworkTests PRIVATE -pthread)
    target_link_options(UIFrameworkTests PRIVATE -pthread)

    # Create custom test target
    add_custom_target(test_run
        COMMAND UIFrameworkTests
        DEPENDS UIFrameworkTests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running Catch2 tests"
    )

    message(STATUS "Catch2 tests enabled - use 'make test_run' to run tests")
else()
    if(NOT BUILD_UI_FRAMEWORK_TESTS)
        message(STATUS "UI Framework tests disabled")
    else()
        message(STATUS "Catch2 not available - tests disabled")
        message(STATUS "To enable tests, ensure ${CMAKE_SOURCE_DIR}/lib/catch2/catch.hpp exists")
    endif()
endif()

# Disable examples and tests when used as submodule to avoid Card class dependency issues
option(BUILD_UI_FRAMEWORK_EXAMPLES "Build UI Framework examples" OFF)
option(BUILD_UI_FRAMEWORK_TESTS "Build UI Framework tests" OFF)

# Only build examples and tests if specifically requested
if(BUILD_UI_FRAMEWORK_EXAMPLES)
    # Add UILabel example
    add_executable(LabelExample examples/label_example.cpp)
    target_link_libraries(LabelExample PRIVATE UIFramework_shared)

    # Add UITextInput example
    add_executable(TextInputExample examples/textinput_example.cpp)
    target_link_libraries(TextInputExample PRIVATE UIFramework_shared)

    # Add Layout examples
    add_executable(LayoutExamples examples/layout_examples.cpp)
    target_link_libraries(LayoutExamples PRIVATE UIFramework_shared)

    # Add Data Binding example
    add_executable(DataBindingExample examples/databinding_example.cpp)
    target_link_libraries(DataBindingExample PRIVATE UIFramework_shared)

    # Add Scene Manager example
    add_executable(SceneExample examples/scene_example.cpp)
    target_link_libraries(SceneExample PRIVATE UIFramework_shared)

    # Add Progress Bar example
    add_executable(ProgressBarExample examples/progressbar_example.cpp)
    target_link_libraries(ProgressBarExample PRIVATE UIFramework_shared)
endif()

if(BUILD_UI_FRAMEWORK_TESTS)
    # FocusableButton Test
    add_executable(FocusableButtonTest testing/testing_focusable_button.cpp)
    target_link_libraries(FocusableButtonTest PRIVATE UIFramework_shared)
endif()