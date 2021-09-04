import machine
import select
import sys
import json

invert_left_dir = True
invert_right_dir = False

led = machine.Pin(25, machine.Pin.OUT)

lf_dir = machine.Pin(17, machine.Pin.OUT)
lf_pwm = machine.Pin(18, machine.Pin.OUT)
lf_fg = machine.Pin(16, machine.Pin.PULL_UP)

lb_dir = machine.Pin(20, machine.Pin.OUT)
lb_pwm = machine.Pin(21, machine.Pin.OUT)
lb_fg = machine.Pin(19, machine.Pin.PULL_UP)

rf_dir = machine.Pin(11, machine.Pin.OUT)
rf_pwm = machine.Pin(12, machine.Pin.OUT)
rf_fg = machine.Pin(10, machine.Pin.PULL_UP)

rb_dir = machine.Pin(14, machine.Pin.OUT)
rb_pwm = machine.Pin(13, machine.Pin.OUT)
rb_fg = machine.Pin(15, machine.Pin.PULL_UP)


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


led.value(1)
move(0, 0, 0, 0)

is_line_available = False
line = ""

while True:
    if is_line_available:
        try:
            data = json.loads(line)['controls']
            for key in ('f', 'b', 'l', 'r'):
                if not key in data:
                    data[key] = 0
            move(data['f'], data['b'], data['l'], data['r'])
        except:
            move(0, 0, 0, 0)
        finally:
            is_line_available = False
            line = ""

    while sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
        ch = sys.stdin.read(1)
        if ch == '\n':
            is_line_available = True
        else:
            line = line + ch
