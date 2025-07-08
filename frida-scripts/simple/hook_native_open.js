function hook_open(){
    var open_addr = Module.findExportByName("libc.so", "open");
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