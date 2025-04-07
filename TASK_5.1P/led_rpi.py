import tkinter as tkr
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

red_led =4
green_led = 5
blue_led = 6

GPIO.setup(red_led, GPIO.OUT)
GPIO.setup(green_led, GPIO.OUT)
GPIO.setup(blue_led, GPIO.OUT)

def switchon_led():
    selected = picked_led.get()
    GPIO.output(red_led, selected =="RED")
    GPIO.output(green_led, selected == "BLUE")
    GPIO.output(blue_led,selected == "GREEN")

def end_pro():
    GPIO.cleanup()
    window.destroy()

window = tkr.Tk()
window.title("LED CONTROLLER")
window.geometry("250x150")

picked_led = tkr.StringVar()
picked_led.set("None")

tkr.Label(window, text= " Which led you want to turn on >> ").pack(pady=5)
tkr.Radiobutton(window, text= "RED" , variable=picked_led, value="RED", command=switchon_led).pack()
tkr.Radiobutton(window, text= "BLUE" , variable=picked_led, value="BLUE", command=switchon_led).pack()
tkr.Radiobutton(window, text= "GREEN" , variable=picked_led, value="GREEN", command=switchon_led).pack()

tkr.Button(window, text = "END", command =end_pro, bg = "RED", fg="BLACK").pack(pady=10)

window.mainloop()
