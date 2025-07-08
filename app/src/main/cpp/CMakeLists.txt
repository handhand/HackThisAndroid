
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("SimpleRasp")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
add_library(${CMAKE_PROJECT_NAME} SHARED
    # List C/C++ source files with relative paths to this CMakeLists.txt.
        SimpleRasp/simple_rasp.cpp
        SimpleRasp/frida_detection.cpp
        SimpleRasp/root_detection.cpp
        SimpleRasp/frida_function_hook_detection.cpp
        SimpleRasp/emulator_detection.cpp
        SimpleRasp/dlfcn/local_dlfcn.c
        SimpleRasp/lib_patch_detection.c
        SimpleRasp/create_thread.cpp
)


# Need to add this for using "clone"
# -D_GNU_SOURCE is a compiler flag that defines the macro GNU_SOURCE.
# This macro enables GNU-specific extensions in the standard C library and system headers.
# When this macro is defined, additional features and functions provided by the GNU C library (glibc) become available,
# which are not part of the standard C or POSIX specifications.
# This is same as "gcc -D_GNU_SOURCE -o create_thread create_thread.c"
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE -D_GNU_SOURCE)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
    # List libraries link to the target library
    android
    log)

add_subdirectory(davincisec/DetectFrida)
add_subdirectory(aimardcr/AndroidSecurityGuard)