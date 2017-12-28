#include "Fredmin.hpp"


struct MyModule : Module {
	enum ParamIds {
		TEMPO_PARAM,
        PITCH_PARAM,
        REST_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		TEMPO_INPUT,
        PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		CV_OUTPUT,
        GATE_OUTPUT,
		NUM_OUTPUTS
	};
    enum LightIds { OFF, ON, CHALF, C1, C2, C4, C8, C3, C6, B1, B2, B3, B4, B5, B6,
        B7, B8, B9, B10, B11, B12, B13, B14, B15, B16, B17, B18, B19, B20, B21, B22,
        B23, B24, B25, B26, B27, B28, B29, B30, B31, NUM_LIGHTS };
    enum CLightIndeces { CHALF_I, C1_I, C2_I, C4_I, C8_I, C3_I, C6_I, NUM_C };

    struct CLight {
        float phase = 0.0;
        float freq;
        int id;
    };

	float phase, sampleLength, freq;
    CLight cLights[NUM_C];
    uint32_t shiftreg;
    unsigned interval:4;
    unsigned theme:4;

	MyModule();
	void step() override;
    void blinkCLights(CLight *);

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


MyModule::MyModule() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
    lights[ON].value = 1.0;
    cLights[CHALF_I].id = CHALF;
    cLights[C1_I].id = C1;
    cLights[C2_I].id = C2;
    cLights[C4_I].id = C4;
    cLights[C8_I].id = C8;
    cLights[C3_I].id = C3;
    cLights[C6_I].id = C6;
    cLights[CHALF_I].freq = 0.5;
    cLights[C1_I].freq = 1.0;
    cLights[C2_I].freq = 2.0;
    cLights[C4_I].freq = 4.0;
    cLights[C8_I].freq = 8.0;
    cLights[C3_I].freq = 3.0;
    cLights[C6_I].freq = 6.0;
}

void MyModule::step() {
    
	sampleLength = 1.0 / engineGetSampleRate();

	float tempo = params[TEMPO_PARAM].value;
	tempo += inputs[TEMPO_INPUT].value;
	tempo = clampf(tempo, -2.0, 2.0);
	freq = 8.0 * powf(2.0, tempo);

    phase += freq * sampleLength;
   	if (phase >= 0.5) {
   		phase -= 0.5;
    
        for (int i = 0; i < NUM_C; ++i)
            blinkCLights(&cLights[i]);
    }

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

void MyModule::blinkCLights(CLight *cLight) {
    cLight->phase += sampleLength;
    if (cLight->phase >= cLight->freq * freq) {
        cLight->phase -= cLight->freq * freq;
    }
    lights[cLight->id].value = (cLight->phase < cLight->freq * freq / 2) ? 1.0 : 0.0;
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
	addParam(createParam<Davies1900hBlackKnob>(Vec(firstKnobX + 3 * RACK_GRID_WIDTH, knobHeight), module, MyModule::TEMPO_PARAM, -2.0, 2.0, 0.0));
    drawLights();

	addInput(createInput<PJ301MPort>(Vec(33, 186), module, MyModule::PITCH_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(33, 275), module, MyModule::GATE_OUTPUT));

}

void MyModuleWidget::drawLights() {
    int x = 90;
    int y = 110;
    int spacing = 12;
    for (int i = 0; i < 2 + MyModule::NUM_C; ++i) {
	    addChild(createLight<MediumLight<BlueLight>>(Vec(x, y), module, i));
        x += spacing;
    }
    for (int i = 2 + MyModule::NUM_C; i < MyModule::NUM_LIGHTS; ++i) {
	    addChild(createLight<MediumLight<GreenLight>>(Vec(x, y), module, i));
        x += spacing;
    }

}

