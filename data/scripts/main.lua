local next_seconds = 10

set_scene("150")

return function(frame)
	local fps = get_fps()
	if frame > 0 and frame % (fps * next_seconds) == 0 then
		next_scene()
	end
end
