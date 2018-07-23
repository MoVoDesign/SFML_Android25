# This variable indicates the location of the source files in the development tree.
#   Here, the macro function `my-dir`, provided by the build system, returns the path of the current directory
#   (the directory containing the `Android.mk` file itself).
LOCAL_PATH := $(call my-dir)

# The CLEAR_VARS variable points to a special GNU Makefile that clears many `LOCAL_XXX` variables for you,
#   such as `LOCAL_MODULE`, `LOCAL_SRC_FILES`, and `LOCAL_STATIC_LIBRARIES`.
#   Note that it *does not* clear `LOCAL_PATH`. This variable must retain its value because the system parses all build control files in a single GNU Make execution context where all variables are global.
#   You must (re-)declare this variable before describing each module.
include $(CLEAR_VARS)

# Each module name must be unique and not contain any spaces.
#   The build system, when it generates the final shared-library file, automatically adds the proper prefix and suffix to the name that you assign to `LOCAL_MODULE`.
#   For example, the example that appears above results in generation of a library called `libhello-jni.so`
LOCAL_MODULE    := sfml-example

# The LOCAL_SRC_FILES variable must contain a list of C and/or C++ source files to build into a module.
#   enumerate the source files with spaces delimiting multiple files
LOCAL_SRC_FILES := app.cpp main.cpp

# allow exceptions
#   https://stackoverflow.com/questions/3217925/enable-exception-c
LOCAL_CPP_FEATURES += exceptions

# This variable is the list of shared libraries modules on which this module depends at runtime.
#   This information is necessary at link time, and to embed the corresponding information in the generated file.
#   all shared libs are defined in NDK/third-party/sfml/Android.mk called at the end
LOCAL_SHARED_LIBRARIES := sfml-system-d
LOCAL_SHARED_LIBRARIES += sfml-window-d
LOCAL_SHARED_LIBRARIES += sfml-graphics-d
LOCAL_SHARED_LIBRARIES += sfml-audio-d
LOCAL_SHARED_LIBRARIES += sfml-network-d
LOCAL_SHARED_LIBRARIES += sfml-activity-d
LOCAL_SHARED_LIBRARIES += openal
# only sfml-main
LOCAL_WHOLE_STATIC_LIBRARIES := sfml-main-d

# The BUILD_SHARED_LIBRARY variable points to a GNU Makefile script that collects all the information you defined in LOCAL_XXX variables
#   since the most recent include.
#   This script determines what to build, and how to do it.
#   A shared-library variable causes the build system to generate a library file with a .so extension.
include $(BUILD_SHARED_LIBRARY)

$(call import-module,third_party/sfml)
$(call import-module,third_party/sfml/extlibs)