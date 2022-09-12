#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/gpio.h"

#include <vector>

#include "span.hpp"

namespace esphome {
namespace tmf8x01 {
enum class sensor_mode_t { PROXIMITY, DISTANCE, COMBINED };
enum class calibration_mode_t { NONE, CALIBRATE, CALIBRATE_AND_ALGO_STATE };

class Tmf8x01Sensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void set_interrupt_pin(GPIOPin *interrupt_pin);
  void set_enable_pin(GPIOPin *enable_pin);
  void set_calibration_mode(const calibration_mode_t &calibration_mode);
  void set_sensor_mode(const sensor_mode_t &sensor_mode);

  float get_setup_priority() const override { return setup_priority::DATA; }
  void dump_config() override;
  void setup() override;
  void update() override;

  uint16_t get_distance();

  bool write_bytes(uint8_t a_register, nonstd::span<const uint8_t> data) {
    return I2CDevice::write_bytes(a_register, data.data(), data.size());
  }

 private:
  bool wait_for_cpu_ready_();
  bool wait_for_boot_loader_();
  bool wait_for_application_();
  uint8_t get_application_id_();
  uint8_t get_cpu_state_();
  bool load_application_();
  bool load_boot_loader_();
  bool download_ram_patch_();
  bool get_status_ack_();
  uint8_t calculate_checksum_(nonstd::span<const uint8_t> buffer);
  uint8_t get_register_contents_();
  bool wait_for_status_register_(uint8_t status);
  void modify_command_set_(uint8_t index, uint8_t bit, bool value);
  uint32_t get_unique_id_();
  uint16_t get_model_();
  void do_calibration_();
  void stop_command_();
  void stop_measurement_();
  nonstd::span<const uint8_t> get_calibration_data_();
  nonstd::span<const uint8_t> get_algo_state_data_();
  std::vector<uint8_t> get_default_command_set_();

 private:
  uint8_t time_index_ = 0;
  bool measure_active_flag_ = false;
  std::array<uint32_t,5> host_time_{};
  std::array<uint32_t,5> moudle_time_{};
  std::vector<uint8_t> measure_command_set_{};
  GPIOPin *interrupt_pin_{nullptr};
  GPIOPin *enable_pin_{nullptr};
  calibration_mode_t calibration_mode_ = calibration_mode_t::NONE;
  sensor_mode_t sensor_mode_ = sensor_mode_t::PROXIMITY;
};

}  // namespace tmf8x01
}  // namespace esphome
