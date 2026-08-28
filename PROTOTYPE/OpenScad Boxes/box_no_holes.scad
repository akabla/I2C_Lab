include <lasercut.scad>; 

thickness = 4;
x = 150;
y = 220;
z = 80;

dt = 0.01;

sides = 5;

tab_num_z = 3;
cutout_size_z = (y-2*thickness) / (2*tab_num_z + 1);

tab_num_y = 4;
cutout_size_y = (z-2*thickness) / (2*tab_num_y + 1);

unit = 70;
box_add_cutouts = [
    [for( i = [cutout_size_z:2*cutout_size_z:y - 
        cutout_size_z])
        [x-3*thickness,i,thickness,cutout_size_z]],
    [for( i = [cutout_size_z:2*cutout_size_z:y - 
        cutout_size_z])
        [x-3*thickness,i,thickness,cutout_size_z]],
    
    [ for( i = [cutout_size_y:2*cutout_size_y:z - cutout_size_y])
        [x-3*thickness,i,thickness,cutout_size_y] ],
    
    [ for( i = [cutout_size_y:2*cutout_size_y:z - cutout_size_y])
        [x-3*thickness,i,thickness,cutout_size_y] ],  
];

square_cutouts = [
    for( i = [0:2*cutout_size_z:y - 
        cutout_size_z])
    [-dt,i+thickness,thickness,cutout_size_z],
    [-dt,0-dt,thickness,thickness+10*dt],
    [-dt,y-thickness-dt,thickness,thickness+10*dt],
    
    for( i = [0:2*cutout_size_z:y - 
        cutout_size_z])
    [z+dt-thickness,i+thickness,thickness,cutout_size_z],
    [z+dt-thickness,0-dt,thickness,thickness+10*dt],
    [z+dt-thickness,y-thickness-dt,thickness,thickness+10*dt],
    
    for( i = [0:2*cutout_size_y:z - 
        cutout_size_y])
    [i+thickness,-dt,cutout_size_y,thickness],
    [0-dt,0-dt,thickness+10*dt,thickness],
    [z-thickness-dt,0-dt,thickness+10*dt,thickness],
    
    for( i = [0:2*cutout_size_y:z - 
        cutout_size_y])
    [i+thickness,y+dt-thickness,cutout_size_y,thickness],
    [0-dt,y-thickness-dt,thickness+10*dt,thickness],
    [z-thickness-dt,y-thickness-dt,thickness+10*dt,thickness],
    
    [3*thickness,3*thickness,z-6*thickness,y-6*thickness]
    
];


   
led_radius = 2.45;
led_y = 23;
pitch = 2.54;

    circles_remove = [ 
[],
[   //LED holes
     for (i = [0 : 1 : 8]) 
     [led_radius,x-12-45,32.5 + i*pitch * 6],  
    
     [led_radius,x-27,y-8-15]
],
[],
[[led_radius,x-12-45,9],],
[],
];


color("Gold",0.5) 
//translate([x,-y/2,0])
//rotate([0,270,0])
    lasercutoutBox(thickness = thickness, x=x, y=y, z=z, 
    sides=sides, num_fingers=6,
    cutouts_a = box_add_cutouts);

color("Blue",0.5)
translate([x+3*thickness,0,0])
rotate([0,270,0])
    lasercutoutSquare(thickness, x=z, y=y,
    cutouts = square_cutouts);
    
color("Red",0.5)
translate([x+7*thickness,thickness,thickness])
rotate([0,270,0])
    lasercutoutSquare(thickness, x=z-2*thickness, y=y-2*thickness,
    circles_remove = [[7.5,z/2-thickness,y/2-thickness]]);
