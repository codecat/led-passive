local width = 10
local gap = 20

return function(frame)
	clear(0, 0, 0)

	for i = 0, 299 do
		local n = (i + frame) % (width + gap)
		if n < width then
			local h = (i + frame * 3 % 360)
			r, g, b = hsv2rgb(h, 0.5, 1)
			local v = (1 - n / width)
			add(i, r*v, g*v, b*v)
		end
	end
end

