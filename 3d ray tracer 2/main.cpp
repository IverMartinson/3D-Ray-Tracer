#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include "include/SDL_func.cpp"
#include "include/time.cpp"
#include "include/geometry.h"
#include "include/math.cpp"
#include "include/sdl_draw.cpp"
#include <random>
#include <math.h>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <thread>
#include "include/general.cpp"

using namespace std;

int width;
int height;
int width_half;
int height_half;

enum Material_Flags{
    is_lit,
    is_unlit,
    is_showing_uv,
    is_light,
};

struct Material{
    Color color;
    Material_Flags flag;
    double refractive_index;
    double diffuse_albedo;
    double specular_albedo;
    double refractive_albedo;
    double reflective_albedo;
    double specular_exponent;

    Material(Color color, Material_Flags flag, double refractive_index=0, double diffuse_albedo=0, double specular_albedo=0, double reflective_albedo=0, double refractive_albedo=0, double specular_exponent=0) : color(color), flag(flag), refractive_index(refractive_index), diffuse_albedo(diffuse_albedo), specular_albedo(specular_albedo), reflective_albedo(reflective_albedo), refractive_albedo(refractive_albedo), specular_exponent(specular_exponent) {}
};

struct Triangle{
    vector3 vertex_1;
    vector3 vertex_2;
    vector3 vertex_3;
    vector3 normal;

    Triangle(vector3 vertex_1, vector3 vertex_2, vector3 vertex_3) : vertex_1(vertex_1), vertex_2(vertex_2), vertex_3(vertex_3) {};
};

vector3 rotate(vector3, vector3);

struct Object{
    vector3 position;
    vector3 rotation;
    vector<Triangle> triangles;
    vector<Triangle> original_triangles;
    Material material;
    vector3 scale;

    Object(vector3 position_, vector3 rotation_, vector3 scale, Material material, vector<Triangle> triangles) : scale(scale), material(material), original_triangles(triangles), triangles(triangles) {
        position = position_;
        rotation = rotation_;
    }

    void update(){
        for (int i = 0; i < triangles.size(); ++i){
            Triangle& triangle = triangles[i];
            Triangle& original_triangle = original_triangles[i];
            triangle.vertex_3 = rotate(original_triangle.vertex_3 * scale, rotation) + position;
            triangle.vertex_2 = rotate(original_triangle.vertex_2 * scale, rotation) + position;
            triangle.vertex_1 = rotate(original_triangle.vertex_1 * scale, rotation) + position;
            
            vector3 direction = cross_multiply(triangle.vertex_2 - triangle.vertex_1, triangle.vertex_3 - triangle.vertex_1);
            triangle.normal = direction / direction.magnitude();
        }
    }
};  

struct Hit{
    Object* object;
    vector3 normal;
    vector3 result;
    vector3 position;

    Hit(Object* object=nullptr, vector3 normal=vector3(), vector3 position=vector3(), vector3 result=vector3(INFINITY, 0, 0)) : object(object), normal(normal), position(position), result(result) {}
};

struct Ray{
    vector<Hit> hits;
    vector3 position;
    vector3 origin;
    vector3 direction;
    int x;
    int y;
    int reflection;
    double distance = 0;

    Ray(vector3 origin, vector3 direction, int x, int y, int reflection=0) : origin(origin), direction(direction), x(x), y(y), reflection(reflection) {}
};

struct Light{
    vector3 position;
    Color color;
    double intensity;
    Object* object;

    Light(vector3 position, Color color, double intensity, Object* object) : position(position), color(color), intensity(intensity), object(object) {}

    void update(){
        object->position = position;
        object->update();
    }
};

struct Camera{
    vector3 position;
    vector3 rotation;
    double resolution;
    double fov;
    double min_clip = 0;
    vector<Ray> rays;
    int max_reflections;

    void generate_rays(int width_half, int height_half) {
        rays.clear();

        float aspect_ratio = static_cast<float>(width_half * 2) / static_cast<float>(height_half * 2);
        float x_increment = tan(fov / 2) / width_half;
        float y_increment = tan(fov / 2) / height_half / aspect_ratio;
        
        for (int i = -width_half; i < width_half; i += resolution) {
            for (int j = -height_half; j < height_half; j += resolution) {
                rays.push_back(Ray(position, vector3(x_increment * i, y_increment * j, 1).normalize(), i + width_half, j + height_half));
            }
        }
    }

    Camera(vector3 position, vector3 rotation, double resolution, double fov, int max_reflections) : position(position), rotation(rotation), resolution(resolution), fov(fov), max_reflections(max_reflections) {}
};

Camera camera(vector3(0, 0, -1410), vector3(0, 0, 0), 1, 1, 9);
vector<Object> scene;
vector<Light> lights;

vector3 rotate_y(double angle, vector3 vector) {
    Matrix matrix = {
        {cos(angle), 0, sin(angle)},
        {0, 1, 0},
        {-sin(angle), 0, cos(angle)}
    };
    return matrix_multiply(vector, matrix);
}

vector3 rotate_x(double angle, vector3 vector) {
    Matrix matrix = {
        {1, 0, 0},
        {0, cos(angle), -sin(angle)},
        {0, sin(angle), cos(angle)}
    };
    return matrix_multiply(vector, matrix);
}

vector3 rotate_z(double angle, vector3 vector) {
    Matrix matrix = {
        {cos(angle), -sin(angle), 0},
        {sin(angle), cos(angle), 0},
        {0, 0, 1}
    };
    return matrix_multiply(vector, matrix);
}

vector3 rotate(vector3 vector, vector3 angle){
    vector = rotate_x(angle.x, vector);
    vector = rotate_y(angle.y, vector);
    vector = rotate_z(angle.z, vector);
    return vector;
}

void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;
    destRect.w = surface->w;
    destRect.h = surface->h;

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
}

vector<string> split_string(string str, char delimiter){
    vector<string> parts;
    string part;
    stringstream ss(str);

    while(getline(ss, part, delimiter)){
        parts.push_back(part);
    }

    return parts;
}

vector<Triangle> read_object_file(string file_name){
    ifstream object_file(file_name);
    string line;

    vector<vector3> points;
    vector<Triangle> triangles;

    if (object_file.is_open()){
        while(object_file.good()){
            getline(object_file, line);
            
            if (line[0] == 'v' && line[1] == ' '){
                istringstream iss(line);
                string foo;
                double x, y, z;

                iss >> foo >> x >> y >> z;

                points.push_back(vector3(x, y, z));
            } else if (line[0] == 'f'){
                line.erase(0, 2);
                if (line.find('/') != string::npos) {
                    vector<vector3> vectors;

                    for (string point : split_string(line, ' ')) {
                        vectors.push_back(points[stoi(split_string(point, '/')[0]) - 1]);
                    }

                    if (vectors.size() >= 3) {
                        triangles.push_back(Triangle(vectors[0], vectors[1], vectors[2]));
                    }
                }
            }
        }
    }

    return triangles;
}

void import_object(vector3 position, vector3 rotation, vector3 scale, string file_name, Material material){
    Object object(position, rotation, scale, material, read_object_file(file_name));

    object.update();

    scene.push_back(object);
}

void create_light(vector3 position, vector3 scale, Color color, double intensity){
    Object object(position, vector3(), scale, Material(color, is_light), read_object_file("cube.obj"));

    scene.push_back(object);
    
    Light light(position, color, intensity, &scene.back());

    light.update();

    lights.push_back(light);
}

vector3 reflection(vector3 incident, vector3 normal){
    return incident - 2 * normal * dot_product(incident, normal);
}

vector3 refraction(vector3 light_angle, vector3 normal, double refractive_index){
    double cosi = max(-1.0, min(1.0, dot_product(light_angle, normal)));
    double etai = 1;
    double etat = refractive_index;
    
    if (cosi < 0){
        cosi *= -1;
        swap(etai, etat);
        normal = normal * -1;
    }

    double eta = etai / etat;
    double k = 1 - pow(eta, 2) * (1 - pow(cosi, 2));
    return k < 0 ? vector3(0, 0, 0) : light_angle * eta + normal * (eta * cosi - sqrt(k));
}

Hit is_intersecting(Ray& ray){
    Hit closest_hit;

    for (Object& object : scene) {
        for(Triangle triangle : object.triangles){
            vector3 E1 = triangle.vertex_2 - triangle.vertex_1;
            vector3 E2 = triangle.vertex_3 - triangle.vertex_1;
            vector3 T = ray.origin - triangle.vertex_1;
            vector3 P = cross_multiply(ray.direction, E2);
            vector3 Q = cross_multiply(T, E1);

            vector3 result = 1 / dot_product(P, E1) * vector3(dot_product(Q, E2), dot_product(P, T), dot_product(Q, ray.direction));
        
            double t = result.x;
            double u = result.y;
            double v = result.z;

            if (t > camera.min_clip && t < closest_hit.result.x && u>=0 && u<=1 && v>=0 && v<=1 && u+v>=0 && u+v<=1){
                closest_hit = Hit(&object, triangle.normal, ray.direction * t + ray.origin, result);           
            }
        }
    }

    return closest_hit;
}

Color simple_cast(Ray ray){ 
    if (ray.reflection > 5) return Color(0, 0, 20);

    Hit hit = is_intersecting(ray);

    if (hit.result.x == INFINITY) return Color(0, 0, 20);

    ray.distance += hit.result.x;

    switch (hit.object->material.flag) {
        case is_lit: {
            Material& material = hit.object->material;

            double diffuse_light_intensity = 0;
            double specular_light_intensity = 0;

            for(Light& light : lights){
                vector3 light_direction = (light.position - hit.position).normalize();
                
                Ray shadow_ray = dot_product(light_direction, hit.normal) < 0 ? Ray(hit.position - hit.normal * 1e-3, light_direction, 0, 0) : Ray(hit.position + hit.normal * 1e-3, light_direction, 0, 0);
                Hit shadow_hit = is_intersecting(shadow_ray);

                if (shadow_hit.result.x == INFINITY || (shadow_hit.object != nullptr && shadow_hit.object->material.flag == is_light)) {
                    diffuse_light_intensity += light.intensity * (1 / pow(ray.distance, 0.5)) * max(0.0, dot_product(light_direction, hit.normal));
                    specular_light_intensity += light.intensity * (1 / pow(ray.distance, 0.5)) * pow(max(0.0, dot_product(-1 * reflection(-1 * light_direction, hit.normal), ray.direction)), material.specular_exponent);                    
                }
            }

            Color reflected_color = Color(0, 0, 0);
            if (material.reflective_albedo > 0){
                vector3 reflection_direction = reflection(ray.direction, hit.normal).normalize();
                Ray reflected_ray(hit.position + hit.normal*(dot_product(reflection_direction, hit.normal) < 0 ? -0.0000001 : 0.0000001), reflection_direction, 0, 0, ray.reflection + 1);
                reflected_ray.distance += ray.distance;
                reflected_color = simple_cast(reflected_ray);
            }

            Color refracted_color = Color(0, 0, 0);
            if (material.refractive_albedo > 0){
                vector3 refraction_direction = refraction(ray.direction, hit.normal, material.refractive_index).normalize();
                Ray refracted_ray(hit.position + hit.normal * dot_product(refraction_direction, hit.normal), refraction_direction, 0, 0, ray.reflection + 1);
                refracted_ray.distance += ray.distance;
                refracted_color = simple_cast(refracted_ray);
            }
                
            Color final_color = material.color * diffuse_light_intensity * material.diffuse_albedo + Color(255, 255, 255) * specular_light_intensity * material.specular_albedo + reflected_color * material.reflective_albedo + refracted_color * material.refractive_albedo;

            final_color.r = final_color.r > 255 ? 255 : final_color.r;
            final_color.g = final_color.g > 255 ? 255 : final_color.g;
            final_color.b = final_color.b > 255 ? 255 : final_color.b;

            return final_color;
        }
        case is_unlit: {
            return hit.object->material.color;
        }
        case is_light: {
            return hit.object->material.color;
        }
        case is_showing_uv: {
            vector3 result = hit.result;
            double t = result.x;
            double u = result.y;
            double v = result.z;

            return Color(255 * (1-(u+v)), 255 * (1-(u+(1-(u + v)))), 255 * (1-(v+(1-(u + v)))));
        }
        default: {
            return Color(255, 0, 255);
        }
    }
}

void simple_cast_thread(int x_start, int x_end, int width, int height, Uint32* pixels) {
    int ray_count_width = width / camera.resolution;
    
    for (int i = x_start; i < x_end; i += camera.resolution) {
        for (int j = 0; j < height; j += camera.resolution) {
            Ray& ray = camera.rays[ray_count_width * (i / camera.resolution) + (j / camera.resolution)];
            
            Uint32 color = simple_cast(ray).to_hex();

            for (int k = 0; k < camera.resolution; ++k) {
                for (int l = 0; l < camera.resolution; ++l) {
                    int pixel_index = (height - ray.y - k) * width + (ray.x + l);
                    if (pixel_index >= 0 && pixel_index < width * height) {
                        pixels[pixel_index] = color;
                    }
                }
            }
        }
    }
}

int main() { 
    // refractive_index, diffuse_albedo, specular_albedo, reflective_albedo, refractive_albedo, specular_exponent
    Material defualt(Color(222, 222, 214), is_lit, 1, 0.6, 0.3, 0.0, 0.0, 10);
    Material gordon (Color(255, 255, 255), is_lit, 1.6, 0.3, 0.5, 0.2, 0.8, 10);
    Material red    (Color(255, 0, 0),     is_lit, 1, 0.9, 0.1, 0.0, 0.0, 10);
    Material green  (Color(0, 255, 0),     is_lit, 1, 0.9, 0.5, 0.1, 0.0, 100);
    Material mirror (Color(0, 255, 0),     is_lit, 1, 0.0, 1.0, 0.7, 0.0, 2025);

     //import_object(vector3(0, 0, 0), vector3(0, 0, 0), vector3(100, 100, 100), "cube.obj", Material(Color(0, 255, 0), is_lit));
     import_object(vector3(0, -500, 0), vector3(0, 3.14, 0), vector3(400, 400, 400), "gordon_freeman.obj", gordon);
     import_object(vector3(0, -500, 0), vector3(0, 0, 0), vector3(500, 500, 500), "plane.obj", defualt);
     import_object(vector3(0, 0, 500), vector3(1.57, 3.14, 0), vector3(500, 500, 500), "plane.obj", defualt);
     import_object(vector3(0, 500, 0), vector3(0, 0, 0), vector3(500, 1, 500), "cube.obj", defualt);
     import_object(vector3(-500, 0, 0), vector3(0, 0, -1.57), vector3(500, 500, 500), "plane.obj", red);
     import_object(vector3(500, 0, 0), vector3(0, 0, 1.57), vector3(500, 500, 500), "plane.obj", green);
     import_object(vector3(-250, 0, 250), vector3(1.57, -1.57 / 2, 0), vector3(500, 1, 500), "cube.obj", mirror);
    //import_object(vector3(0, -300, 0), vector3(0, 3.14, 0), vector3(300, 200, 200), "cube.obj", defualt);
    //import_object(vector3(0, -500, 0), vector3(0, 0, 0), vector3(500, 500, 500), "plane.obj", defualt);
    //import_object(vector3(0, 0, 500), vector3(1.57, 3.14, 0), vector3(500, 500, 500), "plane.obj", defualt);
    //import_object(vector3(0, 500, 0), vector3(0, 0, 0), vector3(500, 1, 500), "cube.obj", defualt);
    //import_object(vector3(-500, 0, 0), vector3(0, 0, -1.57), vector3(500, 500, 500), "plane.obj", defualt);
    //import_object(vector3(500, 0, 0), vector3(0, 0, 1.57), vector3(500, 500, 500), "plane.obj", defualt);

    create_light(vector3(0, 500, 0), vector3(200, 10, 200), Color(255, 255, 255), 40);

    camera.resolution = 1;

    init();

    TTF_Init();

    TTF_Font* font = TTF_OpenFont("GaMaamli-Regular.ttf", 24);

    char angle_text[32];
    int width = 1000;
    int height = 1000;
    int width_half = width / 2;
    int height_half = height / 2;

    camera.generate_rays(width_half, height_half);

    int lastTick = now();
    double dt = 0;

    SDL_Window* window = create_window("3D Ray Tracer", width, height);
    SDL_Renderer* renderer = create_renderer(window);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    Uint32* pixels = new Uint32[width * height];

    memset(pixels, 255, width * height * sizeof(Uint32));

    double angle = 0;
    int thread_count = 20;

    bool running = true;
    SDL_Event event;
    while (running){
        dt = (now() - lastTick) / 1000000000.0;
        lastTick = now();

        while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_DOWN:{
                        ++camera.resolution;
                        break;
                    }
                    case SDLK_UP:{
                        if(camera.resolution > 1)--camera.resolution;
                        break;
                    }
                }
                break;

            default:
                break;
            }
        }

        double move_speed = 10;

        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        if (currentKeyStates[SDL_SCANCODE_W]) {
            camera.position.y += 1 * move_speed;
        }
        if (currentKeyStates[SDL_SCANCODE_S]) {
            camera.position.y -= 1 * move_speed;
        }
        if (currentKeyStates[SDL_SCANCODE_E]) {
            camera.position.z += 1 * move_speed;
        }
        if (currentKeyStates[SDL_SCANCODE_Q]) {
            camera.position.z -= 1 * move_speed;
        }
        if (currentKeyStates[SDL_SCANCODE_A]) {
            camera.position.x -= 1 * move_speed;
        }
        if (currentKeyStates[SDL_SCANCODE_D]) {
            camera.position.x += 1 * move_speed;
        }

        angle += 0.2;

        scene[0].rotation.y += 0.1;
        scene[0].update();

        // lights[0].position = vector3(cos(angle) * 300, 0, sin(angle) * 300);
        // lights[0].update();

        camera.generate_rays(width_half, height_half);

        int increment = width / thread_count;

        vector<thread> threads;

        for (int i = 0; i < width; i+=increment){
            threads.push_back(thread(simple_cast_thread, i, i + increment, width, height, ref(pixels)));
        }

        for (thread& thread : threads) {
            thread.join();
        }

        void* mPixels;
        int pitch;

        SDL_LockTexture(texture, NULL, &mPixels, &pitch);
        memcpy(mPixels, pixels, width * height * sizeof(Uint32));
        SDL_UnlockTexture(texture);

        SDL_RenderCopy(renderer, texture, NULL, NULL);

        snprintf(angle_text, sizeof(angle_text), "%.4f", 1 / dt);
        render_text(renderer, font, angle_text, 0, 0, {255, 255, 255});

        snprintf(angle_text, sizeof(angle_text), "%.0f", camera.resolution);
        render_text(renderer, font, angle_text, 0, 26, {255, 255, 255});
        render_text(renderer, font, (width % int(camera.resolution) == 0) ? "(factor)" : "(non-factor)", 0, 52, {160, 160, 160});

        SDL_RenderPresent(renderer);
    }

    close(window);

    return 0;
}
