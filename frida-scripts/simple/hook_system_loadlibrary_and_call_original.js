/**
 * This script NOOP the System.loadLibrary function, and hook the jni method in Java space.
 * Usage: frida -U -f com.handhandlab.handyandroidctf -l frida-scripts/hook_java_system_loadlibrary.js
 **/
Java.perform(function() {
    const System = Java.use('java.lang.System');
    const Runtime = Java.use('java.lang.Runtime');
    const VMStack = Java.use('dalvik.system.VMStack');
    //load:
    System.load.implementation = function(library) {
        console.log("load called start with: " + library);
        Runtime.getRuntime().load0(VMStack.getStackClass1(), library);
        console.log("load called end with: " + library);
    };
    //loadLibrary:
    System.loadLibrary.implementation = function(library) {
        console.log("loadLibrary called start with: " + library);
        Runtime.getRuntime().load0(VMStack.getCallingClassLoader()(), library);
        console.log("loadLibrary called end with: " + library);
    };

    //use android_dlopen_ext before calling JNI_OnLoad
//    var dlopenPtr = Module.findExportByName(null, "android_dlopen_ext");
//    Interceptor.attach(dlopenPtr, {
//        onEnter: function (args) {
//            this.libPath = args[0].readUtf8String();
//            console.log("android_dlopen_ext called start with: " + this.libPath);
//        },
//        onLeave: function (retval) {
//            console.log("android_dlopen_ext called end with: " + this.libPath);
//        }
//    });
});