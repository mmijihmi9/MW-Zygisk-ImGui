cmake_minimum_required(VERSION 3.22.1)
project(zygisk_mw_imgui CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# إعداد خيارات تجميع الأسمبلي لتفادي أخطاء Clang
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wa,-mcpu=all")

include(FetchContent)

# تعطيل بناء الاختبارات والمكاتب المشاركة لـ Dobby
set(DOBBY_GENERATE_SHARED OFF CACHE BOOL "" FORCE)
set(DOBBY_BUILD_TEST OFF CACHE BOOL "" FORCE)

# جلب Dobby من الفرع الرئيسي الصحيح master
FetchContent_Declare(
    dobby
    GIT_REPOSITORY https://github.com/jmpews/Dobby.git
    GIT_TAG master
)
FetchContent_MakeAvailable(dobby)

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
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(zygisk_mw
    dobby
    ${log-lib}
    ${android-lib}
    ${egl-lib}
    ${gles3-lib}
)
