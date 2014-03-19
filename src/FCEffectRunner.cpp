#include "FCEffectRunner.h"

using namespace cinder;

FCEffectRunner::FCEffectRunner() : effect(0),
      minTimeDelta(0),
      currentDelay(0),
      filteredTimeDelta(0),
      debugTimer(0),
      verbose(false)
{
	// Defaults
    setMaxFrameRate(300);
    opc = OPCClient::create();
}
FCEffectRunner::FCEffectRunner(std::string host, int port) : effect(0),
      minTimeDelta(0),
      currentDelay(0),
      filteredTimeDelta(0),
      debugTimer(0),
      verbose(false)
{
	// Defaults
    setMaxFrameRate(300);
	opc = OPCClient::create();
    setServer(host,port);
}
void FCEffectRunner::setMaxFrameRate(float fps)
{
    minTimeDelta = 1.0 / fps;
}
void FCEffectRunner::setVerbose(bool verbose)
{
    this->verbose = verbose;
}
bool FCEffectRunner::setServer(std::string hostname, int pPort)
{
    opc->connect(hostname,pPort);
	return true;
}
bool FCEffectRunner::setLayout(std::string pFilename)
{
   
	layout = ci::JsonTree(ci::app::loadAsset(pFilename));
    std::string layoutStr = layout.serialize();

    // Set up an empty framebuffer, with OPC packet header
	int frameBytes = layout.getChildren().size() * 3;
    frameBuffer.resize(sizeof(OPCClient::Header) + frameBytes);
    OPCClient::Header::view(frameBuffer).init(0, opc->SET_PIXEL_COLORS, frameBytes);

    // Init pixel info
    frameInfo.init(layout);

    return true;
}


ci::JsonTree FCEffectRunner::getLayout() const
{
    return layout;
}
bool FCEffectRunner::hasLayout() const
{
    return layout.hasChildren();
}
void FCEffectRunner::setEffect(FCEffectRef pEffect)
{
    effect = pEffect;
}
FCEffectRef FCEffectRunner::getEffect()
{
    return effect;
}

float FCEffectRunner::getFrameRate() const
{
    return filteredTimeDelta > 0.0f ? 1.0f / filteredTimeDelta : 0.0f;
}
float FCEffectRunner::getTimePerFrame() const
{
    return filteredTimeDelta;
}
float FCEffectRunner::getBusyTimePerFrame() const
{
    return getTimePerFrame() - getIdleTimePerFrame();
}
float FCEffectRunner::getIdleTimePerFrame() const
{
    return std::max(0.0f, currentDelay);
}
float FCEffectRunner::getPercentBusy() const
{
    return 100.0f * getBusyTimePerFrame() / getTimePerFrame();
}
void FCEffectRunner::update()
{
    doFrame();
}
float FCEffectRunner::doFrame()
{
    float now = ci::app::getElapsedSeconds(); 
	float delta = now - lastElapsedSeconds;
    lastElapsedSeconds = now;

    // Max timestep; jump ahead if we get too far behind.
    const float maxStep = 0.1;
    if (delta > maxStep) {
        delta = maxStep;
    }

    doFrame(delta);
    return delta;
}

void FCEffectRunner::doFrame(float timeDelta)
{
	opc->update();
    frameInfo.timeDelta = timeDelta;

    if (getEffect() && hasLayout()) {
        effect->beginFrame(frameInfo);

        // Only calculate the effect if we have a connection
        if (opc->tryConnect()) {
			
            uint8_t *dest = OPCClient::Header::view(frameBuffer).data();

            for (FCEffect::PixelInfoIter i = frameInfo.pixels.begin(), e = frameInfo.pixels.end(); i != e; ++i) {
                Vec3f rgb(0.f, 0.f, 0.f);
                const FCEffect::PixelInfo &p = *i;

                if (p.isMapped()) {
                    effect->shader(rgb, p);
                    effect->postProcess(rgb, p);
                }

                for (unsigned i = 0; i < 3; i++) {
                    *(dest++) = std::min<int>(255, std::max<int>(0, rgb[i] * 255 + 0.5));
                }
            }

            opc->write(frameBuffer);
        }

        effect->endFrame(frameInfo);
    }

    // Low-pass filter for timeDelta, to estimate our frame rate
    const float filterGain = 0.05;
    filteredTimeDelta += (timeDelta - filteredTimeDelta) * filterGain;

    // Negative feedback loop to adjust the delay until we hit a target frame rate.
    // This lets us hit the target rate smoothly, without a lot of jitter between frames.
    // If we calculated a new delay value on each frame, we'd easily end up alternating
    // between too-long and too-short frame delays.
    currentDelay += (minTimeDelta - timeDelta) * filterGain;

    // Make sure filteredTimeDelta >= currentDelay. (The "busy time" estimate will be >= 0)
    filteredTimeDelta = std::max(filteredTimeDelta, currentDelay);

    // Periodically output debug info, if we're in verbose mode
    if (verbose) {
        const float debugInterval = 1.0f;
        if ((debugTimer += timeDelta) > debugInterval) {
            debugTimer = fmodf(debugTimer, debugInterval);
            debug();
        }
    }

    // Add the extra delay, if we have one. This is how we throttle down the frame rate.
    if (currentDelay > 0) {
        sleep(currentDelay * 1e6);
    }
}
void FCEffectRunner::debug()
{
    fprintf(stderr, " %7.2f FPS -- %6.2f%% CPU [%.2fms busy, %.2fms idle]\n",
        getFrameRate(),
        getPercentBusy(),
        1e3f * getBusyTimePerFrame(),
        1e3f * getIdleTimePerFrame());

    if (effect) {
       // FCEffect::DebugInfo d(shared_from_this());
       //// effect->debug(d);
    }
}
OPCClientRef FCEffectRunner::getClient()
{
    return opc;
}

const FCEffect::PixelInfoVec& FCEffectRunner::getPixelInfo() const
{
    return frameInfo.pixels;
}

const uint8_t* FCEffectRunner::getPixel(unsigned index) const
{
    return OPCClient::Header::view(frameBuffer).data() + index * 3;
}

void FCEffectRunner::getPixelColor(unsigned index, ci::Vec3f rgb)
{
    const uint8_t *byte = getPixel(index);
    for (unsigned i = 0; i < 3; i++) {
        rgb[i] = *(byte++) / 255.0f;
    }
}

FCEffect::PixelInfo::PixelInfo(unsigned index, ci::JsonTree layout)
    : index(index), layout(layout)
{
    if (isMapped()) {
        for (unsigned i = 0; i < 3; i++) {
            point[i] = getArrayNumber("point", i);
        }
    }
}

inline bool FCEffect::PixelInfo::isMapped() const
{
	return layout.hasChildren();
}

ci::JsonTree FCEffect::PixelInfo::get(std::string attribute)
{
    return layout.getChild(attribute);
}

double FCEffect::PixelInfo::getArrayNumber(std::string attribute, int index)
{
	JsonTree array = get(attribute);
	float val = atof(array.getChild(index).getValue().c_str());
	return val;
}

FCEffect::FrameInfo::FrameInfo()
    : timeDelta(0) {}

void FCEffect::FrameInfo::init(ci::JsonTree layout)
{
    timeDelta = 0;
    pixels.clear();

    for (unsigned i = 0; i < layout.getChildren().size(); i++) {
        PixelInfo p(i, layout[i]);
        pixels.push_back(p);
    }
}
FCEffect::DebugInfo::DebugInfo(FCEffectRunnerRef runner)
    : runner(runner) {}


void FCEffect::beginFrame(const FrameInfo &f) {}
void FCEffect::endFrame(const FrameInfo &f) {}
void FCEffect::debug(const DebugInfo &f) {}
void FCEffect::postProcess(const ci::Vec3f rgb, const PixelInfo& p) {}