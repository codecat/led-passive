return function(frame)
	for i = 0, 300 do
		local n = i + frame
		if n <= 150 then
			set(i, n, 0, 255 - n)
		else
			local x = n - 150
			set(i, 255 - x, 0, x)
		end
	end
end
