#include "FCEffectVisualizer.h"
using namespace cinder;
 
FCEffectVisualizer::FCEffectVisualizer() : mScale(120), mOffset(Vec3f(100,100,0))
{
	
}

void FCEffectVisualizer::update() 
{
	
}
void FCEffectVisualizer::draw(ci::Vec3f& rgb, const FCEffect::PixelInfo& p)
{
	mOffset = Vec3f(ci::app::getWindowCenter().x,ci::app::getWindowCenter().y,0);
	gl::pushMatrices();

	gl::lineWidth( 2 );
	 
    gl::enableAlphaBlending();
	gl::color( Color(rgb.x,rgb.y,rgb.z) );
	gl::drawSphere(mOffset+p.point*mScale,3);
	
	gl::disableAlphaBlending();
	gl::popMatrices();
}