#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>

class Color {
public:
  Color() : r(0), g(0), b(0) {}
  Color(int r, int g, int b) : r(r), g(g), b(b) {}
  int r, g, b;
  operator int() { return (r << 16) + (g << 8) + b; }
  Color operator*(double f) { return Color(r * f, g * f, b * f); }
};

#define RED Color(255, 0, 0)
#define GREEN Color(0, 255, 0)
#define BLUE Color(0, 0, 255)
#define WHITE Color(255, 255, 255)
#define BLACK Color(0, 0, 0)

class Camera {
public:
  Camera() : x(1), y(1), dir(1.2), fov(75) {}
  float x, y;
  float dir;
  float fov;
};

enum { NORTH, EAST, SOUTH, WEST };

std::vector<Color> render(Camera camera, int width, int height, int map[10][10],
                          Color color) {
  std::vector<Color> screen;
  // Clear the screen
  for (int i = 0; i < width * height; i++) {
    screen.push_back(Color(0, 0, 0));
  }
  // Shoot a ray for each column
  for (int x = 0; x < width; x++) {
    // Calculate the ray Direction
    float fov = camera.fov * M_PI / 180;
    float rayDir = camera.dir - fov / 2 + fov * x / width;
    // Calculat delta x and delta y
    float dx = cos(rayDir);
    float dy = sin(rayDir);
    // Calculate closest object
    float dist = 0;
    int side = 0;
    float lastX = camera.x;
    float lastY = camera.y;
    while (true) {
      dist += 0.01;
      float x = camera.x + dist * dx;
      float y = camera.y + dist * dy;
      if (map[int(y)][int(x)] == 1) {
        if (int(lastX) != int(x)) {
          if (lastX - int(lastX) > 0.5) {
            side = EAST;
          } else {
            side = WEST;
          }
        } else if (int(lastY) != int(y)) {
          if (lastY - int(lastY) > 0.5) {
            side = NORTH;
          } else {
            side = SOUTH;
          }
        }
        break;
      }
      lastX = x;
      lastY = y;
    }
    // Calculate height of the line
    int lineHeight = height / dist;
    // Calculate the start and end of the line
    int lineStart = -lineHeight / 2 + height / 2;
    int lineEnd = lineHeight / 2 + height / 2;
    // Render the two sides of the wall with different colors
    // Draw the line
    for (int y = 0; y < height; y++) {
      if (y < lineStart || y > lineEnd) {
        screen[y * width + x] = Color(0, 0, 0);
      } else {
          if (side == NORTH || side == EAST) {
              screen[y * width + x] = color;
          } else {
              screen[y * width + x] = color * 0.5;
          }
      }
    }
  }
  return screen;
}

int main() {
  int width = 640;
  int height = 480;
  int map[10][10] = {
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
      {1, 0, 0, 1, 0, 0, 0, 0, 0, 1}, {1, 0, 0, 1, 1, 1, 1, 0, 1, 1},
      {1, 0, 0, 1, 1, 1, 1, 0, 1, 1}, {1, 0, 0, 1, 0, 0, 0, 0, 1, 1},
      {1, 0, 0, 1, 1, 1, 1, 0, 1, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
  Camera camera;
  camera.x = 1;
  camera.y = 1;
  camera.dir = 0;
  Color color = RED;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, width, height,
                                        SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
      break;
    }
    if (SDL_PollEvent(&event) && event.type == SDL_KEYDOWN) {
      if (event.key.keysym.sym == SDLK_LEFT) {
        camera.dir -= 0.1;
      }
      if (event.key.keysym.sym == SDLK_RIGHT) {
        camera.dir += 0.1;
      }
      if (event.key.keysym.sym == SDLK_UP) {
        camera.y += 0.1 * sin(camera.dir);
        camera.x += 0.1 * cos(camera.dir);
      }
      if (event.key.keysym.sym == SDLK_DOWN) {
        camera.y -= 0.1 * sin(camera.dir);
        camera.x -= 0.1 * cos(camera.dir);
      }
    }
    std::vector<Color> screen = render(camera, width, height, map, color);
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        Color pixel = screen[j * width + i];
        SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, 255);
        SDL_RenderDrawPoint(renderer, i, j);
      }
    }
    SDL_RenderPresent(renderer);
  }
}
