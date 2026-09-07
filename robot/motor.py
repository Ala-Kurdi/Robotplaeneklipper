import subprocess

def stop():
    """Stopper begge motorer."""
    subprocess.run(["pinctrl", "set", "17", "op", "dl"])
    subprocess.run(["pinctrl", "set", "27", "op", "dl"])
    subprocess.run(["pinctrl", "set", "22", "op", "dl"])
    subprocess.run(["pinctrl", "set", "23", "op", "dl"])

def forward():
    """Kører fremad."""
    subprocess.run(["pinctrl", "set", "17", "op", "dh"])
    subprocess.run(["pinctrl", "set", "27", "op", "dl"])
    subprocess.run(["pinctrl", "set", "22", "op", "dh"])
    subprocess.run(["pinctrl", "set", "23", "op", "dl"])

def backward():
    """Bakker."""
    subprocess.run(["pinctrl", "set", "17", "op", "dl"])
    subprocess.run(["pinctrl", "set", "27", "op", "dh"])
    subprocess.run(["pinctrl", "set", "22", "op", "dl"])
    subprocess.run(["pinctrl", "set", "23", "op", "dh"])

def left():
    """Drej til venstre (venstre motor bakker, højre motor kører frem)."""
    subprocess.run(["pinctrl", "set", "17", "op", "dl"])
    subprocess.run(["pinctrl", "set", "27", "op", "dh"])
    subprocess.run(["pinctrl", "set", "22", "op", "dh"])
    subprocess.run(["pinctrl", "set", "23", "op", "dl"])

def right():
    """Drej til højre (venstre motor kører frem, højre motor bakker)."""
    subprocess.run(["pinctrl", "set", "17", "op", "dh"])
    subprocess.run(["pinctrl", "set", "27", "op", "dl"])
    subprocess.run(["pinctrl", "set", "22", "op", "dl"])
    subprocess.run(["pinctrl", "set", "23", "op", "dh"])