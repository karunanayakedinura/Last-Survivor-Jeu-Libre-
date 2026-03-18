#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>
#include <cmath>
#include <sstream>

class tqdm {
public:
    /**
     * @brief Construct a new tqdm object.
     * @param total The total number of iterations.
     * @param description A description to display before the progress bar.
     * @param bar_width The width of the progress bar in characters.
     */
    tqdm(int total, const std::string& description = "", int bar_width = 40)
        : total_iterations(total),
          desc(description),
          bar_width(bar_width),
          current_iteration(0),
          start_time(std::chrono::steady_clock::now()) {
        print_progress(0);
    }

    /**
     * @brief Update the progress bar by one step.
     */
    void tick() {
        current_iteration++;
        print_progress(current_iteration);
    }

    /**
     * @brief Update the progress bar by a specified number of steps.
     * @param steps The number of steps to advance.
     */
    void update(int steps = 1) {
        current_iteration += steps;
        print_progress(current_iteration);
    }

    /**
     * @brief Set the description text.
     * @param description The new description.
     */
    void set_description(const std::string& description) {
        desc = description;
        print_progress(current_iteration);
    }

    /**
     * @brief Close the progress bar, ensuring it shows 100% and moves to the next line.
     * Should be called when the loop is finished.
     */
    void close() {
        // If the bar is not already at 100%, print the final state.
        // This prevents a redundant final print if the loop finished perfectly.
        if (current_iteration < total_iterations) {
            print_progress(total_iterations);
        }
        // Always move to the next line at the end.
        std::cerr << "\r" << "100% |";
    }

private:
    int total_iterations;
    std::string desc;
    int bar_width;
    int current_iteration;
    std::chrono::steady_clock::time_point start_time;

    /**
     * @brief Prints the progress bar to the console.
     * @param current The current iteration number.
     */
    void print_progress(int current) {
        // To prevent division by zero.
        if (total_iterations == 0) return;

        float progress = static_cast<float>(current) / total_iterations;
        // Clamp progress to 1.0 in case current > total
        if (progress > 1.0f) {
            progress = 1.0f;
        }

        int pos = static_cast<int>(bar_width * progress);

        auto now = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
        double elapsed_s = elapsed_ms / 1000.0;

        double iter_per_sec = (elapsed_s > 0) ? static_cast<double>(current) / elapsed_s : 0.0;
        double remaining_time = (iter_per_sec > 0) ? (total_iterations - current) / iter_per_sec : 0.0;

        std::stringstream ss;
        ss << "\r"; // Carriage return to overwrite the line

        // Description
        if (!desc.empty()) {
            ss << desc << ": ";
        }

        // Percentage
        ss << std::fixed << std::setprecision(0) << (progress * 100.0) << "% |";
        
        // Progress bar
        for (int i = 0; i < bar_width; ++i) {
            if (i < pos) ss << "#";
            else ss << " ";
        }
        ss << "| ";

        // Iteration count
        ss << current << "/" << total_iterations << " [";

        // Timings
        ss << format_time(static_cast<long>(elapsed_s)) << "<" << format_time(static_cast<long>(remaining_time));
        ss << ", " << std::fixed << std::setprecision(2) << iter_per_sec << "it/s]";

        // Print to std::cerr to avoid interfering with stdout
        std::cerr << ss.str() << std::flush;
    }

    /**
     * @brief Formats seconds into a HH:MM:SS string.
     * @param seconds The total seconds to format.
     * @return A string in HH:MM:SS or MM:SS format.
     */
    std::string format_time(long seconds) {
        std::stringstream ss;
        if (seconds >= 3600) {
            long h = seconds / 3600;
            long m = (seconds % 3600) / 60;
            long s = seconds % 60;
            ss << std::setfill('0') << std::setw(2) << h << ":"
               << std::setfill('0') << std::setw(2) << m << ":"
               << std::setfill('0') << std::setw(2) << s;
        } else {
            long m = seconds / 60;
            long s = seconds % 60;
            ss << std::setfill('0') << std::setw(2) << m << ":"
               << std::setfill('0') << std::setw(2) << s;
        }
        return ss.str();
    }
};
