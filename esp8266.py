import machine
import time
from machine import I2C, Pin

i2c = I2C(scl=Pin(5), sda=Pin(4), freq=100000,timeout=5000000)


def post(data):
    i2c.start()
    i2c.writeto(8, data)
    i2c.stop()
    
    i2c.start()
    response = i2c.readfrom(8, 20)  # Request and read response data of size 20 from Arduino
    i2c.stop()

    response_str = "".join(chr(c) for c in response)
    return response_str.replace("ÿ","") # just to remove extra chars
