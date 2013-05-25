//
//  radomeAppUI.h
//  radome
//
//  Created by Michael Dewberry on 5/19/13.
//
//

#ifndef __radome__radomeAppUI__
#define __radome__radomeAppUI__

class radomeApp;
class radomeAppUIImpl;

class radomeAppUI
{
public:
    radomeAppUI();
    void init(radomeApp* pApp);
    void update();
    void draw();
protected:
    radomeAppUIImpl* _pImpl;
};

#endif /* defined(__radome__radomeAppUI__) */
