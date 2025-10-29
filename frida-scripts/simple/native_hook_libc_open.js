/**
 * This script does not modify any behavior, only log the native method calls.
 * It can be used to test the SimpleRASP's frida detection, as well as the LibPatch detection for libc.so
 * usage: frida -U -f com.handhandlab.handyAndroidHackThis -l frida-scripts/simple/native_hook_libc_open.js
 */
function hook_open(){
    var open_addr = Process.getModuleByName('libc.so').findExportByName("open");
    console.log("open_addr: ", open_addr);
    Interceptor.attach(open_addr,{
        onEnter:function(args){
            //this.returnAddress will return the calling side address
            var returnModule = Process.findModuleByAddress(ptr(this.returnAddress));

            // openFunctionModule should equals returnModule
            console.log("open from module: ", returnModule.name);
        },onLeave:function(retval){
        }
    });
}
hook_open();