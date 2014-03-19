#include "OPCClient.h"
#include "cinder/Json.h"
#include "cinder/Utilities.h"
#include "cinder/app/AppNative.h"
#include <boost/enable_shared_from_this.hpp>

class FCEffectRunner;
typedef boost::shared_ptr< FCEffectRunner > FCEffectRunnerRef;

class FCEffect;
typedef boost::shared_ptr< FCEffect > FCEffectRef;

// Abstract base class for one LED effect
class FCEffect  {

public:
	
    class PixelInfo;
    class FrameInfo;
    class DebugInfo;

    /*
     * Calculate a pixel value, using floating point RGB in the nominal range [0, 1].
     *
     * The 'rgb' vector is initialized to (0, 0, 0). Caller is responsible for
     * clamping the results if necessary. This supports effects that layer with
     * other effects using full floating point precision and dynamic range.
     *
     * This function may be run in parallel, not run at all, or run multiple times
     * per pixel. It therefore can't have side-effects other than producing an RGB
     * value.
     */
    virtual void shader(ci::Vec3f& rgb, const PixelInfo& p) = 0;

    /*
     * Serialized post-processing on one pixel. This runs after shader(), once
     * per mapped pixel, with the ability to modify Effect data. This shoudln't
     * be used for anything CPU-intensive, but some effects require closed-loop
     * feedback based on the calculated color.
     */
    virtual void postProcess(const ci::Vec3f rgb, const PixelInfo& p);

    // Optional begin/end frame callbacks
    virtual void beginFrame(const FrameInfo& f)= 0;
    virtual void endFrame(const FrameInfo& f);

    // Optional callback, invoked once per second when verbose mode is enabled.
    // This can print parameters out to the console.
    virtual void debug(const DebugInfo& d);


    // Information about one LED pixel
    class PixelInfo {
    public:
		PixelInfo(unsigned index, ci::JsonTree layout);

        // Point coordinates
        ci::Vec3f point;

        // Index in the framebuffer
        unsigned index;

        // Parsed JSON for this pixel's layout
        ci::JsonTree layout;

        // Is this pixel being used, or is it a placeholder?
        bool isMapped() const;

	// Look up data from the JSON layout
	ci::JsonTree get(std::string attribute);
	double getNumber(std::string attribute);
	double getArrayNumber(std::string attribute, int index);
    };

    typedef std::vector<PixelInfo> PixelInfoVec;
    typedef std::vector<PixelInfo>::const_iterator PixelInfoIter;

    // Information about one Effect frame
    class FrameInfo {
    public:
        FrameInfo();
        void init(ci::JsonTree layout);

        // Seconds passed since the last frame
        float timeDelta;

        // Info for every pixel
        PixelInfoVec pixels;
    };

    // Information passed to debug() callbacks
    class DebugInfo {
    public:
        DebugInfo(FCEffectRunnerRef runner);

        FCEffectRunnerRef runner;
    };
};

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
	FCEffectRunner(std::string host, int port);
    bool setServer(std::string hostname, int pPort);
    bool setLayout(std::string pFilename);
    void setEffect(FCEffectRef pEffect);
    void setMaxFrameRate(float fps);
    void setVerbose(bool verbose = true);
	
    bool hasLayout() const;
    ci::JsonTree getLayout() const;
    FCEffectRef getEffect();
    OPCClientRef getClient();

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

    // Main loop body
    float doFrame();
    void doFrame(float timeDelta);
	
    void update();

protected:
    
private:
    OPCClientRef opc;
    ci::JsonTree layout;
    FCEffectRef effect;
    std::vector<char> frameBuffer;
    FCEffect::FrameInfo frameInfo;

    float minTimeDelta;
    float currentDelay;
    float filteredTimeDelta;
    float debugTimer;
    bool verbose;
    float lastElapsedSeconds;

    void usage(const char *name);
    void debug();
};