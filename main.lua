function test()
	--io.write("eee")
	--for i,v in pairs(_ENV) do
	--	print(i)
	--end
	for i=0, 7 do
		print(ADC.readChannel(i))
	end
end
