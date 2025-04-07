import tkinter as tkr
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

red_led = 4
green_led = 6
blue_led = 5

GPIO.setup(red_led, GPIO.OUT)
GPIO.setup(green_led, GPIO.OUT)
GPIO.setup(blue_led, GPIO.OUT)

red_pwm = GPIO.PWM(red_led, 100)
blue_pwm = GPIO.PWM(blue_led, 100)
green_pwm = GPIO.PWM(green_led, 100)

red_pwm.start(0)
blue_pwm.start(0)
green_pwm.start(0)

def red_brightness_level(val):
    red_pwm.ChangeDutyCycle(float(val))

def blue_brightness_level(val):
    blue_pwm.ChangeDutyCycle(float(val))

def green_brightness_level(val):
    green_pwm.ChangeDutyCycle(float(val))


def end_pro():
    red_pwm.stop()
    blue_pwm.stop()
    green_pwm.stop()
    GPIO.cleanup()
    window.destroy()

window = tkr.Tk()
window.title("LED CONTROLLER")
window.geometry("275x350")



tkr.Label(window, text= "BRIGHTNESS LEVEL CONTROLLER >> ").pack(pady=10)

tkr.Label(window, text= "RED level").pack()
tkr.Scale(window, from_=0, to=100, orient=tkr.HORIZONTAL, command=red_brightness_level).pack()

tkr.Label(window, text= "BLUE level").pack()
tkr.Scale(window, from_=0, to=100, orient=tkr.HORIZONTAL, command=blue_brightness_level).pack()

tkr.Label(window, text= "GREEN level").pack()
tkr.Scale(window, from_=0, to=100, orient=tkr.HORIZONTAL, command=green_brightness_level).pack()


tkr.Button(window, text = "END", command =end_pro, bg = "RED", fg="BLACK").pack(pady=10)

window.mainloop()
