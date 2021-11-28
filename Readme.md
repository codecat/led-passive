# Passive LED server
Scriptable LED server that you can just keep running all the time on a Raspberry Pi.

## Requirements
* [Raspberry Pi 4](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/) or newer (older might work, but they are slower)
* Any serial connection with the [OPC](http://openpixelcontrol.org/) protocol
	* I use a [Teensy 4.0](https://www.pjrc.com/store/teensy40.html) with [OctoWS2811](https://www.pjrc.com/teensy/td_libs_OctoWS2811.html) and [this program](https://gist.github.com/codecat/6126ff59a0d24a6b86d4d9f89336efa2)
* [WS2811](https://www.amazon.com/WS2811/s?k=WS2811) addressable LED's (or anything else that you can control with OPC over serial)

## Configuration
The configuration is built from a series of blocks. Here's an example configuration file showing the most commonly used options:

```
# The serial port on which to connect to
port = "/dev/ttyACM0"

# Target FPS (can reliably be 60 or higher as well depending on your hardware)
fps = 30

# Main Lua script that always runs
script = "scripts/main.lua"

# Define the amount of strips you have (eg. OPC channels) and the amount of pixels in each
strips {
	{ pixels = 300 }
	{ pixels = 300 }
}

# Define the scenes
scenes {
	# The default scene is always the first scene to be displayed
	default {}

	# A scene can just be a color
	red { r = 255 }
	green { g = 255 }
	blue { b = 255 }

	test {
		# When the "test" scene is ran, the update function from "test.lua" is called
		script = "scripts/test.lua"
	}
}
```

## Scripting
Lua scripts should return a function that will be called every frame. In this function you can set color values.

Use `entr` to automatically restart the server every time the config is changed, or a script is changed.

```
find config scripts | entr -r ./led-passive
```

# API
The following functions are available to Lua:

* `set_scene(name)` sets the current scene
* `next_scene()` switches to the next scene
* `prev_scene()` switches to the previous scene
* `reset_frame_counter()` resets the frame counter passed to update functions
* `set_fps(fps)` sets the target FPS
* `clear(r, g, b)` clears all strips with the a color
* `clear(strip, r, g, b)` clears the given strip with a color
* `set(pixel, r, g, b)` sets a pixel to a color
* `set(strip, pixel, r, g, b)` sets a pixel on the given strip to a color

# License
MIT License
