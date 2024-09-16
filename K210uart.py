from machine import UART
from fpioa_manager import fm
import time
def sending_data(x,y,z):
    global uart;
    FH = bytearray([0x2C,0x12,x,y,z,0x5B])
    uart_A.write(FH);
fm.register(8, fm.fpioa.UART1_TX, force=True)
fm.register(6, fm.fpioa.UART1_RX, force=True)

uart_A = UART(UART.UART1, 115200, 8, 0, 0, timeout=1000, read_buf_len=4096)

Cx = 0
Cy = 0
Cz = 0

while True:

    Cx+=1;
    Cy+=1;
    Cz+=1;
    sending_data(Cx,Cy,Cz)
    print("Cx:",Cx,"Cy",Cy,"Cz:",Cz)
    time.sleep_ms(1000)
