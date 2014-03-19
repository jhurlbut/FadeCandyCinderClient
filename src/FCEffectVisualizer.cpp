#include "FCEffectVisualizer.h"
using namespace cinder;

FCEffectVisualizer::FCEffectVisualizer(FCEffectRunnerRef effectRunner)
{
	setEffectRunner(effectRunner);
}
void FCEffectVisualizer::setEffectRunner(FCEffectRunnerRef pRunner)
{
	mEffectRunner = pRunner;
	mCurrentLayout = pRunner->getLayout();
}
void FCEffectVisualizer::update() 
{
	
}
void FCEffectVisualizer::draw() 
{
	gl::pushMatrices();
	 gl::color( Color::gray(.5) );
    gl::enableAlphaBlending();
	/*if(mCurrentLayout){
		for(auto pt : mCurrentLayout.getChildren()){
			
		}
	}*/
	gl::disableAlphaBlending();
	gl::popMatrices();
}