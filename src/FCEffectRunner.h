/*
*
* Copyright (c) 2014, James Hurlbut
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or
* without modification, are permitted provided that the following
* conditions are met:
*
* Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
*
* Neither the name of James Hurlbut nor the names of its
* contributors may be used to endorse or promote products
* derived from this software without specific prior written
* permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

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
    bool setLayout(std::string pFilename, int pChannel);
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