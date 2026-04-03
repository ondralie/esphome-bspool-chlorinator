#include "bs_pool_text_sensor.h"

#include <string>

#include "esphome/core/log.h"

namespace esphome {
namespace bs_pool {

static const char *const TAG = "bs_pool.text_sensor";

void BSPoolTextSensor::dump_config() { ESP_LOGCONFIG(TAG, "BSPool:"); }

const std::vector<FunctionCode> BSPoolTextSensor::codes_to_poll() {
  std::vector<FunctionCode> active_codes;
  if (this->time_text_sensor_ != nullptr)
    active_codes.push_back(FunctionCode::TIME);
  if (this->version_text_sensor_ != nullptr)
    active_codes.push_back(FunctionCode::SOFTWARE_VERSION);
  if (this->chlorinator_type_text_sensor_ != nullptr)
    active_codes.push_back(FunctionCode::CHLORINATOR_TYPE);
  return active_codes;
}

void BSPoolTextSensor::handle_message(DataPacket &message) {
  switch (message.function_code) {
    case FunctionCode::TIME:
      if (this->time_text_sensor_ != nullptr){
        std::string device_time = std::to_string(message.data_b3) + ":" +
                                  (message.data_b2 < 10 ? "0" : "") + std::to_string(message.data_b2);
        this->time_text_sensor_->publish_state(device_time);
      }
      break;
    case FunctionCode::SOFTWARE_VERSION:
      if (this->version_text_sensor_ != nullptr) {
        std::string version = std::to_string(message.data_b2) + "." + std::to_string(message.data_b3);
        this->version_text_sensor_->publish_state(version);
      }
      break;
    case FunctionCode::CHLORINATOR_TYPE:
      if (this->chlorinator_type_text_sensor_ != nullptr) {
        static const char *const TYPES[] = {
            "P927 10g/h", "P927 15g/h", "P950 20g/h", "P950 25g/h",
            "P953 35g/h", "P956 50g/h", "P956 70g/h", "P957 100g/h",
            "P957 150g/h", "P954 200g/h", "P910 35g/h", "P910 50g/h",
            "P910 70g/h", "P910 100g/h", "P910 200g/h"};
        if (message.data_b2 < 15)
          this->chlorinator_type_text_sensor_->publish_state(TYPES[message.data_b2]);
        else
          this->chlorinator_type_text_sensor_->publish_state("Unknown (" + std::to_string(message.data_b2) + ")");
      }
      break;
  }
}

}  // namespace bs_pool
}  // namespace esphome
