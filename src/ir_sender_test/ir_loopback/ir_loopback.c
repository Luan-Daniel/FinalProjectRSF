// GREEN BOARD

#include <time.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "nec_transmit.h"

#define send_delay_ms 100
void blink(uint pin, uint mshbt, int nbliks, bool fstate);
void wait_pin(uint pin);


int main() {
  // Setup and Init IO //
  stdio_init_all();

  const uint red_gpio = 3;
  gpio_init(red_gpio);
  gpio_set_dir(red_gpio, GPIO_OUT);

  const uint button_gpio = 2;
  gpio_init(button_gpio);
  gpio_set_dir(button_gpio, GPIO_OUT);


  // Setup and Init PIO program for the IR transmissor
  PIO pio = pio0;
  const uint tx_gpio = 5;
  int tx_sm = nec_tx_init(pio, tx_gpio);
  if (tx_sm == -1) {
    printf("could not configure PIO\n");
    blink(red_gpio, 100, -1, 0);
    return -1;
  }


  absolute_time_t start_time=nil_time, end_time=nil_time;
  uint8_t tx_address = 0x00, tx_data = 0x00;
  while (true) {
    start_time=nil_time;
    end_time=nil_time;
    tx_address = 0x00;
    tx_data = 0x00;

    blink(red_gpio, 250, 2, 1);
    printf("[i] Ready to start a new test, press button to continue\n");
    wait_pin(button_gpio);
    blink(red_gpio, 150, 3, 0);
    printf("[i] Test started, please wait\n");

    start_time = get_absolute_time();

    while (true) {
      printf("[i] Button to continue\n");
      wait_pin(button_gpio);
      printf("[ENCODE] 0x%02X, 0x%02X\n",tx_address, tx_data);
      uint32_t tx_frame = nec_encode_frame(tx_address, tx_data);
      printf("[SEND] 0x%04X\n",tx_frame);
      pio_sm_put(pio, tx_sm, tx_frame);

      if (tx_data==0xff) {
        end_time = get_absolute_time();
        for(uint8_t i=0x00;i<0x03;i++) {
          uint32_t tx_frame = nec_encode_frame(0xff, 0xff);
          pio_sm_put(pio, tx_sm, tx_frame);
          sleep_ms(send_delay_ms);
        }
        break;
      }

      tx_data++;
    }

    printf("[i] Test finished, sending data\n");

    printf(
      "[Delay]:%d\n"
      "[DeltaT]:%020d\n"
      "[END]\n",
      send_delay_ms, absolute_time_diff_us(start_time, end_time));
  }

  return 0;
}

void blink(uint pin, uint mshbt, int nbliks, bool fstate){
  while (true) {
    if (nbliks==0) return;
    gpio_put(pin, !fstate);
    sleep_ms(mshbt);
    gpio_put(pin, fstate);
    sleep_ms(mshbt);
    if (nbliks>0) nbliks--;
  }
}

void wait_pin(uint pin){
  while (!gpio_get(pin));
  while (gpio_get(pin));
}