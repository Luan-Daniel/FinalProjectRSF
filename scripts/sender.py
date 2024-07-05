import serial

def main():
  test_count = 0
  while True:
    test_count+=1
    print(f"\n### TEST {test_count} ###\n")
    print("[i] Waiting for green board (sender) output.")
    read_from("COM10")

def read_from(port):
  s = serial.Serial(port=port, parity=serial.PARITY_EVEN, stopbits=serial.STOPBITS_ONE)
  while True:
    data = s.readline().decode()
    if "[END]" in data:
      break
    print(f"> {data}", end='')

if __name__ == "__main__":
  main()