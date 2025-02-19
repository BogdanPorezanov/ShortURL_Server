cmake_minimum_required (VERSION 3.20)

project ("ShortURL_Server")

file(GLOB 
		CPPS "src/*.cpp"
		CS "src/*.c")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable (${PROJECT_NAME} ${CPPS} ${CS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ShortURL_Server PROPERTY CXX_STANDARD 17)
endif()

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME Boost
  VERSION 1.84.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  URL_HASH SHA256=2e64e5d79a738d0fa6fb546c6e5c2bd28f88d268a2a080546f74e5ff98f29d0e
  OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_INCLUDE_LIBRARIES asio" "BOOST_INCLUDE_LIBRARIES beast" # Note the escapes!
)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::asio)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::beast)