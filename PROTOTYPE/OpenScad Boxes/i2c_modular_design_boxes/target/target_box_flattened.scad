use <lasercut.scad>;
$fn=60;

x = 150; y =220; z = 80;

led_radius = 2.45; pitch = 2.54;
circles_remove = [/*[0.5,x-8-10,y-8-8.25],
                  [0.5,x-8-10-68.5,y-8-8.25-195.5],
                  [0.5,x-8-10-68.5,y-8-8.25],
                  [0.5,x-8-10,y-8-8.25-195.5],

[led_radius,x-8-18-10,y-8-28.75],
[led_radius,x-8-18-10-12*pitch,y-8-28.75-3*pitch],
[led_radius,x-8-18-10-12*pitch,y-8-28.75-10*pitch],
[led_radius,x-8-18-10-12*pitch,y-8-28.75-17*pitch],
[led_radius,x-8-18-10-12*pitch,y-8-28.75-24*pitch],
[led_radius,x-8-18-10-12*pitch,y-8-28.75-31*pitch],
[led_radius,x-8-18-10-12*pitch,y-8-28.75-38*pitch],
[led_radius,x-8-18-10-12*pitch,y-8-28.75-45*pitch],
[led_radius,x-8-18-10-12*pitch,y-8-28.75-52*pitch],
[led_radius,x-8-18-10-12*pitch,y-8-28.75-59*pitch]*/
[led_radius,x-12-15,y-8-20],
[led_radius,x-12-15-6.5*pitch,y-8-20-13.5*pitch],
[led_radius,x-12-15-14.5*pitch,y-8-20-13.5*pitch],
[led_radius,x-12-15-22.5*pitch,y-8-20-13.5*pitch],
[led_radius,x-12-15-30.5*pitch,y-8-20-13.5*pitch],

[led_radius,x-12-15-6.5*pitch,y-8-20-26.5*pitch],
[led_radius,x-12-15-14.5*pitch,y-8-20-26.5*pitch],
[led_radius,x-12-15-22.5*pitch,y-8-20-26.5*pitch],
[led_radius,x-12-15-30.5*pitch,y-8-20-26.5*pitch],

[led_radius,x-12-15-6.5*pitch,y-8-20-39.5*pitch],
[led_radius,x-12-15-14.5*pitch,y-8-20-39.5*pitch],
[led_radius,x-12-15-22.5*pitch,y-8-20-39.5*pitch],
[led_radius,x-12-15-30.5*pitch,y-8-20-39.5*pitch],

[led_radius,x-12-15-6.5*pitch,y-8-20-52.5*pitch],
[led_radius,x-12-15-14.5*pitch,y-8-20-52.5*pitch],
[led_radius,x-12-15-22.5*pitch,y-8-20-52.5*pitch],
[led_radius,x-12-15-30.5*pitch,y-8-20-52.5*pitch],
];

power = [[led_radius,3*x/4,10]];

module flat(){
projection(cut = false)

lasercutout(thickness = 4, 
          points = [[0, 0], [146, 0], [146, 212], [0, 212], [0, 0]]
        , simple_tabs = [[180, -2, 0], [180, -2, 216]]
        , finger_joints = [[0, 1, 6], [180, 1, 6], [90, 1, 6]]
        , cutouts = [[138, 30.2857, 4, 30.2857], [138, 90.8571, 4, 30.2857], [138, 151.429, 4, 30.2857]]
          //circles_remove = circles_remove
        ) 

lasercutout(thickness = 4, 
          points = [[0, 0], [146, 0], [146, 212], [0, 212], [0, 0]]
        , simple_tabs = [[180, -2, 0]]
        , finger_joints = [[0, 0, 6], [180, 1, 6], [90, 1, 6]]
        , cutouts = [[138, 30.2857, 4, 30.2857], [138, 90.8571, 4, 30.2857], [138, 151.429, 4, 30.2857]],
        circles_remove = circles_remove
        ) 

lasercutout(thickness = 4, 
          points = [[0, 0], [146, 0], [146, 72], [0, 72], [0, 0]]
        , finger_joints = [[0, 1, 6], [180, 0, 6], [90, 1, 6]]
        , cutouts = [[138, 8, 4, 8], [138, 24, 4, 8], [138, 40, 4, 8], [138, 56, 4, 8], [138, 72, 4, 8]]
        ) 

lasercutout(thickness = 4, 
          points = [[0, 0], [146, 0], [146, 72], [0, 72], [0, 0]]
        , simple_tabs = [[0, -2, 72]]
        , finger_joints = [[0, 1, 6], [180, 1, 6], [90, 1, 6]]
        , cutouts = [[138, 8, 4, 8], [138, 24, 4, 8], [138, 40, 4, 8], [138, 56, 4, 8], [138, 72, 4, 8]],
        circles_remove=power
        ) 

lasercutout(thickness = 4, 
          points = [[0, 0], [72, 0], [72, 212], [0, 212], [0, 0]]
        , finger_joints = [[0, 0, 6], [180, 1, 6], [90, 0, 6], [270, 1, 6]]
        ) 

lasercutout(thickness = 4, 
          points = [[0, 0], [80, 0], [80, 220], [0, 220], [0, 0]]
        , cutouts = [[-0.01, 4, 4, 30.2857], [-0.01, 64.5714, 4, 30.2857], [-0.01, 125.143, 4, 30.2857], [-0.01, 185.714, 4, 30.2857], [-0.01, -0.01, 4, 4.1], [-0.01, 215.99, 4, 4.1], [76.01, 4, 4, 30.2857], [76.01, 64.5714, 4, 30.2857], [76.01, 125.143, 4, 30.2857], [76.01, 185.714, 4, 30.2857], [76.01, -0.01, 4, 4.1], [76.01, 215.99, 4, 4.1], [4, -0.01, 8, 4], [20, -0.01, 8, 4], [36, -0.01, 8, 4], [52, -0.01, 8, 4], [68, -0.01, 8, 4], [-0.01, -0.01, 4.1, 4], [75.99, -0.01, 4.1, 4], [4, 216.01, 8, 4], [20, 216.01, 8, 4], [36, 216.01, 8, 4], [52, 216.01, 8, 4], [68, 216.01, 8, 4], [-0.01, 215.99, 4.1, 4], [75.99, 215.99, 4.1, 4], [12, 12, 56, 196]]
        ) 

lasercutout(thickness = 4, 
          points = [[0, 0], [72, 0], [72, 212], [0, 212], [0, 0]]
        , circles_remove = [[7.5, 36, 106]]
        ) 

;
}

flat();
