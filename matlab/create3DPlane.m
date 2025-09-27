function handles = create3DPlane()
    % Empty segments (to be updated)
    figure('Name', '3D Plane');
    handles.fuselage = plot3([0 0], [0 0], [0 0], 'k', 'LineWidth', 3); hold on;
    handles.wing     = plot3([0 0], [0 0], [0 0], 'b', 'LineWidth', 2);
    handles.tail     = plot3([0 0], [0 0], [0 0], 'g', 'LineWidth', 2);
    handles.vtail    = plot3([0 0], [0 0], [0 0], 'r', 'LineWidth', 2);
    axis equal;
    grid on;
    xlabel('X'); ylabel('Y'); zlabel('Z');
    xlim([-1 1]); ylim([-1 1]); zlim([-1 1]);
    view(3);
end
