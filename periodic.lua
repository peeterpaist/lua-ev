local ev = require'ev'
local loop = ev.Loop.default
local periodic = nil

function start_device_read_loop()
    if periodic ~= nil then
        periodic:stop(loop)
    end

    periodic = ev.Periodic.new(read_devices, 0, 900)
    periodic:start(loop)
    periodic:priority(ev.MAXPRI)
end

function read_devices()
	print(loop:now(), "Reading devices!")
end

print(loop:now(), "Starting...")

start_device_read_loop()

loop:loop()

print(loop:now(), "Normal quit")