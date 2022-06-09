local hr, hg, hb = 255, 0, 0
local mr, mg, mb = 255, 127, 0
local sr, sg, sb = 0, 0, 255

return function(frame)
	local d = os.date("*t")

	local hour_fraction = (d.hour + d.min / 60 + d.sec / 3600) / 24
	local min_fraction = (d.min + d.sec / 60) / 60

	clear(0, 0, 0)

	set(hour_fraction * 300, hr, hg, hb)
	set(min_fraction * 300, mr, mg, mb)
	set(d.sec / 60 * 300, sr, sg, sb)
end
