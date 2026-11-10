cmake_minimum_required(VERSION 3.22.1)
project(zygisk_mw_imgui CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# تنزيل ملف zygisk.hpp المستقل والرسمي تلقائياً
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/zygisk.hpp")
    file(DOWNLOAD 
        "https://raw.githubusercontent.com/topjohnwu/zygisk-module-sample/main/module/jni/zygisk.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/zygisk.hpp"
    )
endif()

# تنزيل ملف shadowhook.h مباشرة لضمان الوصول إليه
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/shadowhook.h")
    file(DOWNLOAD 
        "https://raw.githubusercontent.com/bytedance/android-inline-hook/main/shadowhook/src/main/cpp/include/shadowhook.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/shadowhook.h"
    )
endif()

include(FetchContent)

# جلب مكتبة ShadowHook
FetchContent_Declare(
    shadowhook
    GIT_REPOSITORY https://github.com/bytedance/android-inline-hook.git
    GIT_TAG main
    SOURCE_SUBDIR shadowhook
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

# إضافة المسارات الكاملة والدقيقة لملفات shadowhook.h و zygisk.hpp و imgui.h
target_include_directories(zygisk_mw PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${shadowhook_SOURCE_DIR}/src/main/cpp/include
    ${shadowhook_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(zygisk_mw
    shadowhook
    ${log-lib}
    ${android-lib}
    ${egl-lib}
    ${gles3-lib}
)
