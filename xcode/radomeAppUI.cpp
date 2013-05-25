//
//  radomeAppUI.cpp
//  radome
//
//  Created by Michael Dewberry on 5/19/13.
//
//

#include "radomeAppUI.h"
#include "radomeApp.h"
#include "ciUI.h"

#define SIDEBAR_WIDTH 180
#define CALIBRATIONUI_WIDTH 210
#define DOME_DIAMETER 300
#define DOME_HEIGHT 110
#define NUM_PROJECTORS 3

#define PROJECTOR_INITIAL_HEIGHT 147.5
#define PROJECTOR_INITIAL_DISTANCE DOME_DIAMETER*1.5

class radomeAppUIImpl
{
public:
    void init(radomeApp* pApp);
    void update()
    {
        if (_pUI)
            _pUI->update();
        if (_pCalibrationUI && _pCalibrationUI->isVisible())
            _pCalibrationUI->update();
    }
    void draw()
    {
        if (_pUI)
            _pUI->draw();
        if (_pCalibrationUI && _pCalibrationUI->isVisible())
            _pCalibrationUI->draw();
    }
protected:
    radomeApp* _pApp;
    ciUICanvas* _pUI;
    ciUICanvas* _pCalibrationUI;
    
    vector<string> _displayModeNames;
    vector<string> _mixModeNames;
    vector<string> _mappingModeNames;
};

radomeAppUI::radomeAppUI()
: _pImpl(new radomeAppUIImpl())
{
}

void radomeAppUI::init(radomeApp* pApp)
{
    _pImpl->init(pApp);
}

void radomeAppUI::update()
{
    _pImpl->update();
}

void radomeAppUI::draw()
{
    _pImpl->draw();
}

void addProjectorWidgets(ciUICanvas* pUI, int index)
{
    char buf[30];
    sprintf(buf, "PROJECTOR %d HEIGHT", index);
    pUI->addWidgetDown(new ciUISlider(200, 25, 5.0, 20.0, PROJECTOR_INITIAL_HEIGHT/10.0, buf));
    sprintf(buf, "PROJECTOR %d HEADING", index);
    pUI->addWidgetDown(new ciUISlider(200, 25, 0.0, 120.0, 60.0, buf));
    sprintf(buf, "PROJECTOR %d DISTANCE", index);
    pUI->addWidgetDown(new ciUISlider(200, 25, DOME_DIAMETER/20.0, DOME_DIAMETER/5.0, PROJECTOR_INITIAL_DISTANCE/10.0, buf));
    sprintf(buf, "PROJECTOR %d FOV", index);
    pUI->addWidgetDown(new ciUISlider(200, 25, 20.0, 90.0, 30.0, buf));
    sprintf(buf, "PROJECTOR %d TARGET", index);
    pUI->addWidgetDown(new ciUISlider(200, 25, 0.0, DOME_HEIGHT/10.0, 2.0, buf));
    pUI->addWidgetDown(new ciUISpacer(0, 12));
}

void addRadioAndSetFirstItem(ciUICanvas* pUI, string id, vector<string> options, int orientation, int w, int h)
{
    pUI->addWidgetDown(new ciUIRadio(w, h, id, options, orientation));
    {
        // so silly, ofxUI needs to init radios
        auto pToggle = dynamic_cast<ciUIToggle*>(pUI->getWidget(options[0]));
        if (pToggle)
            pToggle->setValue(true);
    }
}

void radomeAppUIImpl::init(radomeApp* pApp)
{
    _pUI = new ciUICanvas(5, 0, SIDEBAR_WIDTH, pApp->getWindowHeight());
    _pUI->setWidgetSpacing(5.0);
    _pUI->setDrawBack(true);
    
    //_pUI->setFont("GUI/Exo-Regular.ttf");
    _pUI->addWidgetDown(new ciUILabel("RADOME 0.3", CI_UI_FONT_LARGE));
    _pUI->addWidgetDown(new ciUISpacer(0, 12));
    
    _displayModeNames.push_back("3D Scene");
    _displayModeNames.push_back("Cube Map");
    _displayModeNames.push_back("Dome Preview");
    _displayModeNames.push_back("Output Preview");
    addRadioAndSetFirstItem(_pUI, "DISPLAY MODE", _displayModeNames, CI_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addWidgetDown(new ciUISpacer(0, 12));
    
    _pUI->addWidgetDown(new ciUILabel("PROJECTORS", CI_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ciUILabelButton(false, "Calibrate...", CI_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ciUILabelButton(false, "Show Window", CI_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ciUISpacer(0, 12));
    
    _pUI->addWidgetDown(new ciUILabel("CONTENT", CI_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ciUILabelButton(false, "Add 3D Model...", CI_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ciUILabelButton(false, "2D Input...", CI_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ciUILabelButton(false, "Plugins...", CI_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ciUISpacer(0, 12));
    
    _pUI->addWidgetDown(new ciUILabel("MIXER", CI_UI_FONT_MEDIUM));
    //_pUI->addWidgetDown(new ciUIBiLabelSlider(0, 0, SIDEBAR_WIDTH-10, 30, 0, 100, _vidOverlay.getFaderValue()*100.0, "XFADE", "2D", "3D", CI_UI_FONT_MEDIUM));
    
    _mixModeNames.push_back("Underlay");
    _mixModeNames.push_back("Overlay");
    _mixModeNames.push_back("Mask");
    addRadioAndSetFirstItem(_pUI, "BLEND MODE", _mixModeNames, CI_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addWidgetDown(new ciUISpacer(0, 12));
    
    _mappingModeNames.push_back("Lat/Long");
    _mappingModeNames.push_back("Quadrants");
    _mappingModeNames.push_back("Fisheye");
    _mappingModeNames.push_back("Geodesic");
    _mappingModeNames.push_back("Cinematic");
    addRadioAndSetFirstItem(_pUI, "MAPPING MODE", _mappingModeNames, CI_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addWidgetDown(new ciUISpacer(0, 12));
    
    _pCalibrationUI = new ciUICanvas(SIDEBAR_WIDTH + 5, 0, CALIBRATIONUI_WIDTH, pApp->getWindowHeight());
    _pCalibrationUI->setWidgetSpacing(5.0);
    _pCalibrationUI->setDrawBack(true);
    //_pCalibrationUI->setFont("GUI/Exo-Regular.ttf");
    _pCalibrationUI->addWidgetDown(new ciUILabel("CALIBRATION", CI_UI_FONT_MEDIUM));
    _pCalibrationUI->addWidgetDown(new ciUISpacer(0, 12));
    for (int ii=0; ii<3; ii++) {
        addProjectorWidgets(_pCalibrationUI, ii);
    }
    _pCalibrationUI->setVisible(false);

}

