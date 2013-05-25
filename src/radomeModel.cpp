//
//  radomeModel.cpp
//  radome
//
//  Created by Michael Dewberry on 5/25/13.
//
//

#include "radomeModel.h"

//void radomeModel::update(float t) {
//    if (_rotationIncrement) {
//        _rotation[0] += _rotationIncrement;
//    }
//}

void radomeModel::drawModel() {
    gl::pushMatrices();
    gl::translate(_origin);
    gl::translate(-_rotationOrigin);
    gl::rotate(_rotation);
    gl::translate(_rotationOrigin);
    draw();
    gl::popMatrices();
}