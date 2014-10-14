#pragma once

#include "OPCClient.h"
#include "cinder/Json.h"
#include "cinder/Utilities.h"
#include "cinder/app/AppNative.h"
#include <boost/enable_shared_from_this.hpp>
#include "FCEffect.h"
#include "FCEffectVisualizer.h"

class FCEffectRunner;
typedef boost::shared_ptr< FCEffectRunner > FCEffectRunnerRef;

class FCEffect;
typedef boost::shared_ptr< FCEffect > FCEffectRef;


class FCEffectRunner : public boost::enable_shared_from_this<FCEffectRunner>{
public:
    static FCEffectRunnerRef create()
    {
        return ( FCEffectRunnerRef )( new FCEffectRunner() );
    }
	static FCEffectRunnerRef create(std::string host, int port)
    {
        return ( FCEffectRunnerRef )( new FCEffectRunner(host,port) );
    }
	FCEffectRunner();
	~FCEffectRunner();
	FCEffectRunner(std::string host, int port);
    bool setServer(std::string hostname, int pPort);
    bool setLayout(std::string pFilename);
    void setEffect(FCEffectRef pEffect);
	void setVisualizer(FCEffectVisualizerRef pViz);
    void setMaxFrameRate(float fps);
    void setVerbose(bool verbose = true);
	void setChannelNum(int pChan){channelNum = pChan;}
    bool hasLayout() const;
    ci::JsonTree getLayout() const;
    FCEffectRef getEffect();
    OPCClientRef getClient();
	FCEffect::FrameInfo getFrameInfo(){ return frameInfo; }
    // Access to most recent framebuffer information
    const FCEffect::PixelInfoVec& getPixelInfo() const;
    const uint8_t* getPixel(unsigned index) const;
    void getPixelColor(unsigned index, ci::Vec3f rgb);

    // Time stats
    float getFrameRate() const;
    float getTimePerFrame() const;
    float getBusyTimePerFrame() const;
    float getIdleTimePerFrame() const;
    float getPercentBusy() const;
	std::string getDebugString() const;

    // Main loop body
    float doFrame();
    void doFrame(float timeDelta);
	
    void update();
	void draw();
protected:
    
private:
	void addDebugLine(std::string pLine);
    OPCClientRef opc;
    ci::JsonTree layout;
    FCEffectRef effect;
    std::vector<char> frameBuffer;
    FCEffect::FrameInfo frameInfo;
	FCEffectVisualizerRef	mEffectVisualizer;
	
    float minTimeDelta;
    float currentDelay;
    float filteredTimeDelta;
    float debugTimer;
    bool verbose;
    float lastElapsedSeconds;
	int		channelNum;

	std::string mDebugMsg;
	
	void						onConnect( TcpSessionRef session );
	void						onError( std::string err, size_t bytesTransferred );
    void usage(const char *name);
    void debug();
};