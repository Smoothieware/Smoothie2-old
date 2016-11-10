#Segger JTrace SystemView debugging and integration 

Go to http://www.segger.com and download their SystemView free tool and copy only some of the source code files so you end up with the following files:
```
src/Segger_SystemView/SEGGER_SYSVIEW_Int.h
src/Segger_SystemView/Global.h
src/Segger_SystemView/SEGGER.h
src/Segger_SystemView/SEGGER_RTT.c
src/Segger_SystemView/SEGGER_RTT.h
src/Segger_SystemView/SEGGER_RTT_Conf.h
src/Segger_SystemView/SEGGER_SYSVIEW.c
src/Segger_SystemView/SEGGER_SYSVIEW.h
src/Segger_SystemView/SEGGER_SYSVIEW_Conf.h
src/Segger_SystemView/SEGGER_SYSVIEW_ConfDefaults.h
src/Segger_SystemView/SEGGER_SYSVIEW_Config_NoOS.c
```

You will need to make the following changes in src/Segger_SystemView/SEGGER_SYSVIEW_Config_NoOS.c

old
```
#define SYSVIEW_APP_NAME        "Demo Application"
```
new
```
#define SYSVIEW_APP_NAME        "Smoothie2"
```
old
```
SEGGER_SYSVIEW_SendSysDesc("I#15=SysTick");
```
new (note the line is long and scrolls off the right hand side of the page, make sure you copy it all)
```
SEGGER_SYSVIEW_SendSysDesc("I#15=SysTick,I#27=acceleration_tick(),I#28=step_tick(),I#29=unstep_tick(),I#30=Timer2,I#100=Endstops::Endstops()");
```
If you find that you are running out of buffering between the CPU and your PC, then change src/Segger_SystemView/SEGGER_SYSVIEW_ConfDefaults.h as follows

old
```
#define SEGGER_SYSVIEW_RTT_BUFFER_SIZE    1024
```
new
```
#define SEGGER_SYSVIEW_RTT_BUFFER_SIZE    4096
```
Note: 4096 is the largest buffer size supported; you can also increase the speed of the JTAG interface to 15MHz when you start your JLinkGDBServer:

JLinkGDBServer -if jtag -device LPC4330 -speed 15000

Note: I run my JLinkGDBServer on a separate Ubuntu machine onto which my Firepick is attached and use TCP/IP to connect to it.
