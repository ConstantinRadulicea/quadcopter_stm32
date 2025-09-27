function update3DPlane(h, roll, pitch, yaw)
    Rx = [1 0 0; 0 cos(roll) -sin(roll); 0 sin(roll) cos(roll)];
    Ry = [cos(pitch) 0 sin(pitch); 0 1 0; -sin(pitch) 0 cos(pitch)];
    Rz = [cos(yaw) -sin(yaw) 0; sin(yaw) cos(yaw) 0; 0 0 1];
    R = Rz * Ry * Rx;

    fuselage = (R * [0 1; 0 0; 0 0])';
    wing     = (R * [0.3 0.3; -0.5 0.5; 0 0])';
    tail     = (R * [0 0; -0.2 0.2; 0 0])';
    vtail    = (R * [0 0; -0.05 0; 0 0.2])';

    set(h.fuselage, 'XData', fuselage(:,1), 'YData', fuselage(:,2), 'ZData', fuselage(:,3));
    set(h.wing,     'XData', wing(:,1),     'YData', wing(:,2),     'ZData', wing(:,3));
    set(h.tail,     'XData', tail(:,1),     'YData', tail(:,2),     'ZData', tail(:,3));
    set(h.vtail,    'XData', vtail(:,1),    'YData', vtail(:,2),    'ZData', vtail(:,3));
end
