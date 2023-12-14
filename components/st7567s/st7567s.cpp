#include "st7567s.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"

// ST7567 Commands
#define ST7567_BOOSTER_ON 0x2C     // internal power supply on
#define ST7567_REGULATOR_ON 0x2E   // internal power supply on
#define ST7567_POWER_ON 0x2F       // internal power supply on
#define ST7567_POWER_CTL 0x28      // internal power supply off
#define ST7567_CONTRAST 0x80       // 0x80 + (0..31)
#define ST7567_SEG_NORMAL 0xA0     // SEG remap normal
#define ST7567_SEG_REMAP 0xA1      // SEG remap reverse (flip horizontal)
#define ST7567_DISPLAY_NORMAL 0xA4 // display ram content
#define ST7567_DISPLAY_TEST 0xA5   // all pixels on
#define ST7567_INVERT_OFF 0xA6     // not inverted
#define ST7567_INVERT_ON 0xA7      // inverted
#define ST7567_DISPLAY_ON 0XAF     // display on
#define ST7567_DISPLAY_OFF 0XAE    // display off
#define ST7567_STATIC_OFF 0xAC
#define ST7567_STATIC_ON 0xAD
#define ST7567_SCAN_START_LINE 0x40 // scrolling 0x40 + (0..63)
#define ST7567_COM_NORMAL 0xC0      // COM remap normal
#define ST7567_COM_REMAP 0xC8       // COM remap reverse (flip vertical)
#define ST7567_SW_RESET 0xE2        // connect RST pin to GND and rely on software reset
#define ST7567_NOP 0xE3             // no operation
#define ST7567_TEST 0xF0

#define ST7567_COL_ADDR_H 0x10 // x pos (0..95) 4 MSB
#define ST7567_COL_ADDR_L 0x00 // x pos (0..95) 4 LSB
#define ST7567_PAGE_ADDR 0xB0  // y pos, 8.5 rows (0..8)
#define ST7567_RMW 0xE0
#define ST7567_RMW_CLEAR 0xEE

#define ST7567_BIAS_9 0xA2
#define ST7567_BIAS_7 0xA3

#define ST7567_VOLUME_FIRST 0x81
#define ST7567_VOLUME_SECOND 0x00

#define ST7567_RESISTOR_RATIO 0x20
#define ST7567_STATIC_REG 0x0
#define ST7567_BOOSTER_FIRST 0xF8
#define ST7567_BOOSTER_234 0
#define ST7567_BOOSTER_5 1
#define ST7567_BOOSTER_6 3

namespace esphome {
namespace st7567s {

static const char *const TAG = "st7567s";

void ST7567S::setup() {
    ESP_LOGCONFIG(TAG, "Setting up ST7567S LCD Display...");
    dump_config();
    init_internal_(get_buffer_length_());
    display_init_();
}

void ST7567S::dump_config() {
    LOG_DISPLAY("", "ST7567S", this);
    ESP_LOGCONFIG(TAG, "  Height: %d", height_);
    ESP_LOGCONFIG(TAG, "  Width: %d", width_);
    LOG_I2C_DEVICE(this);
    LOG_UPDATE_INTERVAL(this);
}

float ST7567S::get_setup_priority() const { return setup_priority::PROCESSOR; }

void ST7567S::update() {
    do_update_();
    write_display_data();
}

void HOT ST7567S::draw_absolute_pixel_internal(int x, int y, Color color) {
    if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0)
        return;

    uint16_t pos = x + (y / 8) * this->get_width_internal();
    uint8_t subpos = y & 0x07;
    if (color.is_on()) {
        this->buffer_[pos] |= (1 << subpos);
    } else {
        this->buffer_[pos] &= ~(1 << subpos);
    }
}

int ST7567S::get_width_internal() { return width_; }

int ST7567S::get_height_internal() { return height_; }

size_t ST7567S::get_buffer_length_() { return size_t(get_width_internal()) * size_t(get_height_internal()) / 8u; }

void ST7567S::command_(uint8_t value) { write_register(0x00, &value, 1, true); }

#define POINTS_AT_ONCE 64
void HOT ST7567S::write_display_data() {
    command_(ST7567_SCAN_START_LINE);

    for (uint8_t y = 0; y < (uint8_t)get_height_internal() / 8; y++) {
        command_(ST7567_PAGE_ADDR + y); // Set Page
        command_(ST7567_COL_ADDR_H);    // Set MSB Column address
        command_(ST7567_COL_ADDR_L);    // Set LSB Column address

        // I2C bus: for some reason it doesn't work when sending 128 bytes at a time, sending 64
        for (uint8_t x = 0; x < (uint8_t)get_width_internal(); x += POINTS_AT_ONCE) {
            write_register(ST7567_SCAN_START_LINE, &buffer_[y * get_width_internal() + x],
                           get_width_internal() - x > POINTS_AT_ONCE ? POINTS_AT_ONCE : get_width_internal() - x, true);
        }

        App.feed_wdt();
    }
}

void ST7567S::fill(Color color) { memset(buffer_, color.is_on() ? 0xFF : 0x00, get_buffer_length_()); }

void ST7567S::clear(bool invcolor) {
    esphome::Color c;
    c.raw_32 = invcolor ? 0xFFFFFF : 0x00;
    fill(c);
    write_display_data();
}

void ST7567S::display_init_() {
    ESP_LOGD(TAG, "Initializing display...");

    command_(ST7567_BIAS_9);
    command_(ST7567_SEG_NORMAL);
    command_(ST7567_COM_REMAP);
    command_(ST7567_POWER_CTL | 0x4);
    command_(ST7567_POWER_CTL | 0x6);
    command_(ST7567_POWER_CTL | 0x7);
    //********Adjust display brightness********
    command_(0x25); // 0x20-0x27 is the internal Rb/Ra resistance
                    // adjustment setting of V5 voltage RR=4.5V
    // command_(ST7567_RESISTOR_RATIO | 0x6);
    command_(ST7567_BOOSTER_ON);
    command_(ST7567_REGULATOR_ON);
    command_(ST7567_POWER_ON);

    command_(ST7567_SCAN_START_LINE);
    command_(ST7567_DISPLAY_ON);
    command_(ST7567_DISPLAY_NORMAL);

    clear(false);
}

void ST7567S::set_contrast(uint8_t val) // 0..31
{
    // now write the new contrast level to the display (0x81)
    command_(ST7567_VOLUME_FIRST);
    command_(ST7567_VOLUME_SECOND | (val & 0x3f));
}
} // namespace st7567s
} // namespace esphome
