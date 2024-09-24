import sensor, image, time, lcd
from machine import UART
from fpioa_manager import fm

from modules import ybserial
#from robot_Lib import Robot
#from simplePID import PID


def sending_data(x,y,z):
    global uart;
    FH = bytearray([0x2C,0x12,x,y,z,0x5B])
    uart_A.write(FH);
fm.register(8, fm.fpioa.UART1_TX, force=True)
fm.register(6, fm.fpioa.UART1_RX, force=True)

uart_A = UART(UART.UART1, 115200, 8, 0, 0, timeout=1000, read_buf_len=4096)


cx_str = '0'
cy_str = '0'

PIDx = (50, 0, 3)
PIDy = (50, 0, 2)
SCALE = 1000.0

#PID_x = PID(
#    160,
#    PIDx[0] / 1.0 / (SCALE),
#    PIDx[1] / 1.0 / (SCALE),
#    PIDx[2] / 1.0 / (SCALE))

#PID_y = PID(
#    120,
#    PIDy[0] / 1.0 / (SCALE),
#    PIDy[1] / 1.0 / (SCALE),
#    PIDy[2] / 1.0 / (SCALE))


#ser = ybserial()
#bot = Robot(ser)
#bot.set_beep(50)
#bot.set_car_motion(0, 0, 0)
#bot.set_pwm_servo(1, 90)
#bot.set_pwm_servo(2, 90)


lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 800)
sensor.set_auto_exposure(1)                                 # 设置自动曝光
sensor.set_auto_gain(False)                       # 设置画面增益 17 dB 影响实时画面亮度sensor.set_auto_whitebal(False)
sensor.set_auto_whitebal(False)
clock = time.clock()

#print("Hold the object you want to track in front of the camera in the box.")
#print("MAKE SURE THE COLOR OF THE OBJECT YOU WANT TO TRACK IS FULLY ENCLOSED BY THE BOX!")

# Capture the color thresholds for whatever was in the center of the image.
# 50x50 center of QVGA.
#BOX = 30
#r = [(320//2)-(BOX//2), (240//2)-(BOX//2), BOX, BOX]
#for i in range(50):
#    img = sensor.snapshot()
#    img.draw_rectangle(r)
#    lcd.display(img)

#print("Learning thresholds...")
#threshold = [0, 0, 0, 0, 0, 0] # Middle L, A, B values.

#for i in range(50):
 #   img = sensor.snapshot()
  #  hist = img.get_histogram(roi=r)
   # lo = hist.get_percentile(0.01) # Get the CDF of the histogram at the 1% range (ADJUST AS NECESSARY)!
    #hi = hist.get_percentile(0.99) # Get the CDF of the histogram at the 99% range (ADJUST AS NECESSARY)!
    # Average in percentile values.
   # threshold[0] = (threshold[0] + lo.l_value()) // 2
   # threshold[1] = (threshold[1] + hi.l_value()) // 2
   # threshold[2] = (threshold[2] + lo.a_value()) // 2
   # threshold[3] = (threshold[3] + hi.a_value()) // 2
   # threshold[4] = (threshold[4] + lo.b_value()) // 2
   # threshold[5] = (threshold[5] + hi.b_value()) // 2
   # for blob in img.find_blobs([threshold], pixels_threshold=100, area_threshold=100, merge=True, margin=10):
   #     img.draw_rectangle(blob.rect())
   #     img.draw_cross(blob.cx(), blob.cy())
   #     img.draw_rectangle(r, color=(0,255,0))
   # lcd.display(img)

threshold = [30, 45, 35, 60, -17, 36]
state = 0
flag = 1
#pixels_threshold=100, area_threshold=150, merge=True, margin=10
while(True):
    clock.tick()
    img = sensor.snapshot()
    #img.binary([threshold])     #二极化处理
    #img.dilate(1)                   #腐蚀膨胀过滤边沿噪点
    fps = clock.fps()
    data_in = 0
    index = 0
    blobs = img.find_blobs([threshold], pixels_threshold=100, area_threshold=150, merge=False)
    if flag == 1:
        if blobs:
            area_max = 0
            area = None
            for blob in blobs:
                index = index + 1
                if index == 1:
                    area_max = blob.w()*blob.h()
                    area = blob
                else:
                    temp_area = blob.w()*blob.h()
                    if temp_area > area_max:
                        area_max = temp_area
                        area = blob
                state = 1
        else:
            sending_data(int(320/2),int(240/2),0)
            print("0")
        if state == 1:
            #print("area:", index, area.w(), area.h())
            img.draw_rectangle(area[0:4])
            img.draw_cross(area.cx(), area.cy())
           # value_x = PID_x.incremental(area.cx())
            #value_y = -PID_y.incremental(area.cy())
           # if -1 < value_x < 1: value_x = 0
           # if -1 < value_y < 1: value_y = 0
           # last_x = last_x + value_x
           # last_y = last_y + value_y
            #bot.set_pwm_servo(1, last_x)
            #bot.set_pwm_servo(2, last_y)
     #       control_servo(last_x, last_y)
            #print("area:", area.cx(), area.cy())
            #print("value:", last_x, last_y)
            sending_data(int(area.cx()),int(area.cy()),0)
        else:
            sending_data(int(320/2),int(240/2),0)
            print("0")
    else:
        img.binary([threshold])     #二极化处理
        img.dilate(1)                   #腐蚀膨胀过滤边沿噪点

    state = 0
    img.draw_string(0, 0, "%2.1ffps" %(fps), color=(0, 60, 128), scale=2.0)
    lcd.display(img)
    #print("FPS:s", fps)
    #bot.set_car_motion(0, 0, value)
