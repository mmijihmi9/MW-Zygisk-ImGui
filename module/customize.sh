SKIPUNZIP=0
if [ "$MAGISK_VER_CODE" -lt 24000 ] && [ -z "$KSU" ]; then
    ui_print "! يتطلب KernelSU أو Magisk Zygisk"
    abort
fi
ui_print "- تم تثبيت قائمة Modern Warships بنجاح."
