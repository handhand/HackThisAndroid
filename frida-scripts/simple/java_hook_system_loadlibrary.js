/**
 * This script NOOP the System.loadLibrary function, and hook the jni method in Java space.
 * Usage: frida -U -f com.handhandlab.handyAndroidHackThis -l frida-scripts/hook_java_system_loadlibrary.js
 **/
Java.perform(function() {
    // Retrieve the class with Java.use
    const System = Java.use('java.lang.System');
    const Runtime = Java.use('java.lang.Runtime');
    const SystemLoad_2 = System.loadLibrary.overload('java.lang.String');
    const VMStack = Java.use('dalvik.system.VMStack');
    // Modify the implementation
    SystemLoad_2.implementation = function (libname) {
        // Log the event
        console.log("Frida - loadLibrary('" + libname.toString() + "') called" );

        // Note calling the original method like below will NOT work.
        //this.loadLibrary(libname);

        // load library like this: work on Android 35
        // https://github.com/frida/frida-java-bridge/issues/63
        console.log("loadLibrary called start with: " + libname);
        const loaded = Runtime.getRuntime().loadLibrary0(VMStack.getCallingClassLoader(), libname);
//        Runtime.getRuntime().load0(VMStack.getCallingClassLoader(), libname);
        console.log("loadLibrary called end with: " + libname);
    }

    const JniInterface = Java.use("com.handhandlab.handyandroidctf.jni.JniInterface");
    JniInterface.entryPoint.implementation = function () {
        return "HACK!"
    }
});
