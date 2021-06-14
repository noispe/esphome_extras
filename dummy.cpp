#include <esphome/core/application.h>
#include <esphome/components/logger/logger.h>
#include <esphome/components/wifi/wifi_component.h>

using namespace esphome;

void setup() {
  App.pre_setup("livingroom", "today", false);
  auto *log = new logger::Logger(115200, 512, logger::UART_SELECTION_UART0);
  log->pre_setup();
  App.register_component(log);

  auto *wifi = new wifi::WiFiComponent();
  App.register_component(wifi);
  wifi::WiFiAP ap;
  ap.set_ssid("Test SSID");
  ap.set_password("password1");
  wifi->add_sta(ap);

  // auto *gpio = new gpio::GPIOSwitch("GPIO Switch", new GPIOPin(8, OUTPUT));
  // App.register_component(gpio);
  // App.register_switch(gpio);

  App.setup();
}

void loop() { App.loop(); }
