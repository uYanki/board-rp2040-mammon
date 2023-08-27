'''
libraries
'''


from board import *

import usb_hid

from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS as KeyboardLayout

import time

import digitalio
import pwmio

import supervisor

'''
user code
'''


defaultDelay = 0


duckyCommands = {
    'WINDOWS': Keycode.WINDOWS, 'GUI': Keycode.GUI,
    'APP': Keycode.APPLICATION, 'MENU': Keycode.APPLICATION, 'SHIFT': Keycode.SHIFT,
    'ALT': Keycode.ALT, 'CONTROL': Keycode.CONTROL, 'CTRL': Keycode.CONTROL,
    'DOWNARROW': Keycode.DOWN_ARROW, 'DOWN': Keycode.DOWN_ARROW, 'LEFTARROW': Keycode.LEFT_ARROW,
    'LEFT': Keycode.LEFT_ARROW, 'RIGHTARROW': Keycode.RIGHT_ARROW, 'RIGHT': Keycode.RIGHT_ARROW,
    'UPARROW': Keycode.UP_ARROW, 'UP': Keycode.UP_ARROW, 'BREAK': Keycode.PAUSE,
    'PAUSE': Keycode.PAUSE, 'CAPSLOCK': Keycode.CAPS_LOCK, 'DELETE': Keycode.DELETE,
    'END': Keycode.END, 'ESC': Keycode.ESCAPE, 'ESCAPE': Keycode.ESCAPE, 'HOME': Keycode.HOME,
    'INSERT': Keycode.INSERT, 'NUMLOCK': Keycode.KEYPAD_NUMLOCK, 'PAGEUP': Keycode.PAGE_UP,
    'PAGEDOWN': Keycode.PAGE_DOWN, 'PRINTSCREEN': Keycode.PRINT_SCREEN, 'ENTER': Keycode.ENTER,
    'SCROLLLOCK': Keycode.SCROLL_LOCK, 'SPACE': Keycode.SPACE, 'TAB': Keycode.TAB,
    'BACKSPACE': Keycode.BACKSPACE, 'DELETE': Keycode.DELETE,
    'A': Keycode.A, 'B': Keycode.B, 'C': Keycode.C, 'D': Keycode.D, 'E': Keycode.E,
    'F': Keycode.F, 'G': Keycode.G, 'H': Keycode.H, 'I': Keycode.I, 'J': Keycode.J,
    'K': Keycode.K, 'L': Keycode.L, 'M': Keycode.M, 'N': Keycode.N, 'O': Keycode.O,
    'P': Keycode.P, 'Q': Keycode.Q, 'R': Keycode.R, 'S': Keycode.S, 'T': Keycode.T,
    'U': Keycode.U, 'V': Keycode.V, 'W': Keycode.W, 'X': Keycode.X, 'Y': Keycode.Y,
    'Z': Keycode.Z, 'F1': Keycode.F1, 'F2': Keycode.F2, 'F3': Keycode.F3,
    'F4': Keycode.F4, 'F5': Keycode.F5, 'F6': Keycode.F6, 'F7': Keycode.F7,
    'F8': Keycode.F8, 'F9': Keycode.F9, 'F10': Keycode.F10, 'F11': Keycode.F11,
    'F12': Keycode.F12,
}


def convertLine(line):
    newline = []
    # print(line)
    # loop on each key - the filter removes empty values
    for key in filter(None, line.split(" ")):
        key = key.upper()
        # find the keycode for the command in the list
        command_keycode = duckyCommands.get(key, None)
        if command_keycode is not None:
            # if it exists in the list, use it
            newline.append(command_keycode)
        elif hasattr(Keycode, key):
            # if it's in the Keycode module, use it (allows any valid keycode)
            newline.append(getattr(Keycode, key))
        else:
            # if it's not a known key name, show the error for diagnosis
            print(f"Unknown key: <{key}>")
    # print(newline)
    return newline


def parseLine(line):  # 此处自定义指令
    global defaultDelay
    if (line[0:3] == "REM"):
        # ignore ducky script comments
        pass
    elif (line[0:5] == "DELAY"):
        time.sleep(float(line[6:])/1000)
    elif (line[0:6] == "STRING"):
        layout.write(line[7:])
    elif (line[0:5] == "PRINT"):
        print("[SCRIPT]: " + line[6:])
    elif (line[0:6] == "IMPORT"):
        runScript(line[7:])
    elif (line[0:3] == "LOG"):
        print(line[4:])
    else:
        for k in convertLine(line):
            kbd.press(k)
        kbd.release_all()


def runScript(script):
    global defaultDelay

    try:
        f = open(script, "r", encoding='utf-8')
        previousLine = ""
        for line in f.readlines():
            line = line.rstrip()
            if (line[0:6] == "REPEAT"):
                for i in range(int(line[7:])):
                    # repeat the last command
                    parseLine(previousLine)
                    time.sleep(float(defaultDelay)/1000)
            else:
                parseLine(line)
                previousLine = line
            time.sleep(float(defaultDelay)/1000)
        print("Done", script)
    except OSError as e:
        print("Unable to open file ", script)


'''
badusb task
'''

kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayout(kbd)

# turn off automatically reloading when files are written to the pico
supervisor.disable_autoreload()
# sleep at the start to allow the device to be recognized by the host computer
time.sleep(.5)

pin = digitalio.DigitalInOut(GP1)  # mode pin
pin.switch_to_input(pull=digitalio.Pull.DOWN)
# if not in setup mode, inject the payload
if (pin.value):
    # select payload
    script_id = ''
    for gp in [GP5, GP4, GP3, GP2]:  # paload pin
        pin = digitalio.DigitalInOut(gp)
        pin.switch_to_input(pull=digitalio.Pull.DOWN)
        script_id += '1' if pin.value else '0'
    # run script
    from os import listdir
    payloads_path = './payloads'
    for filename in listdir(payloads_path):
        if filename.startswith(script_id) and filename.endswith('.txt'):
            runScript(payloads_path + '/' + filename)
            break


'''
led task
'''


def led_task():

    led = pwmio.PWMOut(LED, frequency=5000, duty_cycle=0)

    while True:
        # Up
        for i in range(50):
            led.duty_cycle = int(i * 2 * 65535 / 100)
            time.sleep(0.01)
        time.sleep(0.5)
        # Down
        for i in range(50):
            led.duty_cycle = 65535 - int(i * 2 * 65535 / 100)
            time.sleep(0.01)
        time.sleep(0.5)


led_task()
