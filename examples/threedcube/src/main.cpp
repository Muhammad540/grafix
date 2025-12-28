/**
 * =======================================================================================
 * 3D WIREFRAME RENDERING ENGINE (Software Renderer)
 * =======================================================================================
 * 
 * This program implements a classic "3D Pipeline" to render a rotating cube on a 
 * 128x64 OLED display. Since the SSD1306 has no GPU, all mathematical calculations
 * (Rotation, Projection, Rasterization) are performed on the Arduino's CPU.
 * 
 * THE RENDERING PIPELINE:
 * The 3D effect is achieved by passing every vertex of the cube through a series of 
 * transformation functions. The data flows like this:
 * 
 *    [Model Space] -> [World Space] -> [View Space] -> [Screen Space]
 * 
 * 1. DEFINITION (Model Space):
 *    The cube is defined by 8 points (vertices) centered at (0,0,0). 
 *    Coordinates range roughly from -0.7 to +0.7, this is tuneable
 * 
 * 2. ROTATION (World Space):
 *    We apply a Rotation Matrix to every point. The `rotate_xz` function spins points 
 *    around the Y-axis link to formula: https://en.wikipedia.org/wiki/Rotation_matrix
 *      new_x = x * cos(angle) - z * sin(angle)
 *      new_z = x * sin(angle) + z * cos(angle)
 * 
 * 3. TRANSLATION (View Space):
 *    If we render the cube at (0,0,0), the "Camera" is inside the cube. 
 *    The `translate_z` function adds a distance (`dz`) to the Z-axis to push the 
 *    object away from the camera so it is visible in front of us.
 * 
 * 4. PROJECTION (Perspective Divide):
 *    Things further away look smaller.
 *    We divide the X and Y coordinates by the depth (Z):
 *      x_projected = x / z
 *      y_projected = y / z
 *    As Z increases (gets further), the resulting X and Y get closer to 0 (the center).
 * 
 * 5. SCREEN MAPPING (Screen Space):
 *    The projected points are floating point numbers (e.g -0.2 to +0.2).
 *    We map them to physical pixels (0 to 127).
 *    
 *    CRITICAL: We use a fixed `scale` factor (32.0) for BOTH X and Y axes.
 *    Do NOT stretch to fit screen width/height, otherwise the cube would look 
 *    like a flat rectangle (cuboid) because the screen aspect ratio is 2:1.
 * 
 *    Final Pixel X = (Screen_Width / 2)  + (x_projected * Scale)
 *    Final Pixel Y = (Screen_Height / 2) - (y_projected * Scale)
 * 
 * 6. RASTERIZATION:
 *    We loop through the "Faces" list, which tells us which vertices connect to which.
 *    We draw lines between these calculated pixel coordinates using the Grafix library.
 * =======================================================================================
 */
#include <Arduino.h>
#include "grafix.h"
#include <math.h>

Grafix display;

struct Point3D {
  float x;
  float y;
  float z;
};

struct Point2D {
  int16_t x;
  int16_t y;
};

// 8 vretices of the cube in 3d space (think of this cube centered at the origin 0,0,0)
const Point3D cubePoints[8] = {
    { 0.7,  0.7,  0.7},
    {-0.7,  0.7,  0.7},
    {-0.7, -0.7,  0.7},
    { 0.7, -0.7,  0.7},

    { 0.7,  0.7, -0.7},
    {-0.7,  0.7, -0.7},
    {-0.7, -0.7, -0.7},
    { 0.7, -0.7, -0.7} 
};

struct Face {
  const uint8_t* indices;
  uint8_t length;
};

const uint8_t f0[] = {0, 1, 2, 3};
const uint8_t f1[] = {4, 5, 6, 7};
const uint8_t f2[] = {0, 4};
const uint8_t f3[] = {1, 5};
const uint8_t f4[] = {2, 6};
const uint8_t f5[] = {3, 7};

const Face fs[] = {
  {f0, 4}, {f1, 4}, 
  {f2, 2}, {f3, 2}, {f4, 2}, {f5, 2}
};
const uint8_t fs_count = 6;

Point3D rotate_xz(Point3D p, float angle) {
  float c = cos(angle);
  float s = sin(angle);
  return {
      p.x * c - p.z * s,
      p.y,
      p.x * s + p.z * c
  };
}

Point3D translate_z(Point3D p, float dz) {
  return {p.x, p.y, p.z + dz};
}

Point3D project(Point3D p){
  if (p.z == 0) p.z = 0.001;
  return {p.x/p.z, p.y/p.z, 0};
}

Point2D screen(Point3D p){
  float w = SCREEN_WIDTH;
  float h = SCREEN_HEIGHT;
  float scale = 32.0;
  return {
    (int16_t)((w * 0.5) + (p.x * scale)), 
    (int16_t)((h * 0.5) - (p.y * scale)) 
  };
}

void line(Point2D p1, Point2D p2){
  display.drawLine(p1.x,p1.y,p2.x,p2.y,1);
}

float dz = 2.0;
float angle = 0.0;

void setup(){
  Serial.begin(115200);
  display.begin();
}

void loop(){
  angle += 0.05;
  display.clear();

  for (uint8_t i = 0; i < fs_count; i++){
    const Face f = fs[i];
    for (uint8_t j = 0; j < f.length; j++){
      uint8_t idx_a = f.indices[j];
      uint8_t idx_b = f.indices[(j+1)%f.length];

      Point3D a = cubePoints[idx_a];
      Point3D b = cubePoints[idx_b];

      line(
        screen(project(translate_z(rotate_xz(a, angle), dz))),
        screen(project(translate_z(rotate_xz(b, angle), dz)))
      );
    }
  }
  display.display();
  delay(16);
}