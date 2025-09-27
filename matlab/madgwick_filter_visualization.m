close all;
clc;
clear;


% -------------------------
% USER SETTINGS
% -------------------------
port = "COM5";       % Change to your COM port
baud = 115200;       % Baud rate from your microcontroller
serial_timeout = 2;  % Seconds to wait on serial init

% -------------------------
% Setup Serial
% -------------------------
s = serialport(port, baud, "Timeout", serial_timeout);
configureTerminator(s, "LF");
flush(s);
pause(2); % Wait for MCU to start sending
flush(s);

% -------------------------
% Main Loop
% -------------------------
handles = create3DPlane();
target_dt = 1 / 60;  % 60 Hz → ~16.67 ms
t_start = tic;  % Start timer
while true
    try
    

    line = readline(s);
    angles = sscanf(line, '%f;%f;%f');
    if numel(angles) ~= 3, continue; end
    roll = deg2rad(angles(1));
    pitch = deg2rad(-angles(2));
    yaw = deg2rad(-angles(3));

    elapsed = toc(t_start);
    if elapsed >= target_dt
    t_start = tic;  % Start timer

    update3DPlane(handles, roll, pitch, yaw);
    drawnow limitrate;
    end;
    catch err
        disp("Error: " + err.message);
        break;
    end
end
