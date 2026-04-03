#pragma once

#include "../bs_pool.h"
#include "esphome/components/select/select.h"

namespace esphome {
namespace bs_pool {

class BSPoolSelect : public BSPoolListener,
                     public Component,
                     public Parented<BSPool> {
 public:
  void dump_config() override;

  const std::vector<FunctionCode> codes_to_poll() override;
  void handle_message(DataPacket &message) override;

  void send_command(uint8_t code, uint8_t b2, uint8_t b3 = 0x04);

  SUB_SELECT(control_mode);
  SUB_SELECT(language);
  SUB_SELECT(relay_status);
};

}  // namespace bs_pool
}  // namespace esphome
