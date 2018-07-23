#NDK_TOOLCHAIN_VERSION := 4.9
# was 14
APP_PLATFORM := android-15
# APP_STL has to match CMAKE_ANDROID_STL_TYPE was static
APP_STL := c++_shared
APP_ABI := armeabi-v7a
APP_MODULES := sfml-activity-d sfml-example
APP_OPTIM := debug
APP_CFLAG := -g -ggdb -O0
