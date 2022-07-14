import time
import pyautogui
from selenium import webdriver
from selenium.webdriver.common.by import By
import keyboard
import ctypes


"""
SELENIUM SETUP
"""
MAX_DISTANCE = 3840 + 1920 ** 4

driver = webdriver.Chrome(service_log_path='NUL')
driver.get("http://localhost:8000")

elements = driver.find_elements(By.CLASS_NAME, "form-check-label")
input_elements = driver.find_elements(By.CLASS_NAME, "form-check-input")

elements.append(driver.find_element(By.CLASS_NAME, "btn"))
input_elements.append(driver.find_element(By.CLASS_NAME, "btn"))

#might want to refresh elements
#C:\Users\salva\source\repos\tobii_reader\x64\Release\


"""
TOBII AND BROWSER FUNCTIONS
"""
tobii_reader = ctypes.CDLL(".\\tobii_reader.dll")

def dist(x1, y1, x2, y2):
    return (y2 - y1) ** 2 + (x2 - x1) ** 2

def GetWindowRectFromName(name:str) -> tuple:
    hwnd = ctypes.windll.user32.FindWindowW(0, name)
    rect = ctypes.wintypes.RECT()

    ctypes.windll.user32.GetWindowRect(hwnd, ctypes.pointer(rect))

    return (rect.left, rect.top)

def closestElement(cx, cy):
    rect = GetWindowRectFromName("React App - Google Chrome")

    lowest_dist = MAX_DISTANCE
    lowest_element = elements[0]

    for e in elements:
        distance = dist(rect[0] + (e.location['x'] + 18) * 2.3, rect[1] + (e.location['y'] + 210) * 2, cx, cy)

        if (distance < lowest_dist):

            lowest_dist = distance
            lowest_element = e

    
    if cy >= elements[len(elements) - 1].location.get("y") + 700:
        return elements[len(elements) - 1]
    
    return lowest_element    

def calibrate() -> tuple:
    x_diff = 0
    y_diff = 0

    while not keyboard.is_pressed("enter"):
        x_diff = tobii_reader.GetX() - pyautogui.position().x
        y_diff = tobii_reader.GetY() - pyautogui.position().y + 25
        
    return (x_diff, y_diff)


"""
MAIN EXECUTION
"""
if __name__ == "__main__":

    try:
        x1 = tobii_reader.GetX()
        time.sleep(0.2)
        x2 = tobii_reader.GetX()
        
        if x1 == x2:
            print("TOBII READER DISCONNECTED")
            print("Exiting...")
            driver.close()
            quit()
    except:
        quit()

    highlighted_element = elements[0]
    x_offset, y_offset = calibrate()

    prev_x = 0
    while True:

        closest_element = closestElement(tobii_reader.GetX() - x_offset, tobii_reader.GetY() - y_offset)

        if closest_element != highlighted_element:
            driver.execute_script("arguments[0].style.border=''", highlighted_element)
            driver.execute_script("arguments[0].style.border='2px solid red'", closest_element)

            highlighted_element = closest_element

        if (keyboard.is_pressed("enter") or tobii_reader.GetX() == prev_x):
            input_elements[elements.index(closest_element)].click()

        prev_x = tobii_reader.GetX()
        time.sleep(0.3)
