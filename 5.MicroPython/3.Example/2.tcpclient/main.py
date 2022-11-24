from machine import Pin, UART
from time import sleep
import pdm

# 配置WiFi

ATCmds = [
    # "AT+RST\r\n",
    "AT+CWMODE=1\r\n",
    "AT+CWJAP=\"SSID\",\"PWD\"\r\n", # WiFi: ssid & pwd
    "AT+CIPMUX=0\r\n",
    "AT+CIPMODE=1\r\n",
    "AT+CIPSTART=\"TCP\",\"192.168.3.20\",6000\r\n", # tcp server: addr & port
    "AT+CIPSEND\r\n"
]

wifi_en = Pin(3, Pin.OUT)
wifi_en.value(1)
uart1 = UART(1, baudrate=115200, tx=Pin(4), rx=Pin(5))


error = False

for cmd in ATCmds:

    none_times = 0

    uart1.write(cmd)
    while True:
        sleep(0.5)  # 等待响应
        recv = str(uart1.read())
        if recv == "None":  # 什么都没读到
            none_times += 1
            if none_times > 20:  # 等待超时
                error = True
                break
            continue
        print(recv)
        if recv.find("OK") != -1:  # 指令执行成功
            break
        error = (recv.find("ERROR") != -
                 1) or (recv.find("FAIL") != -1)  # 指令执行失败

    if error:  # 结束发送指令
        break

# 配置麦克风


PlotCmds = [
    "#{PLOT.GRAPH.CFG|PDM:667699;}\r\n",
    "#{PLOT.VALUE.ADD|PDM:2889,262,1867,2314,-2971,679,1833,2699,3852,-3075,2552,1635,1046,-1706,-762,4047,3129,1326,2447,1989,2604,-889,1435,752,2757,-1753,2317,849,2579,1454,-418,-3766,-264,3969,-2764,3016,351,3505,-316,3345,2788,2121,-3491,-2110,-1459,-2004,507,-25,3628,487,3108,-2284,-500,3841,-243,36,1199,2268,-1099,-506,-3029,-304,-3655,-479,3418,1626,3713,1066,1965,-2321,-2535,-2517,-1019,-1643,1187,-112,2064,-2924,-626,794,630,-3104,-2761,-2004,2804,541,-3085,2947,1662,-543,2719,-2804,-595,-1245,-2255,-2471,-3977,3021,-1547,-1903,-1090,3186,-1989,-1871,-1562,-3196,-910,502,2129,3239,3409,-3375,2912,2108,298,909,2229,-2922,-823,-3539,-3560,-412,4081,2566,4008,745,1006,-1875;}\r\n"
]

if error == False:
    uart1.write(PlotCmds[0])  # 绘图指令

    pcm_rate = 8000  # Hz - default is 12kHz i.e. 3.072MHz bit-sample rate
    pdm.bit_sample_freq = pcm_rate * 256
    pdm.buf_len = 128

    pdm_clk = Pin(11)
    pdm_din = Pin(10)

    def buffer_handler(inactive_buf):
        global uart1, pdm
        pdmdata = pdm.get_buffer(inactive_buf)
        data = ','.join(list(map(str, pdmdata)))
        uart1.write(f"#{{PLOT.VALUE.ADD|PDM:{data};}}\r\n")

    pdm.init(pdm_clk, pdm_din, handler=buffer_handler)
    pdm.start()

    print("running")
    while True:
        sleep(1)
