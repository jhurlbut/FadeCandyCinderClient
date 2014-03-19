#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "FCEffectRunner.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MyEffect;
typedef boost::shared_ptr< MyEffect > MyEffectRef;

class MyEffect : public FCEffect
{
public:
	static MyEffectRef create()
    {
        return ( MyEffectRef )( new MyEffect() );
    }
    MyEffect()
        : cycle (0) {
		mPerlin = Perlin();
	}

    float cycle;
	Perlin					mPerlin;

    void beginFrame(const FrameInfo& f)
    {
        const float speed = 10.0;
        cycle = fmodf(cycle + f.timeDelta * speed, 2 * M_PI);
    }

    void shader(ci::Vec3f& rgb, const PixelInfo& p)
    {
        float distance = p.point.length();
        float wave = sinf(3.0 * distance - cycle) + mPerlin.fBm(p.point);
		rgb = rgbToHSV(Color(0.2, 0.3, wave));
    }
};

class FadeCandyClientApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	FCEffectRunnerRef effectRunner; 
};

void FadeCandyClientApp::setup()
{
	
	//point FC to host and port
	effectRunner = FCEffectRunner::create("localhost",7890);
	//create instance of our custom effect
	MyEffectRef e = MyEffect::create();
	effectRunner->setEffect(boost::dynamic_pointer_cast<FCEffect>( e ));
	effectRunner->setMaxFrameRate(100);
	effectRunner->setVerbose(true);
    effectRunner->setLayout("layouts/grid8x8xy.json");
	//add visualizer to see effect on screen
	FCEffectVisualizerRef viz = FCEffectVisualizer::create();
	effectRunner->setVisualizer(viz);
	
}

void FadeCandyClientApp::mouseDown( MouseEvent event )
{
}

void FadeCandyClientApp::update()
{
	effectRunner->update();
}

void FadeCandyClientApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	gl::setViewport( getWindowBounds() );
	effectRunner->draw();
	gl::setMatricesWindow( getWindowSize() );

	Font mDefault;
	#if defined( CINDER_COCOA )        
				mDefault = Font( "Helvetica", 16 );
	#elif defined( CINDER_MSW )    
				mDefault = Font( "Arial", 16 );
	#endif
	gl::enableAlphaBlending();
		gl::drawStringCentered(effectRunner->getDebugString(),Vec2f(getWindowCenter().x,5),Color(1,1,1),mDefault);
		gl::disableAlphaBlending();
}

CINDER_APP_NATIVE( FadeCandyClientApp, RendererGl )
