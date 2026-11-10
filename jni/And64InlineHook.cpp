#include "And64InlineHook.hpp"
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#define PAGE_START(addr) (~(getpagesize() - 1) & (uintptr_t)(addr))
#define PAGE_SIZE getpagesize()

void A64HookFunction(void *const symbol, void *const replace, void **result) {
    if (!symbol || !replace) return;

    uint32_t *target = (uint32_t *)symbol;
    uintptr_t hook = (uintptr_t)replace;

    if (result) {
        uint32_t *trampoline = (uint32_t *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (trampoline) {
            memcpy(trampoline, target, 16);
            uintptr_t return_addr = (uintptr_t)target + 16;
            trampoline[4] = 0x58000051; // LDR X17, #10
            trampoline[5] = 0xd61f0220; // BR X17
            memcpy(&trampoline[6], &return_addr, sizeof(return_addr));
            *result = (void *)trampoline;
        }
    }

    mprotect((void *)PAGE_START(target), PAGE_SIZE * 2, PROT_READ | PROT_WRITE | PROT_EXEC);

    target[0] = 0x58000051; // LDR X17, #8
    target[1] = 0xd61f0220; // BR X17
    memcpy(&target[2], &hook, sizeof(hook));

    __builtin___clear_cache((char *)target, (char *)target + 16);
}
