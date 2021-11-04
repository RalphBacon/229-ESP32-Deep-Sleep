 /*
  ESP32 (Deep) Sleep Demo. How to restore variable values after sleep. Current Consumption
*/
#include <Arduino.h>
#define LED_PIN 19

// Only RTC GPIOs can be used as a source for external wake-up.
// They are: 0,2,4,12-15,25-27,32-39. Note pins 34-39 are INPUT only (not PULLUP).
#define BTN_PIN 26

// Define a few time periods
#define SECONDS_TO_HANG_ABOUT 10
#define SECONDS_TO_SLEEP 10
#define uS_TO_S_FACTOR 1000000

// Place all variables you want to retain across sleep periods in the 
// RTC SRAM. 8kB SRAM on the RTC for all variables.
RTC_DATA_ATTR int bootCount = 0;

// Initialisation. This code is executed after EVERY wakeup. Your code
// logic must determine how to deal with the wake-up type.
void setup() {

  // Debugging
  Serial.begin(115200);

  // Why are we awake?
  getWakeUpReason();

  // Pin Modes
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  // All done
  log_d("Setup completed");
}

// Just flash the LED to show we are running
void loop() {
  if (millis() > SECONDS_TO_HANG_ABOUT * 1000) {
    gotoSleep();
  }

  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(250);
}

//Put the ESP32 device to sleep, woken by the RTC and/or external switch
void gotoSleep() {

  log_d("Going to sleep");
  
  // Allow wake up if Button Switch is pressed
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BTN_PIN, LOW);  // could use: GPIO_NUM_26

  // Allow RTC to wake up this device after X seconds
  esp_sleep_enable_timer_wakeup(SECONDS_TO_SLEEP * uS_TO_S_FACTOR);

  // Sleep starts
  esp_deep_sleep_start();
}

void getWakeUpReason() {
  log_w("This is (re)boot #%d", ++bootCount);

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      log_w("Wakeup caused by external signal using RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      log_w("Wakeup caused by external signal using RTC_CNTL");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      log_w("Wakeup caused by RTC timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      log_w("Wakeup caused by touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      log_w("Wakeup caused by ULP program");
      break;
    default:
      log_w("Wakeup was not caused by deep sleep: %d", wakeup_reason);
  }
}