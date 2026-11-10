cmake_minimum_required(VERSION 3.22.1)

# تفعيل لغة ASM لبناء أكواد معالجات ARM64
enable_language(ASM)
project(zygisk_mw_imgui CXX C ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# تفعيل البناء الساكن (Static) لدمج ShadowHook بالكامل داخل مكتبة Zygisk
set(SHADOWHOOK_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(SHADOWHOOK_BUILD_SHARED OFF CACHE BOOL "" FORCE)

# جلب مستودع android-inline-hook من الجذر لبناء xdl و shadowhook معا
FetchContent_Declare(
    shadowhook
    GIT_REPOSITORY https://github.com/bytedance/android-inline-hook.git
    GIT_TAG main
    GIT_SUBMODULES_RECURSE TRUE
)
FetchContent_MakeAvailable(shadowhook)

# جلب مكتبة ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.4
)
FetchContent_MakeAvailable(imgui)

find_library(log-lib log)
find_library(android-lib android)
find_library(egl-lib EGL)
find_library(gles3-lib GLESv3)

add_library(zygisk_mw SHARED
    main.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_android.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(zygisk_mw PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${shadowhook_SOURCE_DIR}/shadowhook/src/main/cpp/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# الربط الساكن المباشر مع shadowhook_static
target_link_libraries(zygisk_mw
    shadowhook_static
    ${log-lib}
    ${android-lib}
    ${egl-lib}
    ${gles3-lib}
)
