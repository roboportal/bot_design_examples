from machine import ADC, UART, Pin, I2C
import math
import select
import sys
import json

bot_id = 0
invert_left_dir = True
invert_right_dir = False

device_address = 0x1e

register_a = 0        # Address of Configuration register A
register_b = 0x01     # Address of configuration register B
register_mode = 0x02  # Address of mode register

x_axis_h = 0x03       # Address of X-axis MSB data register
z_axis_h = 0x05       # Address of Z-axis MSB data register
y_axis_h = 0x07       # Address of Y-axis MSB data register

pi = 3.14159265359    # define pi value

conversion_factor = 3.3 / 65535
divider_ratio = 8

uart = UART(1, baudrate=9600, tx=Pin(8), rx=Pin(9))
i2c = I2C(1, scl=Pin(27), sda=Pin(26))
battery_voltage = ADC(28)

led = Pin(25, Pin.OUT)

lf_dir = Pin(17, Pin.OUT)
lf_pwm = Pin(18, Pin.OUT)
lf_fg = Pin(16, Pin.PULL_UP)

lb_dir = Pin(20, Pin.OUT)
lb_pwm = Pin(21, Pin.OUT)
lb_fg = Pin(19, Pin.PULL_UP)

rf_dir = Pin(11, Pin.OUT)
rf_pwm = Pin(12, Pin.OUT)
rf_fg = Pin(10, Pin.PULL_UP)

rb_dir = Pin(14, Pin.OUT)
rb_pwm = Pin(13, Pin.OUT)
rb_fg = Pin(15, Pin.PULL_UP)


def reg_write(addr, reg, data):
    msg = bytearray()
    msg.append(data)
    i2c.writeto_mem(addr, reg, msg)


def reg_read(addr, reg, nbytes=2):
    data = i2c.readfrom_mem(addr, reg, nbytes)
    value = int.from_bytes(data, "big")
    if(value > 32768):
        value = value - 65536
    return value


def init_compass():
    reg_write(device_address, register_a, 0x70)
    reg_write(device_address, register_b, 0xa0)
    reg_write(device_address, register_mode, 0)


def get_heading_angle():
    x = reg_read(device_address, x_axis_h)
    y = reg_read(device_address, y_axis_h)
    reg_read(device_address, z_axis_h)

    heading = math.atan2(y, x)

    if(heading > 2 * pi):
        heading = heading - 2 * pi

    if(heading < 0):
        heading = heading + 2 * pi

    return heading * 180 / pi


def get_coordinates(line):
    tokens = line.split(',')

    lat = tokens[1]
    long = tokens[3]

    if not lat or not long:
        return 0, 0

    lat_deg = lat[0:2]
    lat_min = lat[2:]

    long_deg = long[0:3]
    long_min = long[3:]

    lat = float(lat_deg) + float(lat_min) / 60
    long = float(long_deg) + float(long_min) / 60

    if tokens[2] == 'S':
        lat = -lat

    if tokens[4] == 'W':
        long = -long

    return lat, long


def invert_dir(dir):
    if dir == 1:
        return 0
    else:
        return 1


def setLeft(val):
    lf_pwm.value(val)
    lb_pwm.value(val)


def setRight(val):
    rf_pwm.value(val)
    rb_pwm.value(val)


def move(forward, backward, left, right):
    left_dir = 0
    right_dir = 0

    if backward > forward:
        left_dir = 1
        right_dir = 1

    if forward == 100 and left == 0 and right == 0:
        setLeft(0)
        setRight(0)

    if forward == 100 and left == 100:
        setLeft(0)
        setRight(1)

    if forward == 100 and right == 100:
        setLeft(1)
        setRight(0)

    if forward == 0 and backward == 0 and left == 100:
        setLeft(0)
        setRight(0)
        right_dir = 1

    if forward == 0 and backward == 0 and right == 100:
        setLeft(0)
        setRight(0)
        left_dir = 1

    if backward == 100 and left == 0 and right == 0:
        setLeft(0)
        setRight(0)

    if backward == 100 and left == 100:
        setLeft(0)
        setRight(1)

    if backward == 100 and right == 100:
        setLeft(1)
        setRight(0)

    if forward == 0 and left == 0 and right == 0 and backward == 0:
        setLeft(1)
        setRight(1)

    left_dir = left_dir if invert_left_dir else invert_dir(left_dir)
    right_dir = right_dir if invert_right_dir else invert_dir(right_dir)

    lf_dir.value(left_dir)
    lb_dir.value(left_dir)
    rf_dir.value(right_dir)
    rb_dir.value(right_dir)


init_compass()

led.value(1)
move(0, 0, 0, 0)

gps_command = bytes()
is_gps_command_available = False

move_command = ""
is_move_command_available = False

while True:
    if is_gps_command_available:
        if '$GNGLL' in gps_command:
            heading_angle = get_heading_angle()

            lat, lng = get_coordinates(gps_command)

            lat = lat or 'null'
            lng = lng or 'null'

            reading = battery_voltage.read_u16() * conversion_factor
            voltage = round(reading * divider_ratio, 2)

            telemetry = '{"id":' + str(bot_id) + ',"lat":' + str(lat) + ',"lng":' + str(
                lng) + ',"headingAngle":' + str(heading_angle) + ',"battery":"' + str(voltage) + 'V"}'
            print(telemetry)

        is_gps_command_available = False
        gps_command = bytes()

    if is_move_command_available:
        try:
            data = json.loads(move_command)['controls']
            for key in ('f', 'b', 'l', 'r'):
                if not key in data:
                    data[key] = 0
            move(data['f'], data['b'], data['l'], data['r'])
        except:
            move(0, 0, 0, 0)
        finally:
            is_move_command_available = False
            move_command = ""

    while uart.any() > 0:
        try:
            ch = uart.read(1)
            if ch.decode() == '\n':
                gps_command = gps_command.decode()
                is_gps_command_available = True
            else:
                gps_command = gps_command + ch
        except:
            continue

    while sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
        line = sys.stdin.readline().strip()
        if line:
            move_command = line
            is_move_command_available = True

