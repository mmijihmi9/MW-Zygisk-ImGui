#include <jni.h>
#include <android/log.h>
#include <android/input.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <dlfcn.h>
#include "zygisk.hpp"
#include "And64InlineHook.hpp"
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"

#define LOG_TAG "MW_ZygiskMenu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// تصحيح اسم الحزمة الرسمي للعبة
static const char* TARGET_PACKAGE = "com.shooter.modernwarship";
static bool g_Initialized = false;
static EGLint g_Width = 0;
static EGLint g_Height = 0;

struct MWConfig {
    bool showMenu = true;
    bool espRadar = false;
    bool noFog = false;
    float fovValue = 60.0f;
    float shipSpeedMultiplier = 1.0f;
    float missileSpeed = 1.0f;
    bool fastReload = false;
} config;

typedef int32_t (*AInputQueue_getEvent_t)(AInputQueue* queue, AInputEvent** outEvent);
static AInputQueue_getEvent_t orig_AInputQueue_getEvent = nullptr;

int32_t hooked_AInputQueue_getEvent(AInputQueue* queue, AInputEvent** outEvent) {
    int32_t result = orig_AInputQueue_getEvent(queue, outEvent);
    if (result >= 0 && outEvent && *outEvent) {
        if (g_Initialized && config.showMenu) {
            ImGui_ImplAndroid_HandleInputEvent(*outEvent);
        }
    }
    return result;
}

typedef EGLBoolean (*eglSwapBuffers_t)(EGLDisplay dpy, EGLSurface surface);
static eglSwapBuffers_t orig_eglSwapBuffers = nullptr;

EGLBoolean hooked_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    if (!g_Initialized) {
        eglQuerySurface(dpy, surface, EGL_WIDTH, &g_Width);
        eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_Height);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)g_Width, (float)g_Height);

        ImGui::StyleColorsDark();
        ImGui_ImplOpenGL3_Init("#version 300 es");
        g_Initialized = true;
        LOGI("ImGui Context initialized.");
    }

    // تحديث إدخال أندرويد (ضروري جداً لعمل اللمس وحجم الشاشة)
    ImGui_ImplAndroid_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    if (config.showMenu) {
        ImGui::SetNextWindowSize(ImVec2(480, 360), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Modern Warships Mod Menu", &config.showMenu, ImGuiWindowFlags_NoCollapse)) {
            if (ImGui::BeginTabBar("MWTabs")) {
                if (ImGui::BeginTabItem("Visuals")) {
                    ImGui::Checkbox("Radar ESP", &config.espRadar);
                    ImGui::Checkbox("Remove Water Fog", &config.noFog);
                    ImGui::SliderFloat("FOV Camera", &config.fovValue, 30.0f, 120.0f);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Ship Combat")) {
                    ImGui::SliderFloat("Ship Speed", &config.shipSpeedMultiplier, 1.0f, 3.0f);
                    ImGui::SliderFloat("Missile Speed", &config.missileSpeed, 1.0f, 3.0f);
                    ImGui::Checkbox("Instant Cannon Reload", &config.fastReload);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Status")) {
                    ImGui::Text("Target: %s", TARGET_PACKAGE);
                    ImGui::Text("Screen: %dx%d", g_Width, g_Height);
                    ImGui::Text("Status: Hooked via Zygisk");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return orig_eglSwapBuffers(dpy, surface);
}

class ModernWarshipsModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) return;
        const char *proc_name = env->GetStringUTFChars(args->nice_name, nullptr);
        if (proc_name) {
            if (std::strstr(proc_name, TARGET_PACKAGE) != nullptr) {
                is_target = true;
            }
            env->ReleaseStringUTFChars(args->nice_name, proc_name);
        }
        if (!is_target) {
            // إلغاء تحميل الوحدة إذا لم تكن اللعبة المستهدفة
            api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *) override {
        if (is_target) {
            LOGI("Modern Warships detected. Starting hooks...");
            std::thread(initHooks).detach();
        }
    }

private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
    bool is_target = false;

    static void initHooks() {
        // تقليل وقت الانتظار لتظهر القائمة أسرع
        sleep(2);

        void *libandroid = dlopen("libandroid.so", RTLD_NOW);
        if (libandroid) {
            void *sym_input = dlsym(libandroid, "AInputQueue_getEvent");
            if (sym_input) {
                A64HookFunction(sym_input, (void*)hooked_AInputQueue_getEvent, (void**)&orig_AInputQueue_getEvent);
            }
        }

        void *libegl = dlopen("libEGL.so", RTLD_NOW);
        if (libegl) {
            void *sym_egl = dlsym(libegl, "eglSwapBuffers");
            if (sym_egl) {
                A64HookFunction(sym_egl, (void*)hooked_eglSwapBuffers, (void**)&orig_eglSwapBuffers);
            }
        }
    }
};

REGISTER_ZYGISK_MODULE(ModernWarshipsModule)
