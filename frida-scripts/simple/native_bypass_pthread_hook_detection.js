/**
 * https://bbs.kanxue.com/thread-285932.htm
 * Usage: frida -U -f com.handhandlab.handyAndroidHackThis -l frida-scripts/bypass_pthread_hook_detection.js
 *
 * The result is Emulator and Frida detection will be bypassed
 */
var clone = Module.findExportByName('libc.so', 'clone');
Interceptor.attach(clone, {
    onEnter: function(args) {
        // args[3] 子线程的栈地址。如果这个值为 0，可能意味着没有指定栈地址
        if(args[3] != 0){
            var addr = args[3].add(96).readPointer()
            var so_name = Process.findModuleByAddress(addr).name;
            var so_base = Module.getBaseAddress(so_name);
            var offset = (addr - so_base);
            console.log("===============>", so_name, addr,offset, offset.toString(16));

            // libSimpleRasp.so should be already loaded in memory at this time
            if(so_name == "libSimpleRasp.so") {
                patch_func_nop(addr);
            }
        }
    },
    onLeave: function(retval) {

    }
});

function patch_func_nop(addr) {
    Memory.patchCode(addr, 8, function (code) {
        code.writeByteArray([0xE0, 0x03, 0x00, 0xAA]);
        code.writeByteArray([0xC0, 0x03, 0x5F, 0xD6]);
        console.log("patch code at " + addr)
    });
}