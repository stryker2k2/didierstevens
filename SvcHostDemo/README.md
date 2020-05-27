https://blog.didierstevens.com/2019/10/28/quickpost-compiling-service-dlls-with-mingw-on-kali/
https://blog.didierstevens.com/2019/10/29/quickpost-running-a-service-dll/





Didier Stevens
Monday 28 October 2019
Quickpost: Compiling Service DLLs with MinGW on Kali
Filed under: Quickpost — Didier Stevens @ 7:36

This DEF file can be passed as argument to the MinGW compiler.

To create a 32-bit DLL:
    i686-w64-mingw32-gcc -shared -municode -o SvcHostDemo.dll SvcHostDemo.def SvcHostDemo.cpp

To create a 64-bit DLL:
    x86_64-w64-mingw32-gcc -shared -municode -o SvcHostDemo.dll SvcHostDemo.def SvcHostDemo.cpp

Option -municode is needed because I use TCHARs and want to compile a UNICODE executable.






Didier Stevens
Tuesday 29 October 2019
Quickpost: Running a Service DLL
Filed under: Quickpost — Didier Stevens @ 0:00

To install and run a service DLL compiled with MinGW on Kali, I execute following commands from a BAT file with an elevated command prompt:

1) copy SvcHostDemo.dll %SystemRoot%\System32\SvcHostDemo.dll
2) sc create SvcHostDemo binPath= ^%%SystemRoot^%%"\system32\svchost.exe -k mygroup" type= share start= demand
3) reg add HKLM\SYSTEM\CurrentControlSet\services\SvcHostDemo\Parameters /v ServiceDll /t REG_EXPAND_SZ /d ^%%SystemRoot^%%\System32\SvcHostDemo.dll /f
4) reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Svchost" /v mygroup /t REG_MULTI_SZ /d SvcHostDemo /f
