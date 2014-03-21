#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "FCEffectRunner.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/MayaCamUI.h"

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
        : time (0) {
		mPerlin = Perlin();
	}

    float time;
	Perlin					mPerlin;

    void beginFrame(const FrameInfo& f)
    {
        const float speed = 1.0;
        time += f.timeDelta * speed;
    }

    void shader(ci::Vec3f& rgb, const PixelInfo& p)
    {
        float distance = p.point.length();
		Vec3f timepoint = p.point + Vec3f(0,0,time);
        float noiseval = mPerlin.fBm(timepoint);
		rgb = rgbToHSV(Color(0.2, 0.3, noiseval));
	}
};

class FadeCandyClientApp : public AppNative {
public:
	void setup();
	void update();
	void draw();
	FCEffectRunnerRef effectRunner;
	MayaCamUI	mMayaCam;
	// keep track of the mouse
	Vec2i		mMousePos;
    
    void prepareSettings( Settings *settings );
	void mouseMove( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void resize();
};

void FadeCandyClientApp::setup()
{
	
	//point FC to host and port
	effectRunner = FCEffectRunner::create("localhost",7890);
	//create instance of our custom effect
	MyEffectRef e = MyEffect::create();
	effectRunner->setEffect(boost::dynamic_pointer_cast<FCEffect>( e ));
	effectRunner->setMaxFrameRate(400);
	effectRunner->setVerbose(true);
    effectRunner->setLayout("layouts/grid32x16y.json");
	//add visualizer to see effect on screen
	FCEffectVisualizerRef viz = FCEffectVisualizer::create();
	effectRunner->setVisualizer(viz);
	
	// set up the camera
	CameraPersp cam;
	cam.setEyePoint( Vec3f(300.0f, 250.f, -500.0f) );
	cam.setCenterOfInterestPoint( Vec3f(300.0f, 200.0f, 0.0f) );
	cam.setPerspective( 60.0f, getWindowAspectRatio(), 1.0f, 1000.0f );
	mMayaCam.setCurrentCam( cam );
	gl::disableVerticalSync();
}
void FadeCandyClientApp::prepareSettings( Settings *settings )
{
    settings->setFrameRate( 300.0f );
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
	gl::color( Color(1,1,1) );
	gl::setMatrices( mMayaCam.getCamera() );
	effectRunner->draw();

	//draw debug info
	gl::setMatricesWindow( getWindowSize() );
	
	Font mDefault;
	#if defined( CINDER_COCOA )        
				mDefault = Font( "Helvetica", 16 );
	#elif defined( CINDER_MSW )    
				mDefault = Font( "Arial", 16 );
	#endif
	gl::enableAlphaBlending();
	gl::drawStringCentered(effectRunner->getDebugString(),Vec2f(getWindowCenter().x,5),Color(1,1,1),mDefault);
	gl::drawString(toString(mMayaCam.getCamera().getEyePoint()),Vec2f(0,0));
	gl::drawString(toString(mMayaCam.getCamera().getCenterOfInterestPoint()),Vec2f(0,20));
	gl::disableAlphaBlending();
}
//camera interaction
void FadeCandyClientApp::mouseMove( MouseEvent event )
{
	// keep track of the mouse
	mMousePos = event.getPos();
}

void FadeCandyClientApp::mouseDown( MouseEvent event )
{	
	// let the camera handle the interaction
	mMayaCam.mouseDown( event.getPos() );
}

void FadeCandyClientApp::mouseDrag( MouseEvent event )
{
	// keep track of the mouse
	mMousePos = event.getPos();

	// let the camera handle the interaction
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void FadeCandyClientApp::resize()
{
	// adjust aspect ratio
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
}

CINDER_APP_NATIVE( FadeCandyClientApp, RendererGl )
