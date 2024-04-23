from gpiozero import Button
from signal import pause
import json
import requests
import platform

# check if the OS is not Darwin (macOS)
if platform.system() != "Darwin":
    from gpiozero import Button

    BUTTON_PIN = 2
    button = Button(BUTTON_PIN)
else:
    button = None

def button_pressed():
    try:
        requests.get("http://0.0.0.0:8000/start")
        print("[INFO] button/starter triggered. Starting new race")
    except Exception as e:
        print(f"[WARNING] is the timing app running?\n{e}")

if button:
    button.when_pressed = button_pressed

pause()
