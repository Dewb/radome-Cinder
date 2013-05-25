//
//  radomeGraphics.h
//  radome
//
//  Created by Michael Dewberry on 5/19/13.
//
//

#ifndef __radome__radomeGraphics__
#define __radome__radomeGraphics__

class radomeCamera;

class radomeGraphics
{
public:
    void setCamera(radomeCamera* pCamera);
    void initializeDomeGeometry(int radius, int height);
    void display3DScene();
    void displayDomePreview();
protected:
    void internalDrawScene();
    void internalDrawDome();
    void internalDrawGroundPlane();
    
    int _domeDrawListIndex;
    float _domeRadius;
    float _domeHeight;
    
    radomeCamera* _pCamera;
};

#endif /* defined(__radome__radomeGraphics__) */
