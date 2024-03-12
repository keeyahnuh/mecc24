from tkinter import *
import serial.tools.list_ports
import functools
import re

ports = serial.tools.list_ports.comports()
serialObj = serial.Serial()

root = Tk()
root.config(bg='grey')

def initComPort(index):
    currentPort = str(ports[index])
    comPortVar = str(currentPort.split(' ')[0])
    print(comPortVar)
    serialObj.port = comPortVar
    serialObj.baudrate = 9600
    serialObj.open()

connect = Label(root, text = 'Choose your comm port:', font = ('Calibri', '13'), fg = 'white', bg = 'black', height=1, width=45)
connect.grid(row=0, column=0)

index = 1
for onePort in ports:
    comButton = Button(root, text=onePort, font=('Calibri', '13'), height=1, width=45, command = functools.partial(initComPort, index = ports.index(onePort)))
    comButton.grid(row=index, column=0)
    index = index + 1

dataCanvas = Canvas(root, width=600, height=400, bg='white')
dataCanvas.grid(row=0, column=1, rowspan=100)

# vsb = Scrollbar(root, orient='vertical', command=dataCanvas.yview)
# vsb.grid(row=0, column=2, rowspan=100, sticky='ns')

# dataCanvas.config(yscrollcommand = vsb.set)

dataFrame = Frame(dataCanvas, bg="white")
dataCanvas.create_window((10,0),window=dataFrame,anchor='nw')

# basic gui setup
stateTitle = Label(dataFrame, text = 'BMS State/Status:', font = ('Calibri', 13), fg = 'black', bg = 'white')
stateTitle.grid(row = 0, column = 1)

bat1 = Label(dataFrame, text = 'Battery 1:', font = ('Calibri', 13), fg = 'black', bg = 'white')
bat1.grid(row = 2, column = 0)

bat2 = Label(dataFrame, text = 'Battery 2:', font = ('Calibri', 13), fg = 'black', bg = 'white')
bat2.grid(row = 2, column = 2)

watt = Label(dataFrame, text = 'Overall Wattage:', font = ('Calibri', 13), fg = 'black', bg = 'white')
watt.grid(row = 4, column = 1)

def checkSerialPort():
    if serialObj.isOpen() and serialObj.in_waiting:
        recentPacket = serialObj.readline()
        recentPacketString = recentPacket.decode('utf').rstrip('\n')

        # BMS STATE
        if (recentPacketString == "Charging/Balancing in progress now, not all cells in acceptable range"):
            state = Label(dataFrame, text="Balancing Cells", font = ('Calibri', 13), fg = 'black', bg = 'yellow', height=1, width=20)
            state.grid(row = 1, column = 1)
        if (recentPacketString == "Charging now..."):
            state = Label(dataFrame, text="Charging", font = ('Calibri', 13), fg = 'white', bg = 'red', height=1, width=20)
            state.grid(row = 1, column = 1)
        if (recentPacketString == "Stabilizing voltage for 6 seconds"):
            state = Label(dataFrame, text="Stabilizing", font = ('Calibri', 13), fg = 'black', bg = 'yellow', height=1, width=20)
            state.grid(row = 1, column = 1)
        if (recentPacketString == "Discharging Cell "):
            state = Label(dataFrame, text="Discharging", font = ('Calibri', 13), fg = 'white', bg = 'green', height=1, width=20)
            state.grid(row = 1, column = 1)
        
        # BATTERY STATES
        if ("top" in recentPacketString) == True:
            numbers1 = []
            for char in recentPacketString:
                if char.isdigit():
                    numbers1.append(int(char))
            volts = str(numbers1) + "V"
            bone = Label(dataFrame, text=volts, font = ('Calibri', 13), fg = 'white', bg = 'blue', height=10, width=5)
            bone.grid(row = 3, column = 0)

        if ("bot" in recentPacketString) == True:
            numbers2 = []
            for char in recentPacketString:
                if char.isdigit():
                    numbers2.append(int(char))
            volts = str(numbers2) + "V"
            bone = Label(dataFrame, text=volts, font = ('Calibri', 13), fg = 'white', bg = 'green', height=10, width=5)
            bone.grid(row = 3, column = 2)

        # WATTAGE
        if ("wattage" in recentPacketString) == True:
            numbers3 = []
            for char in recentPacketString:
                if char.isdigit():
                    numbers3.append(int(char))
            volts = str(numbers2) + "W"
            bone = Label(dataFrame, text=volts, font = ('Calibri', 13), fg = 'white', bg = 'blue')
            bone.grid(row = 5, column = 1)


while True:
    root.update()
    checkSerialPort()
    # dataCanvas.config(scrollregion=dataCanvas.bbox("all"))