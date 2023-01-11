#include "ui.hpp"

void UI::set_line_interval(const glm::vec2 pLineInterval) {
    line_interval = pLineInterval;
}

void UI::set_value_interval(const glm::vec2 pValueInterval) {
    value_interval = pValueInterval;
}

void UI::set_xlabel(char* xlabel) {
    this->xlabel = xlabel;
}

void UI::set_ylabel(char* ylabel) {
    this->ylabel = ylabel;
}