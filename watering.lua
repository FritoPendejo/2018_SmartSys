if (delayGo=="yes") then
    dofile("delaying.lua")
	print("proceed to delay")
	gpio.write(1,gpio.LOW)
    end
	
if (soilNow=="dry" and delayGo=="no") then
print("watering time")
watering=1
gpio.write(1,gpio.HIGH)
waterCount=waterCount+1
end

if (waterCount >= (waterDuration/systemClock)+1) then
	print("watering ended")
	gpio.write(1,gpio.LOW)
	soilNow="wet"
	waterCount=0
    delayGo="yes"
    watering=0
	end
	

