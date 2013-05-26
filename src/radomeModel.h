//
//  radomeModel.h
//  radome
//
//  Created by Michael Dewberry on 5/25/13.
//
//

#ifndef __radome__radomeModel__
#define __radome__radomeModel__

#include "AssimpLoader.h"

using namespace ci;
using namespace mndl::assimp;

class radomeModel : public AssimpLoader {
public:
    radomeModel(fs::path filename) : AssimpLoader(filename) {}
    
    //void update(float t);
    void drawWithTransform();
    
    Vec3f getOrigin() const { return _origin; }
    void setOrigin(Vec3f o) { _origin = o; }
    
    Quatf getRotation() const { return _rotation; }
    void setRotation(Quatf r) { _rotation = r; }
    
    Vec3f getRotationOrigin() const { return _rotationOrigin; }
    void setRotationOrigin(Vec3f o) { _rotationOrigin = o; }
    
    float getRotationIncrement() const { return _rotationIncrement; }
    void setRotationIncrement(float f) { _rotationIncrement = f; }
    
    Vec3f _origin;
    
protected:
    Quatf _rotation;
    Vec3f _rotationOrigin;
    float _rotationIncrement;
};


#endif /* defined(__radome__radomeModel__) */
