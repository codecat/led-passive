return function(frame)
	local fps = get_fps()
	local seconds = 10
	if frame > 0 and frame % (fps * seconds) == 0 then
		next_scene()
	end
end
