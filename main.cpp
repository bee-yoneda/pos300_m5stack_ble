#include <M5Stack.h>
#include <driver/adc.h>
#include "driver/i2s.h"
#include "aquestalk.h"
#include "BlePos300.h"
#include "DispFrame.h"
#include "Keyboard.h"

typedef enum {
  DISP_STATUS_NOT_CONNECT = 0,
  DISP_STATUS_CONNECTING,
  DISP_STATUS_MEASURED,
  DISP_STATUS_EDIT,
} DISP_STATUS;

DISP_STATUS mStatus = DISP_STATUS_NOT_CONNECT;

Keyboard kbd;

#define LEN_FRAME 32
uint32_t workbuf[AQ_SIZE_WORKBUF];

////////////////////////////////
//  i2s configuration 
#define  SAMPLING_FREQ   8000 // 8KHz
#define DMA_BUF_COUNT 3
#define DMA_BUF_LEN  LEN_FRAME // one buffer size(one channnel samples)
#define DMA_BUF_SIZE (DMA_BUF_COUNT*DMA_BUF_LEN)
#define I2S_FIFO_LEN  (64/2)
#define  TICKS_TO_WAIT (2*LEN_FRAME/8/portTICK_PERIOD_MS)

//i2s configuration 
static const int i2s_num = 0; // i2s port number
static i2s_config_t i2s_config = {
     .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
     .sample_rate = SAMPLING_FREQ,
     .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
     .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
     .intr_alloc_flags = 0,
     .dma_buf_count = DMA_BUF_COUNT,
     .dma_buf_len = DMA_BUF_LEN,
     .use_apll = 0
};

static void DAC_create()
{
  i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
  i2s_set_pin((i2s_port_t)i2s_num, NULL);
  i2s_stop((i2s_port_t)i2s_num);  // Create時はstop状態
}

static void DAC_release()
{
  i2s_driver_uninstall((i2s_port_t)i2s_num); //stop & destroy i2s driver 
}

int16_t Amplify_edit(int16_t s) {
#if 1 // volume設定しない場合
  return s;
#else
  // 音量設定
  int32_t v = (s * (uint8_t)(0.12*(1<<6)))>>6;
  if(v>32767)       return 32767;
  else if(v<-32768) return -32768;
  else              return (int16_t)(v&0xffff);
#endif
}


// write to I2S DMA buffer
static int DAC_write_val(uint16_t val)
{
  uint16_t sample[2];
  sample[0]=sample[1]=val; // mono -> stereo
  size_t bytes_written;
  esp_err_t iret = i2s_write((i2s_port_t)i2s_num, sample, sizeof(uint16_t)*2, &bytes_written, TICKS_TO_WAIT);
  if(iret!=ESP_OK) return -1;
  if(bytes_written<sizeof(uint16_t)*2) return 0; // timeout
  return 1;
}

static void DAC_start()
{
  i2s_start((i2s_port_t)i2s_num);
  
  // reduce pop noize
  int k;
  for(k=0;k<DMA_BUF_LEN;k++){
    DAC_write_val(0);
  }
  for(k=0;k<=32768;k+=256*3) {
    DAC_write_val((uint16_t)k);
  }
}

static void DAC_stop()
{
  // reduce pop noize
  int k;
  for(k=32768;k>=0;k-=256*3) {
    DAC_write_val((uint16_t)k);
  }
  for(k=0;k<DMA_BUF_SIZE+I2S_FIFO_LEN;k++){
    DAC_write_val(0);
  }
  i2s_stop((i2s_port_t)i2s_num);  
}

// upsampling & write to I2S
static int DAC_write(int len, int16_t *wav)
{
  int i;
  for(i=0;i<len;i++){

    int iret = DAC_write_val((Amplify_edit(wav[i]))^0x8000U);// for Internal-DAC (signed -> unsigned data)
//  int iret = DAC_write_val(wav[i]);// for External SPI-DAC
    if(iret<0) return 404; // -1:parameter error
    if(iret==0) break;  //  0:TIMEOUT
  }
  return i;
}

// 一文の音声出力（同期型）
static void playAquesTalk(const char *koe)
{
  Serial.print("Play:");
  Serial.println(koe);

  int iret = CAqTkPicoF_SetKoe((const uint8_t*)koe, 80, 0xffffU);
  if(iret)  {
    Serial.printf("%d", iret);
    Serial.println("ERR:CAqTkPicoF_SetKoe");
  }

  DAC_start();
  for(;;){
    int16_t wav[LEN_FRAME];
    uint16_t len;
    iret = CAqTkPicoF_SyntheFrame(wav, &len);
    if(iret) break; // EOD
    
    DAC_write((int)len, wav);
  }
  DAC_stop();
}

static void notifyCallback(char* pData) {
  if(mStatus == DISP_STATUS_MEASURED) {
    char playData[10] = {0};
    Serial.print("****data: ");
    Serial.println((char*)pData);
    dispFrame.disp_measured(pData, playData);
    if(strlen(playData)) {
      char koeData[32] = {0};
      sprintf(koeData, "<NUMK VAL=%s>\r",playData);
      playAquesTalk(koeData);
    }
  }
}

void setup() {
  M5.begin();

  adc_power_acquire(); // ADC Power ON

  if(CAqTkPicoF_Init(workbuf, LEN_FRAME, "XXX-XXX-XXX")){
    Serial.println("ERR:CAqTkPicoF_Init");
  }
  DAC_create();

  dispFrame.init();
  dispFrame.disp_connecting();

  blepos300.setup();
  blepos300.setNotifyCallback(notifyCallback);
}

void loop() {
    BLE_STATUS ble_st;

  if(mStatus == DISP_STATUS_NOT_CONNECT) {
    blepos300.doConnect();
    mStatus = DISP_STATUS_CONNECTING;
    delay(1000);
  }
  else if(mStatus == DISP_STATUS_CONNECTING) {
    ble_st = blepos300.getStatus();
    if(ble_st == BLE_STATUS_CONNECTING) {
      blepos300.doConnect();
      delay(1000);
    }
    else if(ble_st == BLE_STATUS_CONNECTED) {
      mStatus = DISP_STATUS_MEASURED;
      dispFrame.display_frame();
    }
  }
  else if(mStatus == DISP_STATUS_MEASURED) {
    ble_st = blepos300.getStatus();
    if(ble_st == BLE_STATUS_NOT_CONNECT) {
      mStatus = DISP_STATUS_NOT_CONNECT;
      dispFrame.disp_connecting();
    }
    else {
      if (M5.BtnA.wasReleased()) {
        dispFrame.select_measure_next();
      }
      else if (M5.BtnB.wasReleased()) {
        mStatus = DISP_STATUS_EDIT;
        kbd.setup();
        while (kbd.loop()) {
          delay(1);
        }
        String min_str = kbd.getString_1();
        String max_str = kbd.getString_2();
        kbd.close();
        if(dispFrame.disp_range(min_str, max_str) < 0) {
          // エラー表示？今は何もしない
        }
        mStatus = DISP_STATUS_MEASURED;
        dispFrame.display_frame(true);
      }
      else if (M5.BtnC.wasReleased()) {
        dispFrame.saveData();
      }
    }
  }

  m5.update();
}