#include "FCEffectRunner.h"

class FCEffectVisualizer;
typedef boost::shared_ptr< FCEffectVisualizer > FCEffectVisualizerRef;

class FCEffectVisualizer {
public:
    static FCEffectVisualizerRef create(FCEffectRunnerRef effectRunner)
    {
        return ( FCEffectVisualizerRef )( new FCEffectVisualizer(effectRunner) );
    }
	 
	FCEffectVisualizer (FCEffectRunnerRef effectRunner);
	FCEffectRunnerRef getEffectRunner(){ return mEffectRunner; }
	void setEffectRunner(FCEffectRunnerRef pRunner);
    virtual void update();
	virtual void draw();
protected:
    
private:
    FCEffectRunnerRef	mEffectRunner;
	ci::JsonTree			mCurrentLayout;
};