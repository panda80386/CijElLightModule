// CIJ El-Light Module
// 2023/01/09 Ver1.00
#include <Easing.h>

const unsigned int PinC = A17;
const unsigned int PinI = A18;
const unsigned int PinJ = A19;

const unsigned int ChannelC = 0;
const unsigned int ChannelI = 1;
const unsigned int ChannelJ = 2;

const unsigned int CijAll[] = {ChannelC, ChannelI, ChannelJ};
const int CijAllLength = sizeof(CijAll) / sizeof(CijAll[0]);


struct ChannelSetting
{
  int Channel;
  int Pin;
};

const ChannelSetting ChannelSettings[3] =
{
  {ChannelC, PinC},
  {ChannelI, PinI},
  {ChannelJ, PinJ}
};

EasingFunc<Ease::SineOut> _easing;

byte _stateC;
byte _stateI;
byte _stateJ;
byte _oldStateC;
byte _oldStateI;
byte _oldStateJ;

TaskHandle_t _autoBlinkTaskHandle = NULL;

void ElInit()
{
  for(int i = 0; i < CijAllLength; i++)
  {
    ChannelSetting setting = ChannelSettings[i];
    Serial.println(setting.Channel);
    pinMode(setting.Pin, OUTPUT);
    ledcSetup(setting.Channel, 12800, 8);
    ledcAttachPin(setting.Pin, setting.Channel);
  }
}

void SetChannels(const unsigned int *channels, int msec, bool isOn)
{
  if (isOn)
  {
    SetDigitalChannels(channels, isOn);
    delay(msec);
  }
  else
  {
    int fadeTime = 150;
    if (msec > fadeTime)
    {
      msec -= fadeTime;
      for(int i = 0; i < fadeTime; i++)
      {
        double t = (double)i / (double)fadeTime;
        int val = _easing.get(1.0 - t);
        SetAnalogChannels(channels, val);
        delay(1);
      }
    }

    SetDigitalChannels(channels, isOn);
    delay(msec);
  }
}

void SetChannel(const unsigned int channel, int msec, bool isOn)
{
  if (isOn)
  {
    SetDigitalChannel(channel, isOn);
    delay(msec);
  }
  else
  {
    int fadeTime = 130;
    if (msec > fadeTime)
    {
      msec -= fadeTime;
      for(int i = 0; i < fadeTime; i++)
      {
        double t = (double)i / (double)fadeTime;
        int val = _easing.get(1.0 - t);
        SetAnalogChannel(channel, val);
        delay(1);
      }
    }

    SetDigitalChannel(channel, isOn);
    delay(msec);
  }
} 

void SetDigitalChannels(const unsigned int *channels, bool isOn)
{
  for(int i = 0; i < sizeof(channels) + 1; i++)
    DigitalWrite(channels[i], isOn);
}

void SetDigitalChannel(const unsigned int channel, bool isOn)
{
  DigitalWrite(channel, isOn);
} 

void SetAnalogChannels(const unsigned int *channels, int val)
{
  for(int i = 0; i < sizeof(channels) + 1; i++)
    AnalogWrite(channels[i], val);
}

void SetAnalogChannel(const unsigned int channel, int val)
{
  AnalogWrite(channel, val);
}

void DigitalWrite(const unsigned int channel, bool val)
{
  ledcWrite(channel, val ? 255 : 0);
}

void AnalogWrite(const unsigned int channel, int val)
{
  ledcWrite(channel, val);
}

void ClearAllChannels()
{
  for(int i = 0; i < CijAllLength; i++)
  {
    DigitalWrite(CijAll[i], false);
  }
}

// 
void CreateAutoBlinkTask()
{
  xTaskCreateUniversal(
    AutoBlinkTask,
    "AutoBlinkTask",
    8192, // スタックサイズ
    NULL, // 起動パラメータ
    1, // 優先度
    &_autoBlinkTaskHandle,
    APP_CPU_NUM // 実行するコア
    );
}

void AutoBlinkTask(void *pvParameters)
{
  while (true)
  {
    SetChannel(ChannelC, 900, HIGH);
    SetChannel(ChannelC, 450, LOW);

    SetChannel(ChannelI, 900, HIGH);
    SetChannel(ChannelI, 450, LOW);

    SetChannel(ChannelJ, 900, HIGH);
    SetChannel(ChannelJ, 1430, LOW);

    SetChannel(ChannelC, 480, HIGH);
    SetChannel(ChannelI, 480, HIGH);
    SetChannel(ChannelJ, 1230, HIGH);
    SetChannels(CijAll, 1100, LOW);

    SetChannels(CijAll, 700, HIGH);
    SetChannels(CijAll, 600, LOW);

    SetChannels(CijAll, 730, HIGH);
    SetChannels(CijAll, 1250, LOW);
  }
}

void DeleteTask(TaskHandle_t* taskHandle)
{
  if (*taskHandle != NULL)
  {
    vTaskDelete(*taskHandle);
    (*taskHandle) = NULL;
  }
}

// setup
void setup()
{
  Serial.begin(115200);
  ElInit();
  CreateAutoBlinkTask();
}

// main loop
void loop()
{
  delay(1);
}
