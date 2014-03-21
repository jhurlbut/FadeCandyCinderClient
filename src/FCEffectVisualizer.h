#pragma once

#include "cinder/Utilities.h"
#include "cinder/app/AppNative.h"
#include "FCEffect.h"

class FCEffectVisualizer;
typedef boost::shared_ptr< FCEffectVisualizer > FCEffectVisualizerRef;

class FCEffectVisualizer {
public:
    static FCEffectVisualizerRef create()
    {
        return ( FCEffectVisualizerRef )( new FCEffectVisualizer() );
    }
	 
	FCEffectVisualizer ();
	
    virtual void update();
	virtual void draw(ci::Vec3f& rgb, const FCEffect::PixelInfo& p);
protected:
    
private:
    FCEffectRunnerRef		mEffectRunner;
	ci::JsonTree			mCurrentLayout;
	float					mScale;
	ci::Vec3f				mOffset;
};