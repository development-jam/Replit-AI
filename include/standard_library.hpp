#pragma once
#include "replit_core.hpp"
#include "graphics.hpp"
#include <fstream>
#include <filesystem>
#include <random>
#include <chrono>
#include <algorithm>
#include <thread>
#include <future>

namespace replit {

// File I/O operations
class FileSystem {
public:
    static std::string read_file(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + path);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return content;
    }
    
    static void write_file(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot write to file: " + path);
        }
        file << content;
    }
    
    static void append_file(const std::string& path, const std::string& content) {
        std::ofstream file(path, std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot append to file: " + path);
        }
        file << content;
    }
    
    static bool file_exists(const std::string& path) {
        return std::filesystem::exists(path);
    }
    
    static bool create_directory(const std::string& path) {
        return std::filesystem::create_directories(path);
    }
    
    static std::vector<std::string> list_directory(const std::string& path) {
        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            files.push_back(entry.path().filename().string());
        }
        return files;
    }
    
    static void delete_file(const std::string& path) {
        std::filesystem::remove(path);
    }
    
    static void copy_file(const std::string& from, const std::string& to) {
        std::filesystem::copy_file(from, to);
    }
    
    static size_t file_size(const std::string& path) {
        return std::filesystem::file_size(path);
    }
};

// Math utilities
class Math {
public:
    static constexpr float PI = 3.14159265359f;
    static constexpr float E = 2.71828182846f;
    
    static float sin(float x) { return std::sin(x); }
    static float cos(float x) { return std::cos(x); }
    static float tan(float x) { return std::tan(x); }
    static float asin(float x) { return std::asin(x); }
    static float acos(float x) { return std::acos(x); }
    static float atan(float x) { return std::atan(x); }
    static float atan2(float y, float x) { return std::atan2(y, x); }
    
    static float sqrt(float x) { return std::sqrt(x); }
    static float pow(float base, float exp) { return std::pow(base, exp); }
    static float log(float x) { return std::log(x); }
    static float log10(float x) { return std::log10(x); }
    static float exp(float x) { return std::exp(x); }
    
    static float abs(float x) { return std::abs(x); }
    static float floor(float x) { return std::floor(x); }
    static float ceil(float x) { return std::ceil(x); }
    static float round(float x) { return std::round(x); }
    
    static float min(float a, float b) { return std::min(a, b); }
    static float max(float a, float b) { return std::max(a, b); }
    static float clamp(float value, float min_val, float max_val) {
        return std::max(min_val, std::min(value, max_val));
    }
    
    static float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }
    
    static float degrees_to_radians(float degrees) {
        return degrees * PI / 180.0f;
    }
    
    static float radians_to_degrees(float radians) {
        return radians * 180.0f / PI;
    }
    
    static int random_int(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }
    
    static float random_float(float min = 0.0f, float max = 1.0f) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);
        return static_cast<float>(dis(gen));
    }
};

// String utilities
class StringUtils {
public:
    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
        if (strings.empty()) return "";
        
        std::string result = strings[0];
        for (size_t i = 1; i < strings.size(); ++i) {
            result += delimiter + strings[i];
        }
        return result;
    }
    
    static std::string to_upper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    static std::string to_lower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r\f\v");
        if (start == std::string::npos) return "";
        
        size_t end = str.find_last_not_of(" \t\n\r\f\v");
        return str.substr(start, end - start + 1);
    }
    
    static bool starts_with(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && 
               str.compare(0, prefix.size(), prefix) == 0;
    }
    
    static bool ends_with(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && 
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    static std::string replace_all(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }
    
    static bool contains(const std::string& str, const std::string& substr) {
        return str.find(substr) != std::string::npos;
    }
};

// Array utilities
template<typename T>
class Array {
private:
    std::vector<T> data;
    
public:
    Array() = default;
    Array(size_t size) : data(size) {}
    Array(std::initializer_list<T> init) : data(init) {}
    
    T& operator[](size_t index) { return data[index]; }
    const T& operator[](size_t index) const { return data[index]; }
    
    void push(const T& item) { data.push_back(item); }
    void pop() { if (!data.empty()) data.pop_back(); }
    
    T& front() { return data.front(); }
    T& back() { return data.back(); }
    
    size_t size() const { return data.size(); }
    bool empty() const { return data.empty(); }
    
    void clear() { data.clear(); }
    void resize(size_t new_size) { data.resize(new_size); }
    
    void insert(size_t index, const T& item) {
        data.insert(data.begin() + index, item);
    }
    
    void remove(size_t index) {
        if (index < data.size()) {
            data.erase(data.begin() + index);
        }
    }
    
    int find(const T& item) const {
        auto it = std::find(data.begin(), data.end(), item);
        return it != data.end() ? static_cast<int>(it - data.begin()) : -1;
    }
    
    void sort() {
        std::sort(data.begin(), data.end());
    }
    
    void reverse() {
        std::reverse(data.begin(), data.end());
    }
    
    Array<T> slice(size_t start, size_t end) const {
        Array<T> result;
        for (size_t i = start; i < end && i < data.size(); ++i) {
            result.push(data[i]);
        }
        return result;
    }
    
    Array<T> filter(std::function<bool(const T&)> predicate) const {
        Array<T> result;
        for (const auto& item : data) {
            if (predicate(item)) {
                result.push(item);
            }
        }
        return result;
    }
    
    template<typename U>
    Array<U> map(std::function<U(const T&)> transform) const {
        Array<U> result;
        for (const auto& item : data) {
            result.push(transform(item));
        }
        return result;
    }
};

// Time utilities
class Time {
public:
    static double get_time() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration<double>(duration).count();
    }
    
    static void sleep(double seconds) {
        std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    }
    
    static std::string format_time(const std::string& format = "%Y-%m-%d %H:%M:%S") {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), format.c_str());
        return ss.str();
    }
};

// Threading utilities
class Threading {
public:
    template<typename F, typename... Args>
    static std::future<typename std::result_of<F(Args...)>::type>
    run_async(F&& f, Args&&... args) {
        return std::async(std::launch::async, std::forward<F>(f), std::forward<Args>(args)...);
    }
    
    static void parallel_for(size_t start, size_t end, std::function<void(size_t)> func) {
        size_t num_threads = std::thread::hardware_concurrency();
        size_t chunk_size = (end - start) / num_threads;
        
        std::vector<std::thread> threads;
        
        for (size_t i = 0; i < num_threads; ++i) {
            size_t chunk_start = start + i * chunk_size;
            size_t chunk_end = (i == num_threads - 1) ? end : chunk_start + chunk_size;
            
            threads.emplace_back([=]() {
                for (size_t j = chunk_start; j < chunk_end; ++j) {
                    func(j);
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
    static int hardware_concurrency() {
        return std::thread::hardware_concurrency();
    }
};

}