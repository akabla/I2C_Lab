include <lasercut.scad>; 

//change
THICKNESS = 6;      // thickness of box walls
WIDTH     = 150;    // x direction
LENGTH    = 220;    // y direction
HEIGHT    = 80;     // z direction

// for the support in the back(blue)
TAB_NUM_Y = 3; //side faces
TAB_NUM_Z = 6; //top and bottom faces

// for the LED perfboard support
SUPPORT_THICKNESS = 3;
SUPPORT_DISTANCE = 10; //measured from the underside of the top face



//DO NOT CHANGE
SIDES = 5;
EPS = 0.01;

// ---------------------------- //
cutout_size_z = (LENGTH-2*THICKNESS) / (2*TAB_NUM_Z + 1);
cutout_size_y = (HEIGHT-2*THICKNESS) / (2*TAB_NUM_Y + 1);

//box cutouts to accomodate  blue and maroon pieces

box_cutouts = [
    [//for backside support (blue)
    for(i=[cutout_size_z:
            2*cutout_size_z:
            LENGTH-cutout_size_z])
       [WIDTH-3*THICKNESS,
        i,
        THICKNESS,
        cutout_size_z]],
    [//for backside support (blue)
    for(i=[cutout_size_z:
            2*cutout_size_z:
            LENGTH-cutout_size_z])
       [WIDTH-3*THICKNESS,
        i,
        THICKNESS,
        cutout_size_z]],
    [//for backside support (blue)
    for(i=[cutout_size_y:
            2*cutout_size_y:
            HEIGHT-cutout_size_y])
       [WIDTH-3*THICKNESS,
        i,
        THICKNESS,
        cutout_size_y],
    //for perfboard support(maroon)
    [2*THICKNESS,
        HEIGHT-2*THICKNESS-SUPPORT_DISTANCE,
        3*THICKNESS,
        SUPPORT_THICKNESS],
    [WIDTH-9*THICKNESS,
        HEIGHT-2*THICKNESS-SUPPORT_DISTANCE,
        3*THICKNESS,
        SUPPORT_THICKNESS]],
    [//for backside support (blue)
    for(i=[cutout_size_y:
            2*cutout_size_y:
            HEIGHT - cutout_size_y])
       [WIDTH-3*THICKNESS,
        i,
        THICKNESS,
        cutout_size_y],
    //for perfboard support(maroon)
    [2*THICKNESS,
        HEIGHT-2*THICKNESS-SUPPORT_DISTANCE,
        3*THICKNESS,
        SUPPORT_THICKNESS],
        
    [WIDTH-9*THICKNESS,
        HEIGHT-2*THICKNESS-SUPPORT_DISTANCE,
        3*THICKNESS,
        SUPPORT_THICKNESS]],
    [//for perfboard support(maroon)
    [HEIGHT-2*THICKNESS-SUPPORT_DISTANCE,
        2*THICKNESS,
        SUPPORT_THICKNESS,
        3*THICKNESS],
    [HEIGHT-2*THICKNESS-SUPPORT_DISTANCE,
        LENGTH/2-2.5*THICKNESS,
        SUPPORT_THICKNESS,
        3*THICKNESS],
    [HEIGHT-2*THICKNESS-SUPPORT_DISTANCE,
        LENGTH-7*THICKNESS,
        SUPPORT_THICKNESS,
        3*THICKNESS]]
];
//cutouts in blue suppport to fit into the box
CUTOUTS_BLUE = [
    //side tabs
    for(i=[0:
           2*cutout_size_z:
           LENGTH-cutout_size_z])
    [-EPS,i+THICKNESS,THICKNESS,cutout_size_z],
    
    for(i=[0:
           2*cutout_size_z:
           LENGTH-cutout_size_z])
    [HEIGHT+EPS-THICKNESS,i+THICKNESS,THICKNESS,cutout_size_z],
    
    for(i=[0:
           2*cutout_size_y:
           HEIGHT-cutout_size_y])
    [i+THICKNESS,-EPS,cutout_size_y,THICKNESS],
    
    for(i=[0:
           2*cutout_size_y:
           HEIGHT-cutout_size_y])
    [i+THICKNESS,LENGTH+EPS-THICKNESS,cutout_size_y,THICKNESS],
    
    // central square cutout
    [3*THICKNESS,3*THICKNESS,HEIGHT-6*THICKNESS,LENGTH-6*THICKNESS],
    
    //fixing corners
    [-EPS/2,-EPS/2,THICKNESS+EPS,THICKNESS+EPS],
    [-EPS/2,LENGTH-THICKNESS-EPS/2,THICKNESS+EPS,THICKNESS+EPS],
    [HEIGHT-THICKNESS-EPS/2,-EPS/2,THICKNESS+EPS,THICKNESS+EPS], 
    [HEIGHT-THICKNESS-EPS/2,LENGTH-THICKNESS-EPS/2,THICKNESS+EPS,THICKNESS+EPS]
    ];

//for the perfboard support(maroon)
CUTOUTS_MAROON = [
            //side face
           [-EPS,-EPS,3*THICKNESS,THICKNESS],
           [6*THICKNESS,-EPS,WIDTH-14*THICKNESS,THICKNESS],
           [-EPS,-EPS,THICKNESS,3*THICKNESS],
           //front face
           [-EPS,-EPS,THICKNESS,3*THICKNESS],
           [-EPS,6*THICKNESS,THICKNESS,LENGTH/2-7.5*THICKNESS],
           [-EPS,LENGTH/2+1.5*THICKNESS,THICKNESS,LENGTH/2-7.5*THICKNESS],
           [-EPS,LENGTH-3*THICKNESS,THICKNESS,3*THICKNESS],
           //side face
           [-EPS,LENGTH-THICKNESS+EPS,3*THICKNESS,THICKNESS],
           [6*THICKNESS,LENGTH-THICKNESS+EPS,WIDTH-14*THICKNESS,THICKNESS],
           //central
           [2.5*THICKNESS,2.5*THICKNESS,WIDTH-5*THICKNESS,LENGTH-5*THICKNESS]
           ];

color("BEIGE",1) 
//translate([x,-y/2,0])
//rotate([0,270,0])
    lasercutoutBox(thickness=THICKNESS,x=WIDTH,y=LENGTH,z=HEIGHT, 
    sides=SIDES, num_fingers=6,
    cutouts_a = box_cutouts);

color("Blue",1)
translate([WIDTH+3*THICKNESS,0,0])
rotate([0,270,0])
    lasercutoutSquare(THICKNESS, x=HEIGHT, y=LENGTH,
    cutouts = CUTOUTS_BLUE);
    
color("DarkGreen",1)
translate([WIDTH+7*THICKNESS,THICKNESS,THICKNESS])
rotate([0,270,0])
    lasercutoutSquare(THICKNESS, x=HEIGHT-2*THICKNESS, y=LENGTH-2*THICKNESS,
    circles_remove = [[7.5,HEIGHT/2-THICKNESS,LENGTH/2-THICKNESS]]);

color("MAROON",1)
translate([0,0,HEIGHT-THICKNESS+SUPPORT_DISTANCE])
lasercutoutSquare(SUPPORT_THICKNESS,x=WIDTH-5*THICKNESS,y=LENGTH,
    cutouts = CUTOUTS_MAROON); 
