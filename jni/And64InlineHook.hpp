#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void A64HookFunction(void *const symbol, void *const replace, void **result = nullptr);
