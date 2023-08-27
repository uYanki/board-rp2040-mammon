from machine import Pin, Timer, ADC, PWM, SoftI2C, SPI
from time import sleep
from neopixel import NeoPixel
from sht30 import SHT30
from mpu6050 import MPU6050
import math
import sdcard as sdcard
import os


print("hello rp2040")
sleep(1)


############### LED ###############

led = Pin(0, Pin.OUT)
led.value(1)
sleep(0.5)
led.value(0)
sleep(0.5)
led.value(1)
sleep(0.5)

############### KEY ###############


def swInteruptHandler(pin):
    print(pin)


sw1 = Pin(7, Pin.IN, Pin.PULL_UP)
sw2 = Pin(12, Pin.IN, Pin.PULL_UP)
sw3 = Pin(13, Pin.IN, Pin.PULL_UP)

sw1.irq(trigger=Pin.IRQ_FALLING, handler=swInteruptHandler)
sw2.irq(trigger=Pin.IRQ_FALLING, handler=swInteruptHandler)
sw3.irq(trigger=Pin.IRQ_FALLING, handler=swInteruptHandler)

############### RGBLED ###############

rgb = Pin(1, Pin.OUT)
np = NeoPixel(rgb, n=2, bpp=3, timing=1)
np[0] = (2, 2, 2)
np[1] = (2, 2, 2)
np.write()

############### BUZZER ###############

buzzer = PWM(Pin(2))
buzzer.duty_u16(32768)  # duty / 65535 = 50

for i in range(1, 300):
    buzzer.freq(i*10)
    sleep(0.02)

buzzer.deinit()

############### ADC ###############

ad_rt = ADC(Pin(28))
ad_vbat = ADC(Pin(29))


def tmrReadADC(timer):
    global ad_rt, ad_vbat
    print(ad_rt.read_u16()/65535*3.3, ad_vbat.read_u16()/65535*3.3)


tmr = Timer()
tmr.init(freq=4, mode=Timer.PERIODIC, callback=tmrReadADC)
sleep(4)
tmr.deinit()


############### sdcard ###############


def sdtest():
    spi = SPI(0, miso=Pin(16), mosi=Pin(19), sck=Pin(18))
    spi.init()  # Ensure right baudrate
    sd = sdcard.SDCard(spi, Pin(14, Pin.OUT))  # Compatible with PCB
    vfs = os.VfsFat(sd)
    os.mount(vfs, "/fc")
    print("Filesystem check")
    print(os.listdir("/fc"))

    line = "abcdefghijklmnopqrstuvwxyz\n"
    lines = line * 200  # 5400 chars
    short = "1234567890\n"

    fn = "/fc/rats.txt"
    print()
    print("Multiple block read/write")
    with open(fn, "w") as f:
        n = f.write(lines)
        print(n, "bytes written")
        n = f.write(short)
        print(n, "bytes written")
        n = f.write(lines)
        print(n, "bytes written")

    with open(fn, "r") as f:
        result1 = f.read()
        print(len(result1), "bytes read")

    fn = "/fc/rats1.txt"
    print()
    print("Single block read/write")
    with open(fn, "w") as f:
        n = f.write(short)  # one block
        print(n, "bytes written")

    with open(fn, "r") as f:
        result2 = f.read()
        print(len(result2), "bytes read")

    os.umount("/fc")

    print()
    print("Verifying data read back")
    success = True
    if result1 == "".join((lines, short, lines)):
        print("Large file Pass")
    else:
        print("Large file Fail")
        success = False
    if result2 == short:
        print("Small file Pass")
    else:
        print("Small file Fail")
        success = False
    print()
    print("Tests", "passed" if success else "failed")


sdtest()


############### I2C ###############


i2c = SoftI2C(sda=Pin(20), scl=Pin(21), freq=400000)

devs = i2c.scan()

if len(devs) > 0:

    print(devs)

    sht = SHT30(i2c, i2c_address=0x44)

    for i in range(5):
        print(sht.measure())
        sleep(0.3)

    mpu = MPU6050(i2c)

    # mpu6050 中断的使用（拿着板子晃动几下即可触发）

    int_imu = Pin(15, Pin.IN, Pin.PULL_UP)
    int_imu.irq(trigger=Pin.IRQ_FALLING, handler=lambda pin: print("mpu_int"))

    addr_mpu6050 = 0x68
    i2c.writeto(addr_mpu6050, bytearray([0x1F, 0x25]))  # 设置加速度阈值为74mg
    i2c.writeto(addr_mpu6050, bytearray([0x20, 0x14]))  # 设置加速度检测时间20ms
    i2c.writeto(addr_mpu6050, bytearray([0x1A, 0x04]))  # 配置外部引脚采样和DLPF数字低通滤波器
    i2c.writeto(addr_mpu6050, bytearray([0x1C, 0x1C]))  # 加速度传感器量程和高通滤波器配置
    i2c.writeto(addr_mpu6050, bytearray([0x37, 0x1C]))  # INT引脚低电平平时
    i2c.writeto(addr_mpu6050, bytearray([0x38, 0x40]))  # 中断使能寄存器

    print("run ok")

    for i in range(500):
        vals = mpu.get_values()
        print({
            "picth": math.atan2(vals["AcY"], vals["AcZ"]) * 57.3,
            "roll": math.atan2(vals["AcX"], vals["AcZ"]) * 57.3
        })
        sleep(0.1)

else:
    print("can't find i2c dev")
