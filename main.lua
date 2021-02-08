print("file loaded")

function init()
	print("init loaded")
end

function update(dt)
	--io.write("eee")
	--for i,v in pairs(_ENV) do
	--	print(i)
	--end
	print(ADC.readChannel(0)) --read channel 0 as it's the only connected one rn
	--os.leave() --leaves the update loop
end

function uninit()
	print("uninit loaded")
end
