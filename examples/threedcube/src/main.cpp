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