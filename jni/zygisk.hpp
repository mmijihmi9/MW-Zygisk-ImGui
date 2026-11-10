#pragma once

#include <jni.h>

namespace zygisk {

struct AppSpecializeArgs {
    jint &uid;
    jint &gid;
    jintArray &gids;
    jint &runtime_flags;
    jobjectArray &rlimits;
    jint &mount_external;
    jstring &se_info;
    jstring &nice_name;
    jintArray &is_child_gids;
    jstring &instruction_set;
    jstring &app_data_dir;
    jboolean &is_profileable;
    jobjectArray &is_profileable_app;
    jboolean &use_memory_safety;
};

enum Option : int {
    FORCE_DENYLIST_UNMOUNT = 0,
    DLCLOSE_MODULE_LIBRARY = 1,
};

class Api {
public:
    virtual bool setOption(Option opt) = 0;
};

class ModuleBase {
public:
    virtual void onLoad(Api *api, JNIEnv *env) {}
    virtual void preAppSpecialize(AppSpecializeArgs *args) {}
    virtual void postAppSpecialize(const AppSpecializeArgs *args) {}
};

} // namespace zygisk

#define REGISTER_ZYGISK_MODULE(clazz) \
extern "C" [[gnu::visibility("default")]] \
void zygisk_module_entry(zygisk::Api *api, JNIEnv *env) { \
    static clazz module; \
    module.onLoad(api, env); \
}
