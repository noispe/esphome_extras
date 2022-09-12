#include "tmf8x01.h"

#include <algorithm>
#include <numeric>
#include <type_traits>

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

#include "TMF8701.h"
#include "TMF8801.h"

namespace esphome {
namespace tmf8x01 {

static const char *const TAG = "tmf8x01";

namespace detail {

template<typename T, size_t N, typename VT = std::vector<typename std::remove_const<T>::type> >
VT progmem_copy(nonstd::span<T, N> mem) {
  VT out{};
  std::transform(mem.begin(), mem.end(), std::back_inserter(out), [](const T &b) { return progmem_read_byte(&b); });
  return out;
}
}  // namespace detail

static constexpr uint16_t mtf_8801 = 0x4120;
static constexpr uint16_t mtf_8701 = 0x5e10;
static constexpr uint8_t reg_mtf8x01_enable = 0xe0;
static constexpr uint8_t reg_mtf8x01_appid = 0x00;
static constexpr uint8_t reg_mtf8x01_id = 0xe3;
static constexpr uint8_t reg_mtf8x01_appreqid = 0x02;

static constexpr uint8_t reg_mtf8x01_cmd_data9 = 0x06;
static constexpr uint8_t reg_mtf8x01_cmd_data8 = 0x07;
static constexpr uint8_t reg_mtf8x01_cmd_data7 = 0x08;
static constexpr uint8_t reg_mtf8x01_cmd_data6 = 0x09;
static constexpr uint8_t reg_mtf8x01_cmd_data5 = 0x0a;
static constexpr uint8_t reg_mtf8x01_cmd_data4 = 0x0b;
static constexpr uint8_t reg_mtf8x01_cmd_data3 = 0x0c;
static constexpr uint8_t reg_mtf8x01_cmd_data2 = 0x0d;
static constexpr uint8_t reg_mtf8x01_cmd_data1 = 0x0e;
static constexpr uint8_t reg_mtf8x01_cmd_data0 = 0x0f;
static constexpr uint8_t reg_mtf8x01_command = 0x10;
static constexpr uint8_t reg_mtf8x01_factorycalib = 0x20;
static constexpr uint8_t reg_mtf8x01_statedatawr = 0x2e;
static constexpr uint8_t reg_mtf8x01_status = 0x1d;
static constexpr uint8_t reg_mtf8x01_contents = 0x1e;
static constexpr uint8_t reg_mtf8x01_result_number = 0x20;
static constexpr uint8_t reg_mtf8x01_version_major = 0x01;
static constexpr uint8_t reg_mtf8x01_version_minorandpatch = 0x12;
static constexpr uint8_t reg_mtf8x01_version_hw = 0xe3;
static constexpr uint8_t reg_mtf8x01_version_serialnum = 0x28;
static constexpr uint8_t reg_mtf8x01_int_enab = 0xe2;
static constexpr uint8_t reg_mtf8x01_int_status = 0xe1;
static constexpr uint8_t reg_mtf8x01_tj = 0x32;

static constexpr uint16_t model_tmf8801 = 0x4120;
static constexpr uint16_t model_tmf8701 = 0x5e10;

// enum cmdset
static constexpr uint8_t cmdset_index_cmd6 = 0x01;
static constexpr uint8_t cmdset_bit_proximity = 0x00;
static constexpr uint8_t cmdset_bit_distance = 0x01;
static constexpr uint8_t cmdset_bit_int = 0x04;
static constexpr uint8_t cmdset_bit_combine = 0x05;

static constexpr uint8_t cmdset_index_cmd7 = 0x00;
static constexpr uint8_t cmdset_bit_calib = 0x00;
static constexpr uint8_t cmdset_bit_algo = 0x01;

// board status
static constexpr uint8_t sta_ok = 0x00;
static constexpr uint8_t sta_err = 0x01;
static constexpr uint8_t sta_err_device_not_detected = 0x02;
static constexpr uint8_t sta_err_soft_version = 0x03;
static constexpr uint8_t sta_err_parameter = 0x04;

// calibration
static constexpr uint8_t tmf8801_calib_data[] = {0x41, 0x57, 0x01, 0xFD, 0x04, 0x00, 0x00,
                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
static constexpr size_t tmf8801_calib_data_size = sizeof(tmf8801_calib_data);
static constexpr uint8_t tmf8801_algo_state[] = {0xB1, 0xA9, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static constexpr size_t tmf8801_algo_state_size = sizeof(tmf8801_algo_state);

static constexpr uint8_t tmf8801_cmd_set[] = {0x01, 0xA3, 0x00, 0x00, 0x00, 0x64, 0x03, 0x84, 0x02};
static constexpr size_t tmf8801_cmd_set_size = sizeof(tmf8801_cmd_set);

static constexpr uint8_t tmf8701_calib_data[] = {0x41, 0x57, 0x01, 0xFD, 0x04, 0x00, 0x00,
                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
static constexpr size_t tmf8701_calib_data_size = sizeof(tmf8701_calib_data);

static constexpr uint8_t tmf8701_algo_state[] = {0xB1, 0xA9, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static constexpr size_t tmf8701_algo_state_size = sizeof(tmf8701_algo_state);

static constexpr uint8_t tmf8701_cmd_set[] = {0x01, 0xA3, 0x00, 0x00, 0x00, 0x64, 0x03, 0x84, 0x02};
static constexpr size_t tmf8701_cmd_set_size = sizeof(tmf8701_cmd_set);

// ram patch
// constexpr uint8_t PROGMEM DFRobot_TMF8701_initBuf[] = {0};
// static constexpr size_t DFRobot_TMF8701_initBuf_size = sizeof(DFRobot_TMF8701_initBuf);

// constexpr uint8_t PROGMEM DFRobot_TMF8801_initBuf[] = {0};
// static constexpr size_t DFRobot_TMF8801_initBuf_size = sizeof(DFRobot_TMF8801_initBuf);

// Helpers from https://github.com/DFRobot/DFRobot_TMF8x01/blob/master/src/DFRobot_TMF8x01.h

/**
 * @union eEnableReg_t
 * @brief  MTF8x01‘s ENABLE register：
 * @n---------------------------------------------------------------------
 * @n|    b7    |    b6    |  b5  |  b4  |  b3  |  b2  |  b1  |    b0    |
 * @n---------------------------------------------------------------------
 * @n| cpu_reset| cpu_ready|            reserve               |    pon   |
 * @n---------------------------------------------------------------------
 */
typedef union {
  struct {
    uint8_t pon : 1; /**< Writing a '1' to this register will enable oscillator and Writing a '0' will set device into
                        standby state. */
    uint8_t reserve : 5;  /**< reserved bits */
    uint8_t cpuReady : 1; /**< ro bit, to determine whether the CPU of the TMF8x01 device is ready, if it is ready, the
                             value is 1.*/
    uint8_t cpuReset : 1; /**< rw, write a '1' to reset cpu. all config of TMF8x01 will reset. */
  };
  uint8_t value; /**< The value of enable register.*/
} __attribute__((packed)) enable_reg_t;

// 11 bytes
typedef struct {
  uint8_t status;       /**< Current status or current general operation.*/
  uint8_t regContents;  /**< Current contents of the I²C RAM from 0x20 to 0xEF, the coding is as follows:
                           0x0A(Calibration data) 0x47(Serial number) 0x55(Results for commands 0x02/0x03 and 0x04*/
  uint8_t tid;          /**< Unique transaction ID, changes with every update of register map by TOF.*/
  uint8_t resultNumber; /**< Result number, incremented every time there is a unique answer.*/
  struct {
    uint8_t reliability : 6; /**< Reliability of object - valid range 0..63 where 63 is best.*/
    uint8_t meastatus : 2;   /**< Will indicate the status of the measurement.*/
  } result_info_t;
  uint16_t dis;      /**< Distance to the peak in [mm] of the object*/
  uint32_t sysclock; /**< System clock/time stamp in units of 0.2 µs.*/
} result_t;

bool Tmf8x01Sensor::wait_for_application_() {
  int retry = 10;
  while (retry-- != 0) {
    if (this->get_application_id_() == 0XC0) {
      return true;
    }
    delay_microseconds_safe(100);
  }
  // LOG Error

  return false;
}

bool Tmf8x01Sensor::wait_for_cpu_ready_() {
  int retry = 10;
  while (retry-- != 0) {
    if (this->get_cpu_state_() == 0x41) {
      return true;
    }
    delay_microseconds_safe(100);
  }
  // LOG Error

  return false;
}

bool Tmf8x01Sensor::wait_for_boot_loader_() {
  int retry = 10;
  while (retry-- != 0) {
    if (this->get_application_id_() == 0x80) {
      return true;
    }
    delay_microseconds_safe(100);
  }
  // LOG Error
  return false;
}

uint8_t Tmf8x01Sensor::get_application_id_() {
  auto id = this->read_byte(reg_mtf8x01_appid);
  if (id.has_value()) {
    return id.value();
  }
  this->status_set_warning();
  return 0;
}

uint8_t Tmf8x01Sensor::get_cpu_state_() {
  auto state = this->read_byte(reg_mtf8x01_enable);
  if (state.has_value()) {
    return state.value();
  }
  this->status_set_warning();
  return 0;
}

bool Tmf8x01Sensor::load_application_() {
  this->write_byte(reg_mtf8x01_appreqid, 0xc0);
  return this->wait_for_application_();
}

bool Tmf8x01Sensor::load_boot_loader_() {
  this->write_byte(reg_mtf8x01_appreqid, 0x80);
  return this->wait_for_boot_loader_();
}

bool Tmf8x01Sensor::download_ram_patch_() {
  if (this->get_application_id_() != 0x80) {
    if (!this->load_boot_loader_()) {
      return false;
    }
  }

  const uint8_t buffer1[] = {0x14, 0x01, 0x29};
  I2CDevice::write_bytes(reg_mtf8x01_cmd_data7, buffer1, sizeof(buffer1));
  if (!get_status_ack_()) {
    return false;
  }
  const uint8_t buffer2[] = {0x43, 0x02, 0x00, 0x00};
  I2CDevice::write_bytes(reg_mtf8x01_cmd_data7, buffer2, sizeof(buffer2));
  if (!get_status_ack_()) {
    return false;
  }

  const uint8_t *init_ptr = nullptr;
  if (this->get_model_() == model_tmf8801) {
    init_ptr = reinterpret_cast<const uint8_t *>(DFRobot_TMF8801_initBuf);
  } else if (this->get_model_() == model_tmf8701) {
    init_ptr = reinterpret_cast<const uint8_t *>(DFRobot_TMF8701_initBuf);
  } else {
    return false;
  }

  while (uint8_t flag = progmem_read_byte(init_ptr++) > 0) {
    std::vector<uint8_t> buffer{};
    buffer.push_back(0x41);
    buffer.push_back(flag);
    size_t idx = 0;
    for (uint8_t idx = 0; idx < flag; idx++) {
      buffer.push_back(progmem_read_byte(init_ptr++));
    }
    buffer.push_back(calculate_checksum_(buffer));
    this->write_bytes(reg_mtf8x01_cmd_data7, buffer);
    if (!get_status_ack_()) {
      return false;
    }
  }

  const uint8_t buffer3[] = {0x11, 0x00};
  I2CDevice::write_bytes(reg_mtf8x01_cmd_data7, buffer3, sizeof(buffer3));

  return this->wait_for_cpu_ready_();
}

bool Tmf8x01Sensor::get_status_ack_() {
  auto res = read_bytes<3>(reg_mtf8x01_cmd_data7);
  if (!res.has_value()){
    return false;
  }
  const auto bytes = res.value();
  if (encode_uint24(bytes[0], bytes[1],bytes[2]) == 0xFF0000) {
    return true;
  }
  return false;
}

uint8_t Tmf8x01Sensor::calculate_checksum_(nonstd::span<const uint8_t> buffer) {
  return std::accumulate(buffer.begin(), buffer.end(), 0) ^ 0xFF;
}

uint8_t Tmf8x01Sensor::get_register_contents_() {
  auto value = this->read_byte(reg_mtf8x01_contents);
  if (value.has_value()) {
    return value.value();
  }
  this->status_set_warning();
  return 0;
}

bool Tmf8x01Sensor::wait_for_status_register_(uint8_t status) {
  int retry = 10;
  while (retry-- != 0) {
    if (this->get_register_contents_() == status) {
      return true;
    }
    delay_microseconds_safe(100);
  }
  // LOG Error

  return false;
}

void Tmf8x01Sensor::modify_command_set_(uint8_t index, uint8_t bit, bool value) {
  if (index > (measure_command_set_.size() - 1) || bit > 7) {
    return;
  }
  if (value) {
    measure_command_set_[index] |= (1 << bit);
  } else {
    measure_command_set_[index] = ~((~measure_command_set_[index]) | (1 << bit));
  }
}

uint32_t Tmf8x01Sensor::get_unique_id_() {
  this->write_byte(reg_mtf8x01_command, 0x47);
  int retry = 10;
  while (retry-- != 0) {
    if (this->read_byte(reg_mtf8x01_contents) == 0x47) {
      auto res = this->read_bytes<4>(reg_mtf8x01_version_serialnum);
      if (res.has_value()) {
        this->stop_command();
        return encode_value<uint32_t>(res.value());
      }
    }
    delay_microseconds_safe(100);
  }
  // LOG Error
  return 0;
}

uint16_t Tmf8x01Sensor::get_model_() { return (this->get_unique_id_() >> 16) & 0xFFFF; }

uint16_t Tmf8x01Sensor::get_distance() { return 0; }

void Tmf8x01Sensor::do_calibration_() {
  if (calibration_mode_ == calibration_mode_t::CALIBRATE) {
    this->modify_command_set_(cmdset_index_cmd7, cmdset_bit_calib, true);
    this->modify_command_set_(cmdset_index_cmd7, cmdset_bit_algo, false);
    this->write_byte(reg_mtf8x01_command, 0x0B);
    this->write_bytes(reg_mtf8x01_result_number, this->get_calibration_data_());
  } else if (calibration_mode_ == calibration_mode_t::CALIBRATE_AND_ALGO_STATE) {
    this->modify_command_set_(cmdset_index_cmd7, cmdset_bit_calib, true);
    this->modify_command_set_(cmdset_index_cmd7, cmdset_bit_algo, true);
    this->write_byte(reg_mtf8x01_command, 0x0b);
    this->write_bytes(reg_mtf8x01_result_number, this->get_calibration_data_());
    this->write_bytes(reg_mtf8x01_statedatawr, this->get_algo_state_data_());
  } else {
    this->modify_command_set_(cmdset_index_cmd7, cmdset_bit_calib, false);
    this->modify_command_set_(cmdset_index_cmd7, cmdset_bit_algo, false);
  }
  this->write_bytes(reg_mtf8x01_cmd_data7, this->measure_command_set_);
  delay_microseconds_safe(10000);
  if (!this->wait_for_status_register_(0x55)) {
    this->status_set_error();
    return;
  }
  // data ready

}

nonstd::span<const uint8_t> Tmf8x01Sensor::get_calibration_data_() {
  auto model = this->get_model_();
  if (model == model_tmf8801) {
    return nonstd::span<const uint8_t, tmf8801_calib_data_size>{tmf8801_calib_data};
  } else if (model == model_tmf8701) {
    return nonstd::span<const uint8_t, tmf8701_calib_data_size>{tmf8701_calib_data};
  } else {
    // error
    return nonstd::span<const uint8_t>{};
  }
}

nonstd::span<const uint8_t> Tmf8x01Sensor::get_algo_state_data_() {
  auto model = this->get_model_();
  if (model == model_tmf8801) {
    return nonstd::span<const uint8_t, tmf8801_algo_state_size>{tmf8801_algo_state};
  } else if (model == model_tmf8701) {
    return nonstd::span<const uint8_t, tmf8701_algo_state_size>{tmf8701_algo_state};
  } else {
    // error
    return nonstd::span<const uint8_t>{};
  }
}

std::vector<uint8_t> Tmf8x01Sensor::get_default_command_set_() {
  auto model = this->get_model_();
  if (model == model_tmf8801) {
    return detail::progmem_copy(nonstd::span<const uint8_t, tmf8801_cmd_set_size>{tmf8801_cmd_set});
  } else if (model == model_tmf8701) {
    return detail::progmem_copy(nonstd::span<const uint8_t, tmf8701_cmd_set_size>{tmf8701_cmd_set});
  } else {
    // error
    return std::vector<uint8_t>{};
  }
}

void Tmf8x01Sensor::set_enable_pin(GPIOPin *enable_pin) { enable_pin_ = enable_pin; }

void Tmf8x01Sensor::set_interrupt_pin(GPIOPin *interrupt_pin) { interrupt_pin_ = interrupt_pin; }

void Tmf8x01Sensor::set_calibration_mode(const calibration_mode_t &calibration_mode) {
  calibration_mode_ = calibration_mode;
}

void Tmf8x01Sensor::set_sensor_mode(const sensor_mode_t &sensor_mode) { sensor_mode_ = sensor_mode; }

void Tmf8x01Sensor::setup() {
  if (this->enable_pin_ != nullptr) {
    this->enable_pin_->setup();
    this->enable_pin_->digital_write(false);
    delay_microseconds_safe(10000);
    this->enable_pin_->digital_write(true);
  }
  if (this->interrupt_pin_ != nullptr) {
    this->interrupt_pin_->setup();
  }
  delay_microseconds_safe(10000);

  this->write_byte(reg_mtf8x01_enable, 0x01);
  if (!this->wait_for_cpu_ready_()) {
    this->status_set_error();
    return;
  }
  if (this->get_application_id_() == 0x80) {
    // download ram patch

    if (!this->download_ram_patch_()) {
      this->status_set_error();
      return;
    }
    if(!this->wait_for_application_()) {
      this->load_application_();
    }
    if(!this->wait_for_application_()) {
      this->status_set_error();
      return;
    }
  }
  measure_command_set_ = this->get_default_command_set_();

  this->do_calibration_();
}

void Tmf8x01Sensor::dump_config() {
  LOG_SENSOR("", "DFR_TMF8X01", this);
  LOG_UPDATE_INTERVAL(this);
  LOG_I2C_DEVICE(this);
  LOG_PIN("Enable Pin:", this->enable_pin_);
  LOG_PIN("Interrupt Pin:", this->interrupt_pin_);
  ESP_LOGCONFIG(TAG, "Mode: %d", this->sensor_mode_);
  ESP_LOGCONFIG(TAG, "Calibration: %d", this->calibration_mode_);
}

void Tmf8x01Sensor::update() {}

void Tmf8x01Sensor::stop_command_() {
  write_byte(reg_mtf8x01_command,0xff);
  delay_microseconds_safe(500);
}

void Tmf8x01Sensor::stop_measurement_() {
  this->measure_active_flag_ = false;
  this->stop_command_();
  this->time_index_ = 0;

}

}  // namespace tmf8x01
}  // namespace esphome
