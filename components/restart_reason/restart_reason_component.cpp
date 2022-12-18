#include "restart_reason_component.h"
#include <esphome/core/log.h>

#if defined(USE_ESP8266)
#include <user_interface.h>
#include <Arduino.h>
#elif defined(USE_ESP32_FRAMEWORK_ARDUINO)
#include <Esp.h>
#elif defined(USE_ESP_IDF)
#include <esp_system.h>
#include <esp_sleep.h>
#endif

static const char *const TAG = "reset_reason.text_sensor";

#if defined(USE_ESP32)
std::string esphome::restart_reason::RestartReasonComponent::get_startup_reason_() {
  esp_reset_reason_t reset_reason = ::esp_reset_reason();
  if (reset_reason == ESP_RST_DEEPSLEEP) {
    esp_sleep_source_t wake_reason = esp_sleep_get_wakeup_cause();
    if (wake_reason == ESP_SLEEP_WAKEUP_EXT0)
      return "ESP_SLEEP_WAKEUP_EXT0";
    if (wake_reason == ESP_SLEEP_WAKEUP_EXT0)
      return "ESP_SLEEP_WAKEUP_EXT0";
    if (wake_reason == ESP_SLEEP_WAKEUP_EXT1)
      return "ESP_SLEEP_WAKEUP_EXT1";
    if (wake_reason == ESP_SLEEP_WAKEUP_TIMER)
      return "ESP_SLEEP_WAKEUP_TIMER";
    if (wake_reason == ESP_SLEEP_WAKEUP_TOUCHPAD)
      return "ESP_SLEEP_WAKEUP_TOUCHPAD";
    if (wake_reason == ESP_SLEEP_WAKEUP_ULP)
      return "ESP_SLEEP_WAKEUP_ULP";
    if (wake_reason == ESP_SLEEP_WAKEUP_GPIO)
      return "ESP_SLEEP_WAKEUP_GPIO";
    if (wake_reason == ESP_SLEEP_WAKEUP_UART)
      return "ESP_SLEEP_WAKEUP_UART";
    return std::string{"WAKEUP_"} + to_string((int) reset_reason);
  }
  if (reset_reason == ESP_RST_UNKNOWN)
    return "ESP_RST_UNKNOWN";
  if (reset_reason == ESP_RST_POWERON)
    return "ESP_RST_POWERON";
  if (reset_reason == ESP_RST_SW)
    return "ESP_RST_SW";
  if (reset_reason == ESP_RST_PANIC)
    return "ESP_RST_PANIC";
  if (reset_reason == ESP_RST_INT_WDT)
    return "ESP_RST_INT_WDT";
  if (reset_reason == ESP_RST_TASK_WDT)
    return "ESP_RST_TASK_WDT";
  if (reset_reason == ESP_RST_WDT)
    return "ESP_RST_WDT";
  if (reset_reason == ESP_RST_BROWNOUT)
    return "ESP_RST_BROWNOUT";
  if (reset_reason == ESP_RST_SDIO)
    return "ESP_RST_SDIO";
  return std::string{"RESET_"} + to_string((int) reset_reason);
}
#endif

#if defined(USE_ESP8266)
std::string esphome::restart_reason::RestartReasonComponent::get_startup_reason_() {
  rst_info *info = ESP.getResetInfoPtr();
  if (info->reason == REASON_DEFAULT_RST)
    return "REASON_DEFAULT_RST";
  if (info->reason == REASON_WDT_RST)
    return "REASON_WDT_RST";
  if (info->reason == REASON_EXCEPTION_RST)
    return "REASON_EXCEPTION_RST";
  if (info->reason == REASON_SOFT_WDT_RST)
    return "REASON_SOFT_WDT_RST";
  if (info->reason == REASON_SOFT_RESTART)
    return "REASON_SOFT_RESTART";
  if (info->reason == REASON_DEEP_SLEEP_AWAKE)
    return "REASON_DEEP_SLEEP_AWAKE";
  if (info->reason == REASON_EXT_SYS_RST)
    return "REASON_EXT_SYS_RST";
  return std::string{"RESET_"} + to_string((int) info->reason);
}
#endif

void esphome::restart_reason::RestartReasonComponent::dump_config() { LOG_TEXT_SENSOR("", "Reset Reason Sensor", this); }

float esphome::restart_reason::RestartReasonComponent::get_setup_priority() const { return setup_priority::DATA; }
void esphome::restart_reason::RestartReasonComponent::setup() {
  set_timeout(15000, [this]() { this->publish_state(get_startup_reason_()); });
}

