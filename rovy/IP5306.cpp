#include "IP5306.h"

void IP5306::begin(void) {
  Wire.begin();

  reg_SYS_CTL0_t.reg_byte = i2c_read(IP5306_ADDRESS, SYS_CTL0);
  reg_SYS_CTL1_t.reg_byte = i2c_read(IP5306_ADDRESS, SYS_CTL1);
  reg_SYS_CTL2_t.reg_byte = i2c_read(IP5306_ADDRESS, SYS_CTL2);

  reg_Charger_CTL0_t.reg_byte = i2c_read(IP5306_ADDRESS, Charger_CTL0);
  reg_Charger_CTL1_t.reg_byte = i2c_read(IP5306_ADDRESS, Charger_CTL1);
  reg_Charger_CTL2_t.reg_byte = i2c_read(IP5306_ADDRESS, Charger_CTL2);
  reg_Charger_CTL3_t.reg_byte = i2c_read(IP5306_ADDRESS, Charger_CTL3);
}

void IP5306::i2c_write(uint8_t slave_address, uint8_t register_address, uint8_t data) {
  Wire.beginTransmission(slave_address);
  Wire.write(register_address);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t IP5306::i2c_read(int slave_address, uint8_t register_address) {
  Wire.beginTransmission(slave_address);
  Wire.write(register_address);
  Wire.endTransmission();

  Wire.requestFrom(slave_address, 1);

  for (int i = 0; i < 10; i++) {
    int value = Wire.read();

    if (value > -1) {
      return uint8_t(value);
    }
    delay(1);
  }

  return 0;

}

void IP5306::boost_mode(uint8_t boost_en) {
  reg_SYS_CTL0_t.bits.BOOST_ENABLE = boost_en;

  i2c_write(IP5306_ADDRESS, SYS_CTL0, reg_SYS_CTL0_t.reg_byte);
}

void IP5306::charger_mode(uint8_t charger_en) {
  reg_SYS_CTL0_t.bits.CHARGER_ENABLE = charger_en;

  i2c_write(IP5306_ADDRESS, SYS_CTL0, reg_SYS_CTL0_t.reg_byte);
}

void IP5306::power_on_load(uint8_t power_on_en) {
  reg_SYS_CTL0_t.bits.POWER_ON_LOAD = power_on_en;

  i2c_write(IP5306_ADDRESS, SYS_CTL0, reg_SYS_CTL0_t.reg_byte);
}

void IP5306::boost_output(uint8_t output_val) {
  reg_SYS_CTL0_t.bits.SET_BOOST_OUTPUT_ENABLE = output_val;

  i2c_write(IP5306_ADDRESS, SYS_CTL0, reg_SYS_CTL0_t.reg_byte);
}

void IP5306::button_shutdown(uint8_t shutdown_val) {
  reg_SYS_CTL0_t.bits.BUTTON_SHUTDOWN = shutdown_val;

  i2c_write(IP5306_ADDRESS, SYS_CTL0, reg_SYS_CTL0_t.reg_byte);
}

void IP5306::boost_ctrl_signal(uint8_t press_val) {
  reg_SYS_CTL1_t.bits.BOOST_CTRL_SIGNAL_SELECTION = press_val;

  i2c_write(IP5306_ADDRESS, SYS_CTL1, reg_SYS_CTL1_t.reg_byte);
}

void IP5306::flashlight_ctrl_signal(uint8_t press_val) {
  reg_SYS_CTL1_t.bits.FLASHLIGHT_CTRL_SIGNAL_SELECTION = press_val;

  i2c_write(IP5306_ADDRESS, SYS_CTL1, reg_SYS_CTL1_t.reg_byte);
}

void IP5306::short_press_boost(uint8_t boost_en) {
  reg_SYS_CTL1_t.bits.SHORT_PRESS_BOOST_SWITCH_ENABLE = boost_en;

  i2c_write(IP5306_ADDRESS, SYS_CTL1, reg_SYS_CTL1_t.reg_byte);
}

void IP5306::boost_after_vin(uint8_t val) {
  reg_SYS_CTL1_t.bits.BOOST_AFTER_VIN = val;

  i2c_write(IP5306_ADDRESS, SYS_CTL1, reg_SYS_CTL1_t.reg_byte);
}

void IP5306::low_battery_shutdown(uint8_t shutdown_en) {
  reg_SYS_CTL1_t.bits.LOW_BATTERY_SHUTDOWN_ENABLE = shutdown_en;

  i2c_write(IP5306_ADDRESS, SYS_CTL1, reg_SYS_CTL1_t.reg_byte);
}

void IP5306::set_long_press_time(uint8_t press_time_val) {
  reg_SYS_CTL2_t.bits.LONG_PRESS_TIME = press_time_val;

  i2c_write(IP5306_ADDRESS, SYS_CTL2, reg_SYS_CTL2_t.reg_byte);
}

void IP5306::set_light_load_shutdown_time(uint8_t shutdown_time) {
  reg_SYS_CTL2_t.bits.LIGHT_LOAD_SHUTDOWN_TIME = shutdown_time;

  i2c_write(IP5306_ADDRESS, SYS_CTL2, reg_SYS_CTL2_t.reg_byte);
}

void IP5306::set_charging_stop_voltage(uint8_t voltage_val) {
  reg_Charger_CTL0_t.bits.CHARGING_FULL_STOP_VOLTAGE = voltage_val;

  i2c_write(IP5306_ADDRESS, Charger_CTL0, reg_Charger_CTL0_t.reg_byte);
}

void IP5306::end_charge_current(uint8_t current_val) {
  reg_Charger_CTL1_t.bits.END_CHARGE_CURRENT_DETECTION = current_val;

  i2c_write(IP5306_ADDRESS, Charger_CTL1, reg_Charger_CTL1_t.reg_byte);
}

void IP5306::charger_under_voltage(uint8_t voltage_val) {
  reg_Charger_CTL1_t.bits.CHARGE_UNDER_VOLTAGE_LOOP = voltage_val;

  i2c_write(IP5306_ADDRESS, Charger_CTL1, reg_Charger_CTL1_t.reg_byte);
}

void IP5306::set_battery_voltage(uint8_t voltage_val) {
  reg_Charger_CTL2_t.bits.BATTERY_VOLTAGE = voltage_val;

  i2c_write(IP5306_ADDRESS, Charger_CTL2, reg_Charger_CTL2_t.reg_byte);
}

void IP5306::set_voltage_pressure(uint8_t voltage_val) {
  reg_Charger_CTL2_t.bits.VOLTAGE_PRESSURE = voltage_val;

  i2c_write(IP5306_ADDRESS, Charger_CTL2, reg_Charger_CTL2_t.reg_byte);
}

void IP5306::set_cc_loop(uint8_t current_val) {
  reg_Charger_CTL3_t.bits.CHARGE_CC_LOOP = current_val;

  i2c_write(IP5306_ADDRESS, Charger_CTL3, reg_Charger_CTL3_t.reg_byte);
}

uint8_t IP5306::check_charging_status(void) {
  reg_READ0_t.reg_byte = i2c_read(IP5306_ADDRESS, REG_READ0);

  return reg_READ0_t.bits.CHARGE_ENABLE;
}

uint8_t IP5306::check_battery_status(void) {
  reg_READ1_t.reg_byte = i2c_read(IP5306_ADDRESS, REG_READ1);

  return reg_READ1_t.bits.BATTERY_STATUS;
}

uint8_t IP5306::check_load_level(void) {
  reg_READ2_t.reg_byte = i2c_read(IP5306_ADDRESS, REG_READ2);

  return reg_READ2_t.bits.LOAD_LEVEL;
}

uint8_t IP5306::get_battery_level(void) {
  uint8_t data = reg_READ2_t.reg_byte = i2c_read(IP5306_ADDRESS, REG_READ4);

  switch (data & 0xF0) {
    case 0xE0: return 25;
    case 0xC0: return 50;
    case 0x80: return 75;
    case 0x00: return 100;
    default: return 0;
  }
}

uint8_t IP5306::short_press_detect(void) {
  reg_READ3_t.reg_byte = i2c_read(IP5306_ADDRESS, REG_READ3);

  return reg_READ3_t.bits.SHORT_PRESS_DETECT;
}

uint8_t IP5306::long_press_detect(void) {
  reg_READ3_t.reg_byte = i2c_read(IP5306_ADDRESS, REG_READ3);

  return reg_READ3_t.bits.LONG_PRESS_DETECT;
}

uint8_t IP5306::double_press_detect(void) {
  reg_READ3_t.reg_byte = i2c_read(IP5306_ADDRESS, REG_READ3);

  return reg_READ3_t.bits.DOUBLE_PRESS_DETECT;
}
