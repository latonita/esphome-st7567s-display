#pragma once

#include "esphome/components/display/display_buffer.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace st7567s {

class ST7567S;

using st7567s_writer_t = std::function<void(ST7567S &)>;

class ST7567S : public PollingComponent,
                public display::DisplayBuffer,
                public i2c::I2CDevice {
public:
  void set_height(uint16_t height) { this->height_ = height; }
  void set_width(uint16_t width) { this->width_ = width; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;
  void fill(Color color) override;
  void clear(bool invcolor);
  void write_display_data();
  void set_contrast(uint8_t val); // 0..31
  display::DisplayType get_display_type() override {
    return display::DisplayType::DISPLAY_TYPE_BINARY;
  }

protected:
  int get_height_internal() override;
  int get_width_internal() override;
  size_t get_buffer_length_();

  void display_init_();
  void command_(uint8_t value);

  void draw_absolute_pixel_internal(int x, int y, Color color) override;

  int16_t width_ = 128, height_ = 64;
};

} // namespace st7567s
} // namespace esphome
