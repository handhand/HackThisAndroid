## Hack This Android!
This project is for testing your Android security development skills, as well as your hacking skill.

compatible with frida 17
refer: https://stackoverflow.com/questions/79700740/frida-17-module-getexportbyname-typeerror-not-a-function

## 模块介绍
### SimpleRasp
实现了几种检测Frida、root、emulator的简单方法，注意并没有把所有可能的防护策略都用上，比如没有在constructor启动、没有使用自定义的库函数等，可以作为一个入门级的hack练手

### davincisec/FridaDetect - https://github.com/darvincisec/DetectFrida
- 文章: https://darvincitech.wordpress.com/2019/12/23/detect-frida-for-android/
- 做了些改动：参考local_dlfcn使用__LP64__来区分架构
- lib无法找到的问题，参考这个issue: https://github.com/darvincisec/DetectFrida/issues/44
- 关于named pipe，亲测没有用了，注释掉相关代码

### AndroidSecurityGuard - https://github.com/aimardcr/AndroidNativeGuard
- 关键的函数都自己重写了，见SecureAPI模块
- 在Jni_OnLoad() 中启动检测
- 使用dl_iterate_phdr来查看进程中加载的so，类似检查/proc/self/maps文件
- named pipe应该检查不到了，注释掉了
- 通过链接websocket来判断，但容易有false positive，见https://medium.com/@aimardcr/detecting-frida-the-right-way-7cb3227edafb
- AntiLibPatch还没看到懂，有很多hardcode的数字和字符，原理估计类似于 SimpleRasp 的 lib_patch_detection.c

### ProRasp
- TODO: 结合所有的防护方法

### The dark side
- frida-scripts文件夹里放的是破解检测的frida-script, 主要是针对SimpleRasp的

## 对一些防护方法的说明
#### SimpleRasp 通过对比so文件的内容和内存中的代码来判断是否被hook
参考: https://www.52pojie.cn/thread-1921073-1-1.html
实现: app/src/main/cpp/simple_rasp/frida_function_hook_detection.cpp
用到的库: https://github.com/luoyesiqiu/local_dlfcn

#### ptrace 附加防止frida
frida只需要短暂的attach，并不需要一直attach到进程
https://blog.csdn.net/qq_38851536/article/details/105087447
另外用spawn的方式可以避免检测，所以这个功能先不实现。

#### 对davincisec/FridaDetect - detect_frida_memdiskcompare()的解析
1. 先读取elf section header，把带有 executable flag 的section记录下来，包括起始地址、大小，计算对应的checksum (通常是.text和.plt, 用readelf -S 看的话flag应该是AX)
2. 读取/proc/self/maps文件，读取对应lib的行，将第一行的start地址作为基地址，然后加上section offset作为起始地址，并使用记录的section大小，计算这段内存的checksum
3. 对比文件section的checksum和内存中的checksum，如果不一致则说明被hook了。
注意有个有趣的地方，如果监控的是自己(libDetectFrida.so)，那么调试运行的时候，加断点也会检测出内存出现修改，和文件的checksum对不上；

#### SimpleRASP 对detect_frida_memdiskcompare()的实现
1. 读取的是program header可执行的段，再读取maps文件里加载的可执行的内存块，再对比两者
2. 使用clone来启动线程
3. clone的线程调用jni会出错，暂时无法解决，先用socket来通知java检测结果

#### Cert pinning
点击按钮会请求baidu的网页，因为android默认的安全机制，一般情况下加了代理使用代理的证书后会请求失败；
可以通过bypass_certpinning.js来绕过

## 参考
- so startup description - http://www.dbp-consulting.com/tutorials/debugging/linuxProgramStartup.html
- use clone to create thread - https://nullprogram.com/blog/2015/05/15/

## TODO
- 加上加固
- 用assembly来创建线程
- 加上ollvm