#ifndef _UI_HPP_
#define _UI_HPP_

#include "../common/font_engine.hpp"
#include <glm.hpp>
#include <string>
#include <vector>

class UI {

public:

    UI(const glm::vec2 pResolution)
        : screen_resolution(pResolution)
        , font_engine(pResolution)
    {}

    void set_line_interval(const glm::vec2 pLineInterval);
    void set_value_interval(const glm::vec2 value_interval);
    void set_xlabel(char* xlabel);
    void set_ylabel(char* ylabel);

    void construct_elements();
    void render();

private:

    glm::vec2 line_interval;
    glm::vec2 value_interval;
    glm::vec2 screen_resolution;

    char* xlabel;
    char* ylabel;

private:

    struct RenderDatum {
        unsigned int stringID;
        glm::vec2 offset;
    };

    FontEngine font_engine;
    std::vector<RenderDatum> render_data;
};

#endif