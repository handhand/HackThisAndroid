/**
 * refer 1: https://bbs.kanxue.com/thread-285932.htm
 * refer 2: https://bbs.kanxue.com/thread-284838.htm
 * Usage: frida -U -f com.handhandlab.hackThisAndroid -l frida-scripts/simple/native_noop_pthread.js
 * this script will
 * 1. hook android_dlopen_ext to detect the loading of the target SimpleRasp.so library
 * 2. After SimpleRasp.so is loaded, then hook the libc function pthread_create. Check the parameter of
 * pthread_create, if the the thread function is in the SimpleRasp.so library,
 * then it will return 0, which means the thread is not created.
 * The result is Emulator detection will be bypassed, and but pthread_create hooking will be detected.
 */
function patchPthreadCreate(){
    let pthread_create = Module.findGlobalExportByName("pthread_create")
    let originPthraedCreate = new NativeFunction(pthread_create, "int", ["pointer", "pointer", "pointer", "pointer"]);
    let hackPthraedCreate = new NativeCallback(function (a, b, c, d) {
        var m = Process.getModuleByName("libSimpleRasp.so");
        var base = m.base
        var pthreadFunctionModule = Process.findModuleByAddress(c);
        if (pthreadFunctionModule.name == m.name) {
            console.log("+++ noop pthread_create for ", m.name);
            return 0;
        }
        return originPthraedCreate(a, b, c, d)
    }, "int", ["pointer", "pointer", "pointer", "pointer"])
    Interceptor.replace(pthread_create, hackPthraedCreate)
}

Java.perform(function() {
    // hook dlopen will crash the process in emulator api 35.
    var dlopen = Module.findGlobalExportByName("android_dlopen_ext");
    if (dlopen) {
        var isLoaded = false;
        Interceptor.attach(dlopen, {
            onEnter: function(args) {
                console.log("android_dlopen_ext");
//                console.log("args[0] =>", args==null);
                var libPath = args[0].readUtf8String();
                console.log("dlopen called with: " + libPath);
                if (libPath.indexOf("libSimpleRasp.so") !== -1) {
                    isLoaded = true;
                }
            },
            onLeave: function(retval) {
//                console.log("dlopen returned: " + retval);
                if(isLoaded) {
//                    var targetModule = Process.getModuleByName("libhandhandlab.so");
                    console.log("target loaded");
                    patchPthreadCreate();
                    isLoaded = false;
                }
            }
        });
    } else {
        console.log("android_dlopen_ext not found!");
    }
});
