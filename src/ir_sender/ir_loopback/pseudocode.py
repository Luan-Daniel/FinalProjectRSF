# Pseudocódigo simplificando o funcionamento do emissor.
address = 0x00
delay_ms = 100

loop:
  blink_led(2) # pisca o led duas vezes
  set_led(1)
  print("[i] Ready to start a new test, press button to continue")
  wait_button_press()

  blink_led(3)
  print("[i] Test started, please wait")

  initial_time = get_us_time() # guarda o tempo inicial medido em microsegundos

  for data in range(0x00, 0xFF):
    # codifica e então transmite os dados usado funções da biblioteca
    frame = nec_encode_frame(address, data)
    transmit_frame(frame)
    sleep_ms(delay_ms) # dá tempo para o nó receptor decodificar dados

  final_time = get_us_time()

  # envia sinal de "final de transmissão"
  for _ in range(2): # transmite um frame com todos os bits setados
    frame = nec_encode_frame(0xFF, 0xFF)
    transmit_frame(frame)

  # manda dados coletados para o terminal
  print("[i] Test finished, sending data")
  print(f"[Delay]:{delay_ms}\n[DeltaT]:{final_time-initial_time}\n[END]")