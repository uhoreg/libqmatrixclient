cmake_minimum_required(VERSION 3.1)

project(qmatrixclient)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cmake/FindOlm.cmake)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build" FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

# Setup command line parameters for the compiler and linker
CHECK_CXX_COMPILER_FLAG("-Wall" WALL_FLAG_SUPPORTED)
if ( WALL_FLAG_SUPPORTED AND NOT CMAKE_CXX_FLAGS MATCHES "(^| )-Wall($| )")
    add_compile_options("-Wall")
endif ( )
CHECK_CXX_COMPILER_FLAG("-Wpedantic" PEDANTIC_FLAG_SUPPORTED)
if ( PEDANTIC_FLAG_SUPPORTED AND NOT CMAKE_CXX_FLAGS MATCHES "(^| )pedantic($| )")
    add_compile_options("-Wpedantic")
endif ( )

set(CMAKE_CXX_STANDARD 11)

find_package(Qt5 5.2.1 REQUIRED Network Gui)
get_filename_component(Qt5_Prefix "${Qt5_DIR}/../../../.." ABSOLUTE)
find_package(Olm REQUIRED)

message( STATUS )
message( STATUS "=============================================================================" )
message( STATUS "                       libqmatrixclient Build Information" )
message( STATUS "=============================================================================" )
if (CMAKE_BUILD_TYPE)
    message( STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif(CMAKE_BUILD_TYPE)
message( STATUS "Using compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}" )
message( STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}" )
message( STATUS "Using Qt ${Qt5_VERSION} at ${Qt5_Prefix}" )
if (BUILD_OLM)
    message( STATUS "Using libolm ${Olm_VERSION} sources at ${Olm_Prefix}" )
else (BUILD_OLM)
    message( STATUS "Using libolm ${Olm_VERSION} at ${Olm_Prefix}" )
endif (BUILD_OLM)
message( STATUS "=============================================================================" )
message( STATUS )

if (BUILD_OLM)
    # The whole BUILD_OLM thing is rather a stopgap for now, so no support
    # for updates to the sources tree
    aux_source_directory(${Olm_Prefix}/src olm_SRCS)
    set (olm_SRCS ${olm_SRCS}
        ${Olm_Prefix}/lib/crypto-algorithms/sha256.c
        ${Olm_Prefix}/lib/crypto-algorithms/aes.c
        ${Olm_Prefix}/lib/curve25519-donna/curve25519-donna.c
    )
    set(olm_dep_INCLUDE_DIR ${Olm_Prefix}/lib)
    set(Olm_LIBRARIES olm)
    add_library(${Olm_LIBRARIES} ${olm_SRCS})
    target_include_directories(${Olm_LIBRARIES} PUBLIC
        "${Olm_INCLUDE_DIRS}"
        "${olm_dep_INCLUDE_DIR}"
    )
    target_compile_definitions(${Olm_LIBRARIES} PUBLIC
        "-DOLMLIB_VERSION_MAJOR=${Olm_VERSION_MAJOR}"
        "-DOLMLIB_VERSION_MINOR=${Olm_VERSION_MINOR}"
        "-DOLMLIB_VERSION_PATCH=${Olm_VERSION_PATCH}"
    )
    if (PEDANTIC_FLAG_SUPPORTED)
        target_compile_options(${Olm_LIBRARIES} PRIVATE "-Wno-pedantic")
    endif (PEDANTIC_FLAG_SUPPORTED)

    CHECK_C_COMPILER_FLAG("-std=c99" C99_FLAG_SUPPORTED)
    if (C99_FLAG_SUPPORTED)
        # Can't use target_compile_options here because it also adds the flag
        # to C++ flags which is improper; and set(CMAKE_C_STANDARD 99) for some
        # reason just does nothing.
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
        CHECK_CXX_COMPILER_FLAG("-Werror" WERROR_FLAG_SUPPORTED)
        if (WERROR_FLAG_SUPPORTED)
            target_compile_options(${Olm_LIBRARIES} PRIVATE "-Werror")
        endif (WERROR_FLAG_SUPPORTED)
    endif (C99_FLAG_SUPPORTED)
endif (BUILD_OLM)

# Set up source files
set(libqmatrixclient_SRCS
   connectiondata.cpp
   connection.cpp
   logging.cpp
   room.cpp
   user.cpp
   settings.cpp
   encryptionmanager.cpp
   events/event.cpp
   events/roommessageevent.cpp
   events/roomnameevent.cpp
   events/roomaliasesevent.cpp
   events/roomcanonicalaliasevent.cpp
   events/roommemberevent.cpp
   events/roomtopicevent.cpp
   events/typingevent.cpp
   events/receiptevent.cpp
   jobs/basejob.cpp
   jobs/checkauthmethods.cpp
   jobs/passwordlogin.cpp
   jobs/sendeventjob.cpp
   jobs/setroomstatejob.cpp
   jobs/postreceiptjob.cpp
   jobs/joinroomjob.cpp
   jobs/leaveroomjob.cpp
   jobs/roommessagesjob.cpp
   jobs/syncjob.cpp
   jobs/mediathumbnailjob.cpp
   jobs/logoutjob.cpp
   jobs/uploaddevicekeys.cpp
   events/encryptedevent.cpp
)

aux_source_directory(jobs/generated libqmatrixclient_job_SRCS)

set(example_SRCS examples/qmc-example.cpp)

add_library(qmatrixclient ${libqmatrixclient_SRCS} ${libqmatrixclient_job_SRCS})
set_property(TARGET qmatrixclient PROPERTY VERSION "0.0.0")
set_property(TARGET qmatrixclient PROPERTY SOVERSION 0 )

target_include_directories(qmatrixclient SYSTEM PRIVATE "${Olm_INCLUDE_DIRS}")
target_link_libraries(qmatrixclient Qt5::Core Qt5::Network Qt5::Gui ${Olm_LIBRARIES})

add_executable(qmc-example ${example_SRCS})
target_link_libraries(qmc-example Qt5::Core qmatrixclient)

if (WIN32)
    install (FILES mime/packages/freedesktop.org.xml
             DESTINATION mime/packages)
endif (WIN32)
