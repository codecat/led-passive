local HSVToRGB = dofile("scripts/hsv.lua")

return function(frame)
	for i = 0, 300 do
		local h = ((i/2) + frame % 360) / (2 * math.pi)
		r, g, b = HSVToRGB(h, 0.5, 1)
		set(i, r, g, b)
	end
end
