#include "rack.hpp"


using namespace rack;


extern Plugin *plugin;

////////////////////
// module widgets
////////////////////

struct MyModuleWidget : ModuleWidget {
    int num_blue_lights;
    int num_green_lights;

	MyModuleWidget();
    void drawLights();
};
