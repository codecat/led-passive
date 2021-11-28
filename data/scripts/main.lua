return function(frame)
	if frame % 60 == 0 then
		next_scene()
		reset_frame_counter()
	end
end
