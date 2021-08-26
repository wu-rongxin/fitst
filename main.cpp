/*
  needs libmosquitto-dev
  $ gcc -o libmosq libmosq.c -lmosquitto
  author: matt ji
  date: 2019/3/25
  
*/
#include "workingThread.h"
#include "log.h"
#include "misc.h"
#include "update.h"
#include "stdio.h"
#include <unistd.h>
#include <thread>
#include "lora.h"
#include "monitorConfig.h"
#include "autohold.h"
using namespace std;

int main(int argc, char *argv[])
{
    //初始化log
    int ret = initlog();
    if(ret != 0)
    {
        printf("init log file error, exit");
        return 1;
    }
    log(0, "tk server version V2.9\n");
    //1. 读取config文件
    if(readConfig() != 0)
    {
        log(0, "read config file error, exit"); 
        return 2;
    }
    //2. 启动读取楼层map的线程，因为在安装完成后需要设置楼层map
    //重启程序会耗时很久
    thread (readMapThread).detach();
    //启动状态板,主板的io默认电平会阻止状态办启动，需要设置io电平
    set_normal();
    //初始化云端mqtt
    //mqtt_setup();
    //初始化本地publisher
    mqtt_setup_l();

    //1. 建立与状态板线程
    thread (mcuEleThread).detach();
    //2. 建立与io板的线程
    thread (eleMcuThread).detach();
    //2. 建立与适配层线程
    thread (eleCloudThread).detach();
    //3. 建立开关门按键延时
    thread (panelThread).detach();
    thread (autoThread).detach();
    thread (watchdogThread).detach();
    //4. 建立lora 线程
    #ifdef LORA_SUPPORT
    thread(loraThread).detach();
    #endif

    //wrx 一个线程用来整理当前电梯状态以及呼梯请求，填充队列。
    //wrx 另一个线程通过mqtt把队列里的信息发送出去。
    thread (eleStatePushThread).detach();
    thread (eleStatePopThread).detach();

    //test

    while(1){
        //500ms
        usleep(500000);
  }
 
}
