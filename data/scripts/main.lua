print("MAIN SCRIPT")

clear(0, 255, 0, 0)
clear(1, 0, 255, 0)

--[[
return function()
	clear(0, 0, 0)
	for i = 0, 150 do
		set(i, i, 0, 255 - i)
	end
end
]]

return "test"
