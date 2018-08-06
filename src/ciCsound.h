#ifndef CI_CSOUND
#define CI_CSOUND

#include "cinder/app/App.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/Utilities.h"
#include "cinder/audio/ChannelRouterNode.h"

#include <CsoundLib64/CppSound.hpp> // umbeannt, damit in den Frameworks gesucht wird
#include <CsoundLib64/csPerfThread.hpp> // umbenannt..
#include <string>
#include <stdio.h>
using namespace std;

typedef struct csdata_ {
    CSOUND *cs;
    long bufframes;
    int ret;
    int nchnls;
    int nchnls_i;
    bool running;
    bool shouldRecord;
    bool shouldMute;
    bool useAudioInput;
    bool midiInEnabled;

    //        __unsafe_unretained NSMutableArray *valuesCache;
} csdata;

typedef std::shared_ptr<class ciCsound>	ciCsoundRef;

class ciCsound  : public cinder::audio::Node {

public:
    ciCsound(const cinder::audio::Node::Format &format=Format()) : Node(format){};
    ~ciCsound(){}
	//void setupFile(string fileName);
	void start();
	void stop();
	void pause();
    void listOrcChannels();

	void setChan(const char *name, double val);
	MYFLT getChan(const char *name);
    MYFLT* getAudio(const char *name);//fills received_myflt
    void getTable(double *&tablePtr,int tableNum);
	void scoreEvent(char type, const double *pFields);

    CSOUND* getCsound();


    
    bool showHelp;

private:
    
    std::string csdFile();
    std::string csdText;
    
    shared_ptr<CppSound> csound;
    shared_ptr<CsoundPerformanceThread> csThread;
    csdata mCsData;
    MYFLT  *spin,*spout;
    MYFLT* getOutputChannelPtr(string * channelName, controlChannelType channelType=CSOUND_OUTPUT_CHANNEL);
    MYFLT* getInputChannelPtr(string * channelName, controlChannelType channelType);
    
    void initialize()				override;
    void uninitialize()				override;
    void process(ci::audio::Buffer *buffer)override;

};
#endif // CI_CSOUND
