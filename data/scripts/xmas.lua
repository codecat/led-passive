local width = 30
local gap = 60

return function(frame)
	clear(0, 0, 0)

	for i = 0, 299 do
		local n = (i + frame) % (width + gap)
		if n < width then
			add(i, 0, (1 - n / width) * 255, 0)
		end
	end

	for i = 0, 299 do
		local n = (i - frame) % (width + gap)
		if n < width then
			add(i, n / width * 255, 0, 0)
		end
	end
end
