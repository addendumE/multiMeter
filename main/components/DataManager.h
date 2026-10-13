#define NO_IIC_DEBUG
#include "Thread.h"
#include "ObjManager.h"
#include "Persistance.h"
#include "AnalogInput.h"
#include "DigitalOutput.h"
#include "DigitalOutputFeedback.h"
#include "DigitalInput.h"
#include "Device.h"
#include "Threshold.h"
#include <functional>
#include "Hal.h"
#include "Adc.h"
#include "ThingSpeak.h"
#include "driver/gpio.h"

#pragma once
#define CH_NUMBER 4
#define IN_BOOT_BUTTON          GPIO_NUM_9

class DataManager: public Thread
{
public:
    DataManager (ObjManager &om, Persistance &persistance, Hal &hal);
    virtual ~DataManager();
    string getSSID();
    string getPwd();
    string getKey();
    string getMqttUri();
    string getUid();
    using ButtonCallBack = std::function<void(void)>;
    void onBootButton(ButtonCallBack cb) {
        bootButtonCallBack = cb;
    }

    void setDeviceStatus(const string &status);
    void fwUpgrade() {}
private:
    ButtonCallBack bootButtonCallBack;
    void loop();
    ObjManager &objectManagr;
    Persistance &persistance;
    Hal &hal;
    size_t loopCnt;
    int last_state;
    Device device;
    Adc adc;
    AnalogInput ai[CH_NUMBER];
    ThingSpeak ts;
};
