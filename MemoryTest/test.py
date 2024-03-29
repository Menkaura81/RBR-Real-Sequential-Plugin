from ReadWriteMemory import ReadWriteMemory  # Memory writing
import struct
import keyboard
import time

def ieee754(value):
    #Repackages values to 32bit floating point
    packed_v = struct.pack('>L', value)
    return struct.unpack('>f', packed_v)[0]

rwm = ReadWriteMemory()
process = rwm.get_process_by_name('RichardBurnsRally_SSE.exe')
process.open() 
print(process)       
# Base address. Got from the pointer we have
#x_pointer = process.get_pointer(0x007eac48)
x_pointer = process.get_pointer(0x0165FC68)
CarPtr = process.get_pointer(0x8EF660)  
#base_address = process.get_base_address()
#print(base_address)

#speed = process.read(x_pointer) + 0xc
RPM = process.read(x_pointer) + 0x10
#temp = process.read(x_pointer) + 0x14
#turbo = process.read(x_pointer) + 0x18
#session_time = process.read(x_pointer) + 0x15c
brake_pressure = process.read(CarPtr) + 0x520

f1_address = process.get_pointer(0x0165FC68, offsets=[-0x96490])
#f2_address = base_address + 0x003CF3A0
#f3_address = 0x10A02D78

r1_address = process.get_pointer(0x0165FC68, offsets=[-0x963E0])
#r2_address = 0x109FD228
#r3_address = 0x10A02E28

frontBrake = process.read(f1_address)
rearBrake = process.read(r1_address)
maxPressure = frontBrake + rearBrake

print(frontBrake)
print(rearBrake)
print(maxPressure)

while(True):
    
    if keyboard.is_pressed('q') and rearBrake < maxPressure :

        time.sleep(0.2)        
        rearBrake += 200000
        frontBrake -= 200000        
        
        process.write(f1_address, frontBrake)
        #process.write(f2_address, frontBrake)
        #process.write(f3_address, frontBrake)

        process.write(r1_address, rearBrake)
        #process.write(r2_address, rearBrake)
        #process.write(r3_address, rearBrake)

        #rearBrake = process.read(r1_address)
        #frontBrake = process.read(f1_address)


    if keyboard.is_pressed('w') and ieee754(frontBrake) < maxPressure:

        time.sleep(0.2) 
                        
        frontBrake += 200000
        rearBrake -= 200000
        
        process.write(f1_address, frontBrake)
        #process.write(f2_address, frontBrake)
        #process.write(f3_address, frontBrake)

        process.write(r1_address, rearBrake)
        #process.write(r2_address, rearBrake)
        #process.write(r3_address, rearBrake)
    
        #frontBrake = process.read(f1_address) 
        #wwqqqrearBrake = process.read(r1_address)

    
    #print("Brake Pressure: ", rpm)
    #print("Engine RPM: ", rpm, end='\r')


"""
    
while(True):
    print("Race Time: %s:%s" %(RaceHr , RaceMin), end="\r")
    print("Engine: %s RPM   Gear: %s   Throttle: %s   Brake: %s   Clutch: %s" %(EngineRpm , ControlGear, ControlThrottle , ControlBrake, ControlClutch))
    print("Coolant Temp: %s°C" % EngineCoolantTemperature)
    print("")
    print("Brake Disk Temp")
    print("LF: %s°C	RF: %s°C" % (LFBrakeDiskTemp , RFBrakeDiskTemp))
    print("LB: %s°C	RB: %s°C" % (LBBrakeDiskTemp , RBBrakeDiskTemp))





# Found pointers:
#          Clutch = 0x86C
#          Gear = 0x1100
# I can read foot brake pressure from udp telemetry but is total (fron + rear)
"""