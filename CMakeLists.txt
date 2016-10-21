cmake_minimum_required(VERSION 3.1)

project(Moneyphant CXX) # Project name and programming languages (CXX = C++).

option(SQLCIPHER "Build with sqlcipher" Off)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

# We need the path to the directory with the CMakeLists.txt of libmr.
# You don't need to build libmr yourself!
set(LIBMR_DIR ${CMAKE_SOURCE_DIR}/../libmr)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LIBMR_DIR}/cmake)

# If you want to build libmr including classes that use Qt classes you have to make sure cmake can find a recent Qt version.
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/local/Cellar/qt5/5.7.0)


###
### General Build Stuff
###
# Throw error if we are not building out of source.
include(RequireOutOfSourceBuild)

# Build type
if (NOT CMAKE_BUILD_TYPE)
	# Default to Release.
	set(CMAKE_BUILD_TYPE "Release")
	message(STATUS "No build type selected, default to Release. You can choose debug build by appending -DCMAKE_BUILD_TYPE=Debug to the cmake command.")
endif()

# Put binaries in bin/ and lib/ subfolders instead of CMAKE_BINARY_DIR root.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


###
### Qt5
###
# Qt5 documentation on using CMake: http://doc.qt.io/qt-5/cmake-manual.html

# Add Homebrew default install directory of qt5 formula.
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/local/Cellar/qt5/5.7.0)
# Find includes in corresponding build directories.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically for sources.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to run rcc automatically for sources.
set(CMAKE_AUTORCC ON)

find_package(Qt5Core 5.6.0 REQUIRED)
find_package(Qt5Widgets 5.6.0 REQUIRED)
find_package(Qt5PrintSupport 5.6.0 REQUIRED)


###
### Sources
###
file(GLOB_RECURSE src_h src/*.h)
file(GLOB_RECURSE src_cpp src/*.cpp)
file(GLOB_RECURSE src_third_party third_party/*.cpp)

# ui files
file(GLOB_RECURSE src_designer src/*.ui)
qt5_wrap_ui(src_ui ${src_designer})

# resource files
set(src_rcc graphics/graphics.qrc sql/evolutions.qrc)

set(src_all ${src_h} ${src_cpp} ${src_ui} ${src_third_party} ${src_rcc})


###
### Targets
###
# Build libmr instead of depending on a existing build
add_subdirectory(${LIBMR_DIR}/ ${CMAKE_BINARY_DIR}/libmr)

# Moneyphant GUI application

# Resources
add_executable(moneyphant WIN32 MACOSX_BUNDLE ${src_all})
set_target_properties(moneyphant PROPERTIES CXX_STANDARD 14)


###
### Dependencies
###
# libmr
target_include_directories(moneyphant PUBLIC ${LIBMR_DIR}/include)
target_link_libraries(moneyphant libmr)

# third party code
target_include_directories(moneyphant PUBLIC third_party)

# sqlpp11
find_package(Sqlpp11 REQUIRED)
message(STATUS "SQLPP11_INCLUDE_DIRS=${SQLPP11_INCLUDE_DIRS}")
message(STATUS "SQLPP11_INCLUDE_DIRS=${SQLPP11_LIBRARIES}")
target_include_directories(moneyphant PUBLIC ${SQLPP11_INCLUDE_DIRS})
target_link_libraries(moneyphant ${SQLPP11_LIBRARIES})

# sqlite3
if (SQLCIPHER)
	find_package(SQLCipher REQUIRED)
	set(SQLITE3_LIBRARIES "${SQLCIPHER_LIBRARIES}")
	set(SQLITE3_INCLUDE_DIRS "${SQLCIPHER_INCLUDE_DIR}")
else()
	find_package(Sqlite3 REQUIRED)
endif()
target_link_libraries(moneyphant ${SQLITE3_LIBRARIES})

# Qt
target_link_libraries(moneyphant Qt5::Core)
target_link_libraries(moneyphant Qt5::Widgets)
target_link_libraries(moneyphant Qt5::PrintSupport)

# Configure version.cpp to contain version information based on the repository history
add_custom_target(version_cpp ALL COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${PROJECT_SOURCE_DIR} -P ${LIBMR_DIR}/cmake/ConfigureVersionCppFromGit.cmake)
add_dependencies(moneyphant version_cpp)
