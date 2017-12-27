#include "Fredmin.hpp"


struct MyModule : Module {
	enum ParamIds {
		TEMPO_PARAM,
        PITCH_PARAM,
        REST_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		CLOCK_INPUT,
        PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		CV_OUTPUT,
        GATE_OUTPUT,
		NUM_OUTPUTS
	};

    struct CLight {
        float phase;
        float freq;
        int id;
    };

	float phase, sampleLength, period;
    CLight chalf, c1, c2, c4,c8, c3, c6;
    uint32_t shiftreg;
    unsigned interval:4;
    unsigned theme:4;

	MyModule();
	void step() override;
    void blinkCLights(CLight);

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


MyModule::MyModule() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, 41) {
    chalf.id = 2;
    c1.id = 3;
    c2.id = 4;
    c4.id = 5;
    c8.id = 6;
    c3.id = 7;
    c6.id = 8;
    chalf.phase = 0.5;
    c1.freq = 1.0;
    c2.freq = 2.0;
    c4.freq = 4.0;
    c8.freq = 8.0;
    c3.freq = 3.0;
    c6.freq = 6.0;
}

void MyModule::step() {
    
	sampleLength = 1.0 / engineGetSampleRate();

    period = 1.0 / params[TEMPO_PARAM].value;;
    blinkCLights(chalf);
    blinkCLights(c1);
    blinkCLights(c2);
    blinkCLights(c4);
    blinkCLights(c8);
    blinkCLights(c3);
    blinkCLights(c6);
	// Compute the frequency from the pitch parameter and input
//	float pitch = params[PITCH_PARAM].value;
//	pitch += inputs[PITCH_INPUT].value;
//	pitch = clampf(pitch, -4.0, 4.0);
//	float freq = 440.0 * powf(2.0, pitch);
//
	// Accumulate the phase
//    phase += freq * deltaTime;
//   	if (phase >= 1.0)
//   		phase -= 1.0;
//
//	// Compute the sine output
//	float sine = sinf(2 * M_PI * phase);
//	outputs[SINE_OUTPUT].value = 5.0 * sine;
//
//	// Blink light at 1Hz
//	blinkPhase += deltaTime;
//	if (blinkPhase >= 1.0)
//		blinkPhase -= 1.0;
//	lights[BLINK_LIGHT].value = (blinkPhase < 0.5) ? 1.0 : 0.0;
}

void MyModule::blinkCLights(CLight cLight) {
    cLight.phase += sampleLength;

    if (cLight.phase >= cLight.freq * period)
        cLight.phase -= 2 * cLight.freq * period;
//    lights[cLight.id].value = (cLight.phase < 0) ? 1.0 : 0.0;
    lights[cLight.id].value = 1.0;
}

MyModuleWidget::MyModuleWidget() : num_blue_lights (9), num_green_lights (31) {
	MyModule *module = new MyModule();
	setModule(module);
	box.size = Vec(45 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/MyModule.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    int firstKnobX = 28;
    int knobHeight = 48;
	addParam(createParam<Davies1900hBlackKnob>(Vec(firstKnobX, knobHeight), module, MyModule::PITCH_PARAM, -3.0, 3.0, 0.0));
	addParam(createParam<Davies1900hBlackKnob>(Vec(firstKnobX + 3 * RACK_GRID_WIDTH, knobHeight), module, MyModule::TEMPO_PARAM, 0.0, 7.0, 0.0));
    drawLights();

	addInput(createInput<PJ301MPort>(Vec(33, 186), module, MyModule::PITCH_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(33, 275), module, MyModule::GATE_OUTPUT));

}

void MyModuleWidget::drawLights() {
    int x = 90;
    int y = 110;
    int spacing = 12;
    for (int i = 0; i < num_blue_lights; ++i) {
	    addChild(createLight<MediumLight<BlueLight>>(Vec(x, y), module, i));
        x += spacing;
    }
    for (int i = 0; i < num_green_lights; ++i) {
	    addChild(createLight<MediumLight<GreenLight>>(Vec(x, y), module, i));
        x += spacing;
    }
}

