#pragma once
#include "replit_core.hpp"
#include <cmath>

namespace replit {

// Color class for graphics operations
class Color {
public:
    float r, g, b, a;
    
    Color(float red = 0, float green = 0, float blue = 0, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}
    
    static Color RGB(int red, int green, int blue) {
        return Color(red / 255.0f, green / 255.0f, blue / 255.0f);
    }
    
    static Color RGBA(int red, int green, int blue, int alpha) {
        return Color(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f);
    }
    
    static Color WHITE() { return Color(1, 1, 1); }
    static Color BLACK() { return Color(0, 0, 0); }
    static Color RED() { return Color(1, 0, 0); }
    static Color GREEN() { return Color(0, 1, 0); }
    static Color BLUE() { return Color(0, 0, 1); }
    static Color YELLOW() { return Color(1, 1, 0); }
    static Color MAGENTA() { return Color(1, 0, 1); }
    static Color CYAN() { return Color(0, 1, 1); }
};

// 2D Vector for positions, velocities, etc.
class Vector2D {
public:
    float x, y;
    
    Vector2D(float x = 0, float y = 0) : x(x), y(y) {}
    
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    
    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    
    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }
    
    Vector2D normalized() const {
        float mag = magnitude();
        if (mag == 0) return Vector2D(0, 0);
        return Vector2D(x / mag, y / mag);
    }
    
    float distance(const Vector2D& other) const {
        return (*this - other).magnitude();
    }
    
    float dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }
};

// Rectangle for collision detection and rendering
class Rectangle {
public:
    float x, y, width, height;
    
    Rectangle(float x = 0, float y = 0, float w = 0, float h = 0)
        : x(x), y(y), width(w), height(h) {}
    
    bool intersects(const Rectangle& other) const {
        return x < other.x + other.width &&
               x + width > other.x &&
               y < other.y + other.height &&
               y + height > other.y;
    }
    
    bool contains(const Vector2D& point) const {
        return point.x >= x && point.x <= x + width &&
               point.y >= y && point.y <= y + height;
    }
    
    Vector2D center() const {
        return Vector2D(x + width / 2, y + height / 2);
    }
};

// Sprite class for game objects
class Sprite {
public:
    Vector2D position;
    Vector2D velocity;
    Vector2D scale;
    float rotation;
    Color tint;
    Rectangle bounds;
    std::string texture_path;
    bool visible;
    int layer;
    
    Sprite(const std::string& texture = "", float x = 0, float y = 0)
        : position(x, y), velocity(0, 0), scale(1, 1), rotation(0),
          tint(Color::WHITE()), texture_path(texture), visible(true), layer(0) {
        update_bounds();
    }
    
    void update(float delta_time) {
        position = position + velocity * delta_time;
        update_bounds();
    }
    
    void update_bounds() {
        bounds = Rectangle(position.x, position.y, 32 * scale.x, 32 * scale.y);
    }
    
    bool collides_with(const Sprite& other) const {
        return bounds.intersects(other.bounds);
    }
    
    void move_to(float x, float y) {
        position = Vector2D(x, y);
        update_bounds();
    }
    
    void move_by(float dx, float dy) {
        position = position + Vector2D(dx, dy);
        update_bounds();
    }
};

// Input handling system
class Input {
public:
    enum class KeyState { UP, DOWN, PRESSED, RELEASED };
    enum class MouseButton { LEFT, RIGHT, MIDDLE };
    
    static std::unordered_map<std::string, KeyState> keys;
    static Vector2D mouse_position;
    static std::unordered_map<MouseButton, KeyState> mouse_buttons;
    
    static bool is_key_down(const std::string& key) {
        return keys[key] == KeyState::DOWN || keys[key] == KeyState::PRESSED;
    }
    
    static bool is_key_pressed(const std::string& key) {
        return keys[key] == KeyState::PRESSED;
    }
    
    static bool is_mouse_down(MouseButton button) {
        return mouse_buttons[button] == KeyState::DOWN || 
               mouse_buttons[button] == KeyState::PRESSED;
    }
    
    static Vector2D get_mouse_position() {
        return mouse_position;
    }
};

// Window class for graphics rendering
class Window {
public:
    std::string title;
    int width, height;
    Color background_color;
    bool is_open;
    std::vector<std::shared_ptr<Sprite>> sprites;
    
    Window(const std::string& title = "Replit Game", int w = 800, int h = 600)
        : title(title), width(w), height(h), background_color(Color::BLACK()), is_open(true) {}
    
    void clear() {
        // Clear the screen with background color
    }
    
    void render() {
        clear();
        // Sort sprites by layer
        std::sort(sprites.begin(), sprites.end(), 
                 [](const auto& a, const auto& b) { return a->layer < b->layer; });
        
        // Render all sprites
        for (auto& sprite : sprites) {
            if (sprite->visible) {
                draw_sprite(*sprite);
            }
        }
        
        present();
    }
    
    void draw_sprite(const Sprite& sprite) {
        // Draw sprite at position with rotation, scale, and tint
    }
    
    void draw_rectangle(const Rectangle& rect, const Color& color) {
        // Draw filled rectangle
    }
    
    void draw_circle(const Vector2D& center, float radius, const Color& color) {
        // Draw filled circle
    }
    
    void draw_line(const Vector2D& start, const Vector2D& end, const Color& color) {
        // Draw line
    }
    
    void draw_text(const std::string& text, const Vector2D& position, 
                   const Color& color, int font_size = 12) {
        // Draw text
    }
    
    void present() {
        // Present the rendered frame
    }
    
    void close() {
        is_open = false;
    }
    
    void add_sprite(std::shared_ptr<Sprite> sprite) {
        sprites.push_back(sprite);
    }
    
    void remove_sprite(std::shared_ptr<Sprite> sprite) {
        sprites.erase(std::remove(sprites.begin(), sprites.end(), sprite), sprites.end());
    }
    
    bool should_close() const {
        return !is_open;
    }
    
    void poll_events() {
        // Handle window events, input, etc.
    }
    
    void set_vsync(bool enabled) {
        // Enable/disable vertical sync
    }
    
    void set_fullscreen(bool fullscreen) {
        // Toggle fullscreen mode
    }
};

// Audio system
class AudioSystem {
public:
    static void play_sound(const std::string& path, float volume = 1.0f) {
        // Play sound effect
    }
    
    static void play_music(const std::string& path, bool loop = true, float volume = 1.0f) {
        // Play background music
    }
    
    static void stop_music() {
        // Stop background music
    }
    
    static void set_master_volume(float volume) {
        // Set master volume
    }
};

// Timer and animation system
class Timer {
public:
    float duration;
    float elapsed;
    bool running;
    std::function<void()> callback;
    
    Timer(float duration, std::function<void()> callback = nullptr)
        : duration(duration), elapsed(0), running(false), callback(callback) {}
    
    void start() {
        running = true;
        elapsed = 0;
    }
    
    void stop() {
        running = false;
    }
    
    void update(float delta_time) {
        if (!running) return;
        
        elapsed += delta_time;
        if (elapsed >= duration) {
            running = false;
            if (callback) callback();
        }
    }
    
    float progress() const {
        return elapsed / duration;
    }
    
    bool is_finished() const {
        return elapsed >= duration;
    }
};

// Physics utilities
class Physics {
public:
    static bool check_collision_circle_circle(
        const Vector2D& pos1, float radius1,
        const Vector2D& pos2, float radius2) {
        return pos1.distance(pos2) < (radius1 + radius2);
    }
    
    static bool check_collision_rect_rect(
        const Rectangle& rect1, const Rectangle& rect2) {
        return rect1.intersects(rect2);
    }
    
    static Vector2D reflect_velocity(const Vector2D& velocity, const Vector2D& normal) {
        return velocity - normal * (2 * velocity.dot(normal));
    }
    
    static Vector2D apply_gravity(const Vector2D& velocity, float gravity, float delta_time) {
        return Vector2D(velocity.x, velocity.y + gravity * delta_time);
    }
};

}