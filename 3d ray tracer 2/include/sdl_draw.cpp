#include <SDL2/SDL.h>
#include <vector>

using namespace std;

struct Color{
    int r, g, b, a;

    Color(int r=255, int g=255, int b=255, int a=255) : r(r), g(g), b(b), a(a) {}

    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b);
    }
    
    Color operator-(const Color& other) const {
        return Color(r - other.r, g - other.g, b - other.b);
    }
    
    Color operator*(const double& other) const {
        return Color(r * other, g * other, b * other);
    }
    
    Color operator/(const double& other) const {
        return Color(r / other, g / other, b / other);
    }

    Uint32 to_hex(){
        return (r << 24) | (g << 16) | (b << 8) | a;
    }
};

void clear_window(SDL_Renderer* renderer, Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void set_color(SDL_Renderer* renderer, Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void draw_pixel(SDL_Renderer* renderer, int x, int y, Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, x, y);
}

void draw_rect_filled(SDL_Renderer* renderer, vector<int> window_size, int x, int y, int width, int height, Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect;
    rect.x = x + window_size[0] - width / 2;
    rect.y = -y + window_size[1] - height / 2;
    rect.w = width;
    rect.h = height;
    SDL_RenderFillRect(renderer, &rect);
}

void draw_line(SDL_Renderer* renderer, vector<int> window_size, int x1, int y1, int x2, int y2, Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, x1 + window_size[0], -y1 + window_size[1], x2 + window_size[0], -y2 + window_size[1]);
}

void render(SDL_Renderer* renderer){
    SDL_RenderPresent(renderer);
}
