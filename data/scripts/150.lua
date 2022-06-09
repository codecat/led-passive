local width = 10
local gap = 50
local speed = 2
local huestart = 0
local huelen = 50
local huespeed = 2

return function(frame)
	clear(0, 0, 0)

	for strip = 0, 1 do
		if strip == 1 then
		end

		for i = 0, 299 do
			local pos = i + frame * speed
			local n = pos % (width + gap)
			if n < width then
				local h = huestart + (pos % huelen)
				r, g, b = hsv2rgb(h, 0.5, 1)
				local v = (1 - n / width)
				local movepos = i
				if strip == 1 then
					movepos = 299 - i
				end
				get_strip(strip):add(movepos, r*v, g*v, b*v)
			end
		end
	end

	huestart = (huestart + huespeed) % 360
end

