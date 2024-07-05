// BLACK BOARD

#include <time.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "nec_receive.h"

#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#endif

void blink(uint pin, uint mshbt, int nbliks, bool fstate);
void wait_pin(uint pin);

int main() {
  // Setup and Init IO //
  stdio_init_all();

  const uint led_pin = PICO_DEFAULT_LED_PIN;
  gpio_init(led_pin);
  gpio_set_dir(led_pin, GPIO_OUT);

  const uint button_gpio = 15;
  gpio_init(button_gpio);
  gpio_set_dir(button_gpio, GPIO_OUT);


  // Setup and Init PIO program for the IR transmissor
  PIO pio = pio0;
  const uint rx_gpio = 14;
  int rx_sm = nec_rx_init(pio, rx_gpio);
  if (rx_sm == -1) {
    printf("could not configure PIO\n");
    blink(led_pin, 100, -1, 0);
    return -1;
  }

  absolute_time_t start_time=nil_time, end_time=nil_time;
  uint16_t bytes_received=0x00, bytes_decoded=0x00, unexpected_bytes=0x00;
  uint8_t rx_address, rx_data, tx_data=0x00;
  bool should_end=false;
  while (true) {
    start_time=nil_time;
    end_time=nil_time;
    bytes_received=0x00;
    bytes_decoded=0x00;
    unexpected_bytes=0x00;
    tx_data=0x00;
    should_end=false;

    blink(led_pin, 250, 2, 1);
    printf("[i] Ready to start a new test, press button to continue\n");
    wait_pin(button_gpio);
    blink(led_pin, 150, 3, 0);

    printf("[i] Clearing FIFO\n");
    while (!pio_sm_is_rx_fifo_empty(pio, rx_sm))
      pio_sm_get(pio, rx_sm);

    printf("[i] FIFO clear, now listening for new signals\n");

    while (!should_end) {
      while (!pio_sm_is_rx_fifo_empty(pio, rx_sm)) {
        if (!bytes_received) start_time = get_absolute_time();
        uint32_t rx_frame = pio_sm_get(pio, rx_sm);
        bytes_received+=2;

        if (nec_decode_frame(rx_frame, &rx_address, &rx_data)) {
          if (rx_address==0xff && rx_data==0xff) {
            end_time = get_absolute_time();
            should_end=true;
            break;
          }

          bytes_decoded+=2;
          if (rx_address!=0x00) unexpected_bytes++;
          if (tx_data!=0x00)
            if (rx_data!=(tx_data+1)) {
              unexpected_bytes++;
            }
          tx_data=rx_data;
        }
      }
    }

    printf("[i] Test finished, sending data\n");

    printf(
      "[Br]:%d\n"
      "[Bd]:%d\n"
      "[Bu]:%d\n"
      "[DeltaT]:%020d\n"
      "[END]\n",
      bytes_received, bytes_decoded,
      unexpected_bytes, absolute_time_diff_us(start_time, end_time));
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