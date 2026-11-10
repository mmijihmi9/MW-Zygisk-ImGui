#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SHADOWHOOK_MODE_SHARED 0
#define SHADOWHOOK_MODE_UNIQUE 1

int shadowhook_init(int mode, bool debuggable);
void *shadowhook_hook_sym_name(const char *lib_name, const char *sym_name, void *new_addr, void **orig_addr);
int shadowhook_unhook(void *stub);

#ifdef __cplusplus
}
#endif
