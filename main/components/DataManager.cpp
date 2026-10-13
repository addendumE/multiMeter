#include "DataManager.h"
#include <math.h>
#include <stdint.h>
#include <esp_log.h>

static const char *TAG="DM";

DataManager::DataManager (ObjManager &objectManagr,Persistance &persistance,Hal &hal):
    Thread("dataManager",8192),
    objectManagr(objectManagr), // object manager
    persistance(persistance), // perssitance manager
    hal(hal),
    loopCnt(0),
    last_state(0),
    device("device","device",persistance),
    ai (
        {"Ain0","tensione in 0",persistance,UNIT_VOLTAGE,2},
        {"Ain1","tensione in 1",persistance,UNIT_VOLTAGE,2},
        {"Ain2","tensione in 2",persistance,UNIT_VOLTAGE,2},
        {"Ain3","tensione in 3",persistance,UNIT_VOLTAGE,2}
        )
{
    gpio_config_t in_conf = {};
    in_conf.intr_type = GPIO_INTR_DISABLE;
    in_conf.mode = GPIO_MODE_INPUT;
    in_conf.pin_bit_mask = (1ULL << IN_BOOT_BUTTON);
    in_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    in_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&in_conf);

    objectManagr.addObject(&device);    
    for (auto ch=0; ch<CH_NUMBER; ch++)
    {
        adc.init(ch,ADC_ATTEN_DB_12);
        objectManagr.addObject(&ai[ch]);
    }
    start();
}

DataManager::~DataManager()
{

}

//-------------------------   MAIN LOOP --------------------
void DataManager::loop()
{
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (true)
    {
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
        int state = gpio_get_level(IN_BOOT_BUTTON);
        ESP_LOGI(TAG,"button %d",state);
        if (state != last_state && state == 0) {
                ESP_LOGI(TAG,"trigger");
            if (bootButtonCallBack) bootButtonCallBack();
        }
        last_state = state;
        device.setUptime(loopCnt/60.0f);
        device.setFreeHeap(hal.heapOccupation());
        for (auto ch=0; ch<CH_NUMBER; ch++)
        {
            float raw = adc.readVoltage(ch);
            ai[ch].setValue(raw);
        //    ESP_LOGI(TAG,"ADC ch %d raw:%f value:%f",ch,raw,ai[ch].getValue());
        }
        if (loopCnt % 600 == 0)
        {
            ts.clean();
            for (auto ch=0; ch<CH_NUMBER; ch++)
            {
                ts.setValue(ch+1, ai[ch].getValue());
            }
            ts.publish(getKey());
        }
        loopCnt++;
    }
}

//------------------------- AUX FUNCTIONS --------------------
string DataManager::getSSID()
{
    return device.getPropertyValueString(PROP_SSID);
}

string DataManager::getPwd()
{
    return device.getPropertyValueString(PROP_SSID_KEY);

}

string DataManager::getKey()
{
    return device.getPropertyValueString(PROP_TS_KEY);
}

string DataManager::getMqttUri()
{
    return device.getPropertyValueString(PROP_MQTT_URI);
}

string DataManager::getUid()
{
    return device.getPropertyValueString(PROP_UID);
}

void DataManager::setDeviceStatus(const string &status)
{
    device.setPropertyValueString(Types::PROP_VALUE,status);
}

