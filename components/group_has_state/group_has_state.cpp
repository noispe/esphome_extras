#include "group_has_state.h"

#include "esphome/core/log.h"

#define TAG this->get_component_source()

void esphome::group_has_state::GroupHasState::dump_config() {
  ESP_LOGCONFIG(TAG, "Setup group_has_state tracker");
  LOG_UPDATE_INTERVAL(this);
  ESP_LOGCONFIG(TAG, "%d binary sensors", this->binary_sensors_.size());
  for (auto s : this->binary_sensors_) {
    LOG_BINARY_SENSOR("   ", "Binary sensor", s);
  }

#ifdef USE_SENSOR
  ESP_LOGCONFIG(TAG, "%d sensors", this->sensors_.size());
  for (auto s : this->sensors_) {
    LOG_SENSOR("   ", "Sensor", s);
  }
#else
  ESP_LOGCONFIG(TAG, "No sensors added");
#endif

#ifdef USE_TEXT_SENSOR
  ESP_LOGCONFIG(TAG, "%d text sensors", this->text_sensors_.size());
  for (auto s : this->text_sensors_) {
    LOG_TEXT_SENSOR("   ", "Text", s);
  }
#else
  ESP_LOGCONFIG(TAG, "No text sensors added");
#endif

#ifdef USE_SELECT
  ESP_LOGCONFIG(TAG, "%d selects", this->select_sensors_.size());
  for (auto s : this->select_sensors_) {
    LOG_SELECT("   ", "Select", s);
  }
#else
  ESP_LOGCONFIG(TAG, "No selects added");
#endif

#ifdef USE_NUMBER
  ESP_LOGCONFIG(TAG, "%d numbers", this->number_sensors_.size());
  for (auto s : this->number_sensors_) {
    LOG_NUMBER("   ", "Number", s);
  }
#else
  ESP_LOGCONFIG(TAG, "No numbers added");
#endif
}

void esphome::group_has_state::GroupHasState::update() {
  int ready = 0;
  int total = 0;
  bool is_ok = true;
  for (auto s : this->binary_sensors_) {
    ESP_LOGVV(TAG, "%s -> %d", s->get_object_id().c_str(), s->has_state());
    total++;
    ready += s->has_state() ? 1 : 0;
    is_ok &= s->has_state();
  }

#ifdef USE_SENSOR
  for (auto s : this->sensors_) {
    ESP_LOGVV(TAG, "%s -> %d", s->get_object_id().c_str(), s->has_state());
    total++;
    ready += s->has_state() ? 1 : 0;
    is_ok &= s->has_state();
  }
#endif

#ifdef USE_TEXT_SENSOR
  for (auto s : this->text_sensors_) {
    ESP_LOGVV(TAG, "%s -> %d", s->get_object_id().c_str(), s->has_state());
    total++;
    ready += s->has_state() ? 1 : 0;
    is_ok &= s->has_state();
  }
#endif

#ifdef USE_SELECT
  for (auto s : this->select_sensors_) {
    ESP_LOGVV(TAG, "%s -> %d", s->get_object_id().c_str(), s->has_state());
    is_ok &= s->has_state();
  }
#endif

#ifdef USE_NUMBER
  for (auto s : this->number_sensors_) {
    ESP_LOGVV(TAG, "%s -> %d", s->get_object_id().c_str(), s->has_state());
    total++;
    ready += s->has_state() ? 1 : 0;
    is_ok &= s->has_state();
  }
#endif
  ESP_LOGV(TAG, "%d of %d ready, all ready %d", ready, total, is_ok);
  this->publish_state(is_ok);
}
