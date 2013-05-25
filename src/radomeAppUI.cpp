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
#include "Resources.h"

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
    
    void onUIEvent(ciUIEvent* pEvent);
    void onKeyDown(KeyEvent event);

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

void radomeAppUI::keyDown(KeyEvent event)
{
    _pImpl->onKeyDown(event);
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

bool matchRadioButton(string widgetName, vector<string> names, int* pValue)
{
    for (int ii = 0; ii < names.size(); ii++) {
        if (widgetName == names[ii]) {
            if (pValue) {
                *pValue = ii;
            }
            return true;
        }
    }
    return false;
}

void radomeAppUIImpl::init(radomeApp* pApp)
{
    _pApp = pApp;

    CI_UI_GLOBAL_PADDING = 3; // todo: solve this more cleanly in ciUI
    
    _pUI = new ciUICanvas(5, 0, SIDEBAR_WIDTH, pApp->getWindowHeight());
    _pUI->setWidgetSpacing(5.0);
    _pUI->setDrawBack(true);
    _pUI->setFont(loadResource(EXO_REGULAR_TTF), "Exo-Regular", 24, 18, 14);
    _pUI->registerUIEvents(this, &radomeAppUIImpl::onUIEvent);
    
    _pUI->addWidgetDown(new ciUILabel("RADOME 0.3", CI_UI_FONT_LARGE));
    _pUI->addWidgetDown(new ciUISpacer(0, 10));
    
    _displayModeNames = pApp->getDisplayModeNames();
    addRadioAndSetFirstItem(_pUI, "DISPLAY MODE", _displayModeNames, CI_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addWidgetDown(new ciUISpacer(0, 10));
    
    _pUI->addWidgetDown(new ciUILabel("PROJECTORS", CI_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ciUILabelButton(false, "Calibrate...", CI_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ciUILabelButton(false, "Show Window", CI_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ciUISpacer(0, 10));

    _pUI->addWidgetDown(new ciUILabel("CONTENT", CI_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ciUILabelButton(false, "Add 3D Model...", CI_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ciUILabelButton(false, "2D Input...", CI_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ciUILabelButton(false, "Plugins...", CI_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ciUISpacer(0, 10));
    
    _pUI->addWidgetDown(new ciUILabel("MIXER", CI_UI_FONT_MEDIUM));
    _pUI->addWidgetDown(new ciUIBiLabelSlider(0, 0, SIDEBAR_WIDTH-10, 30, 0, 100, 850.0, "XFADE", "2D", "3D", CI_UI_FONT_MEDIUM));
    
    _mixModeNames.push_back("Underlay");
    _mixModeNames.push_back("Overlay");
    _mixModeNames.push_back("Mask");
    addRadioAndSetFirstItem(_pUI, "BLEND MODE", _mixModeNames, CI_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addWidgetDown(new ciUISpacer(0, 10));
    
    _mappingModeNames.push_back("Lat/Long");
    _mappingModeNames.push_back("Quadrants");
    _mappingModeNames.push_back("Fisheye");
    _mappingModeNames.push_back("Geodesic");
    _mappingModeNames.push_back("Cinematic");
    addRadioAndSetFirstItem(_pUI, "MAPPING MODE", _mappingModeNames, CI_UI_ORIENTATION_VERTICAL, 16, 16);
    _pUI->addWidgetDown(new ciUISpacer(0, 10));
    
    _pUI->autoSizeToFitWidgets();
    _pUI->setVisible(true);

    
    _pCalibrationUI = new ciUICanvas(SIDEBAR_WIDTH + 5, 0, CALIBRATIONUI_WIDTH, pApp->getWindowHeight());
    _pCalibrationUI->setWidgetSpacing(5.0);
    _pCalibrationUI->setDrawBack(true);
    _pCalibrationUI->setFont(loadResource(EXO_REGULAR_TTF), "Exo-Regular", 24, 18, 14);
    _pCalibrationUI->autoSizeToFitWidgets();
    _pCalibrationUI->registerUIEvents(this, &radomeAppUIImpl::onUIEvent);

    _pCalibrationUI->addWidgetDown(new ciUILabel("CALIBRATION", CI_UI_FONT_MEDIUM));
    _pCalibrationUI->addWidgetDown(new ciUISpacer(0, 10));
    for (int ii=0; ii<3; ii++) {
        addProjectorWidgets(_pCalibrationUI, ii);
    }
    
    _pCalibrationUI->autoSizeToFitWidgets();
    _pCalibrationUI->setVisible(false);
    
}

void radomeAppUIImpl::onUIEvent(ciUIEvent* pEvent)
{
    auto widget = pEvent->widget;
    string name = widget->getName();
        
    int radio;
    if(matchRadioButton(name, _displayModeNames, &radio))
    {
        _pApp->setDisplayMode(radio);
        return;
    }
    
    if (matchRadioButton(name, _mixModeNames, &radio))
    {
        //_pApp->setMixMode(radio);
        return;
    }
    if (matchRadioButton(name, _mappingModeNames, &radio))
    {
        //_pApp->setMappingMode(radio);
        return;
    }
    
    if (name == "XFADE") {
        auto slider = dynamic_cast<ciUISlider*>(widget);
        if (slider) {
            //_vidOverlay.setFaderValue(slider->getValue());
        }
    } else if (name == "Add 3D Model...") {
        auto pButton = dynamic_cast<ciUIButton*>(widget);
        if (pButton && !pButton->getValue())
        {
            _pApp->browseForModel();
        }
    } else if (name == "Show Window") {
        auto pButton = dynamic_cast<ciUIButton*>(widget);
        if (pButton && !pButton->getValue())
        {
            //showProjectorWindow();
        }
    } else if (name == "Calibrate...") {
        auto pButton = dynamic_cast<ciUIButton*>(widget);
        if (pButton && !pButton->getValue())
        {
            if (_pCalibrationUI) {
                bool bVis = _pCalibrationUI->isVisible();
                _pCalibrationUI->setVisible(!bVis);
            }
        }
    }
}

void radomeAppUIImpl::onKeyDown(KeyEvent event)
{
    //float accel = 3.0;
    //auto model = *(_modelList.rbegin());
    
    switch (event.getChar())
    {
//        case 'w': if (model) model->_origin.y += accel; break;
//        case 's': if (model) model->_origin.y -= accel; break;
//        case 'a': if (model) model->_origin.x -= accel; break;
//        case 'd': if (model) model->_origin.x += accel; break;
//        case 'z': if (model) model->_origin.z += accel; break;
//        case 'x': if (model) model->_origin.z -= accel; break;
//        case 'W': if (model) model->_origin.y += accel * 4; break;
//        case 'S': if (model) model->_origin.y -= accel * 4; break;
//        case 'A': if (model) model->_origin.x -= accel * 4; break;
//        case 'D': if (model) model->_origin.x += accel * 4; break;
//        case 'Z': if (model) model->_origin.z += accel * 4; break;
//        case 'X': if (model) model->_origin.z -= accel * 4; break;
        case 'l':
        {
            _pApp->browseForModel();
            break;
        }
        case 'm':
        {
            _pApp->cycleDisplayMode();
            auto radio = dynamic_cast<ciUIRadio*>(_pUI->getWidget("DISPLAY MODE"));
            if (radio) {
                radio->activateToggle(_displayModeNames[_pApp->getCurrentDisplayMode()]);
            }            
        }
            break;
        case 'f':
        {
            //_fullscreen = !_fullscreen;
            //ofSetFullscreen(_fullscreen);
        }
            break;
        case 'p':
        {
            //showProjectorWindow();
        }
            break;
        case 'c':
        {
            if (_pCalibrationUI)
                _pCalibrationUI->setVisible(!_pCalibrationUI->isVisible());
        }
            break;
        case 'C':
        {
//            if (_modelList.size() > 0)
//            {
//                auto m = _modelList.back();
//                _modelList.pop_back();
//                if (m) delete(m);
//            }
        }
            break;
        case 'r':
        {
//            if (model) {
//                if (model->getRotationIncrement() == 0) {
//                    model->setRotationOrigin(model->getOrigin());
//                    model->setRotation(ofVec4f(0.0,
//                                               frand_bounded(),
//                                               frand_bounded(),
//                                               frand_bounded()));
//                }
//                model->setRotationIncrement(model->getRotationIncrement() + 2.0);
//                if (model->getRotationIncrement() == 10.0) {
//                    model->setRotationIncrement(0.0);
//                }
//            }
        }
            break;
    }
    
}


