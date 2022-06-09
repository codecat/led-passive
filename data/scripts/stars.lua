local stars = {}
local star_hue = 300
local star_hue_random = 50
local star_hue_add = 0
local star_per_time = 0
local star_per_time_random = 4
local star_seconds = 0.1
local star_speed = 4
local star_speed_random = 10
local star_movement = 1
local star_clear = true

local function deg2rad(deg)
	return deg * math.pi / 180
end

local function is_star_at(strip, pixel)
	for _, v in ipairs(stars) do
		if v.strip == strip and v.pixel == pixel then
			return true
		end
	end
	return false
end

local function new_star()
	local strip, pixel
	local tries = 0
	repeat
		strip = math.floor(math.random() * 2)
		pixel = math.floor(math.random() * 300)
		tries = tries + 1
	until tries > 5 or not is_star_at(strip, pixel)

	local h = star_hue + star_hue_random * math.random()
	local r, g, b = hsv2rgb(h, 0.5, 1)

	table.insert(stars, {
		strip = strip,
		pixel = pixel,
		rgb = {
			r = r,
			g = g,
			b = b,
		},
		value = 0,
		speed = star_speed + math.random() * star_speed_random,
		reverse = false,
		movement = star_movement * (math.random() * 2 - 1),
	})
end

local function new_stars()
	local num = star_per_time + math.floor(math.random() * star_per_time_random)
	if num == 0 then
		return
	end

	for i = 1, num do
		new_star()
	end

	star_hue += star_hue_add
end

return function(frame)
	local fps = get_fps()
	if frame % (fps * star_seconds) == 0 then
		new_stars()
	end

	local remove = {}

	if star_clear then
		clear(0, 0, 0)
	end

	for i, v in ipairs(stars) do
		if v.reverse then
			v.value = v.value - v.speed
			if v.value < 0 then
				v.value = 0
				table.insert(remove, i)
			end
		else
			v.value = v.value + v.speed
			if v.value > 255 then
				v.value = 255
				v.reverse = true
				v.speed /= 2
			end
		end

		local r, g, b
		r = v.rgb.r * (v.value / 255)
		g = v.rgb.g * (v.value / 255)
		b = v.rgb.b * (v.value / 255)
		add(v.strip, (v.pixel + frame * v.movement) % 300, r, g, b)
	end

	for _, v in ipairs(remove) do
		table.remove(stars, v)
	end
end
