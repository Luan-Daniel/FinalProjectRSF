# Pseudocódigo simplificando o funcionamento do nó receptor.

loop:
  blink_led(2)
  set_led(1)
  print("[i] Ready to start a new test, press button to continue")
  wait_button_press()

  blink_led(3)
  print("[i] Clearing FIFO")
  clear_nec_receptor_fifo() # limpa qualquer sinal não processado antes do teste

  keep_looking_for_signals = True
  bytes_received = 0
  bytes_decoded = 0
  unexpected_bytes = 0
  previous_data = 0x00


  while keep_looking_for_signals:
    while nec_receptor_fifo_not_empty():
      if bytes_received==0: # se é o primeiro sinal detectado:
        initial_time = get_us_time()

      frame = get_nec_signal()
      bytes_received += 2

      try:
        address, data = decode_frame(frame)

        # Checa se recebeu o sinal de "fim de transmissão"
        if address==0xFF and data==0xFF:
          final_time = get_us_time()
          keep_looking_for_signals = False
          break

        bytes_decoded += 2

        # Checa por bytes inesperados
        if address != 0x00:
          unexpected_bytes += 1
        if previous_data!=0x00 and data!=previous_data:
          unexpected_bytes += 1

        previous_data = data

      except FailToDecode:
        continue

  # manda dados coletados para o terminal
  print("[i] Test finished, sending data")
  print(f"[Br]:{bytes_received}")
  print(f"[Bd]:{bytes_decoded}")
  print(f"[Bu]:{unexpected_bytes}")
  print(f"[DeltaT]:{final_time-initial_time}")
  print("[END]")