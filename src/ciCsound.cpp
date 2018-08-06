/*
 TODO:
 - put whole csound framework into ciCsound
 - use audioloop/Soundstream
 */

#include "ciCsound.h"


//-------------------------------------------------
void ciCsound::setupFile(string fileName) {
    const cinder::fs::path fName= fileName;
    cinder::DataSourceRef filename = cinder::app::loadAsset( fName );
    /* make instance of csound */
    csound = new CppSound;
    /* compile the instrument and score from .csd file */
    csound->Compile((char*)fileName.c_str());
    /* start csound in it's own thread */
    csThread = new CsoundPerformanceThread(csound->getCsound());
}

std::string ciCsound::csdFile(){
    
    csdText = "<CsoundSynthesizer> \n\
<CsOptions> csound \n\
";
    if(showHelp) csdText.append(" --help \n\
");
    if(mCsData.midiInEnabled)  csdText.append("--midi-devices -+rtmidi=coremidi --midi-key-cps=4 --midi-velocity-amp=5 \n\
");
    csdText.append(" -iadc \n\
");
    csdText.append("-g -+rtaudio=null  -d --devices  -odac temp.orc temp.sco\n\
</CsOptions>  \n\
<CsInstruments>  \n\
sr = 44100  \n\
ksmps = 32  \n\
nchnls = 2  \n\
0dbfs = 1 ;set maximum level as 1\n\
");
    if(mCsData.midiInEnabled)csdText.append("massign   0, 1 ;assign all incoming midi to instr 1\n\
");
    csdText.append(";-----------------------------------------------------------\n\
;Instrument 3 : New FM algorithm, modified to produce large timbre\n\
;               shifts using modulation of I and r. Detuned chorusing employed.\n\
;-----------------------------------------------------------\n\
instr       1\n\
ishift      =           .00666667               ;shift it 8/1200.\n\
ipch        =           cpspch(p5)              ;convert parameter 5 to cps.\n\
ioct        =           octpch(p5)              ;convert parameter 5 to oct.\n\
aadsr       linsegr     0, p3/3, 1.0, p3/3, 1.0, p3/3, 0 ;ADSR envelope\n\
amodi       linseg      0, p3/3, 5, p3/3, 3, p3/3, 0 ;ADSR envelope for I\n\
amodr       linseg      p6, p3, p7              ;r moves from p6->p7 in p3 sec.\n\
a1          =           amodi*(amodr-1/amodr)/2\n\
a1ndx       =           abs(a1*2/20)            ;a1*2 is normalized from 0-1.\n\
a2          =           amodi*(amodr+1/amodr)/2\n\
a3          tablei      a1ndx, 3, 1             ;lookup tbl in f3, normal index\n\
ao1         poscil      a1, ipch, 2             ;cosine\n\
a4          =           exp(-0.5*a3+ao1)\n\
ao2         poscil      a2*ipch, ipch, 2        ;cosine\n\
aoutl       poscil      .05*aadsr*a4, ao2+cpsoct(ioct+ishift), 1 ;fnl outleft\n\
aoutr       poscil      .05*aadsr*a4, ao2+cpsoct(ioct-ishift), 1 ;fnl outright\n\
outs        aoutl, aoutr\n\
endin\n\
instr 2  \n\
k2 chnget \"freq\" \n\
aIn       inch      1   ;take input from channel 1\n\
aInB      inch      2   ;take input from channel 1\n\
kInLev    downsamp  aIn ;convert audio input to control signal\n\
//printk    .2, abs(kInLev)\n\
;make modulator frequency oscillate 200 to 1000 Hz\n\
kModFreq  poscil    400, 1/2\n\
kModFreq  =         kModFreq+600+k2\n\
aMod      poscil    1, kModFreq ;modulator signal\n\
aRM       =         aIn * aMod ;ring modulation\n\
outch     1, aRM, 2, aRM ;output to channel 1 and 2\n\
endin   \n\
</CsInstruments>   \n\
<CsScore>  \n\
f0 0 65537        ;blank\n\
f1 0 65537  10 1      ;sine wave\n\
f2 0 65537  11 1      ;cosine wave\n\
f3 0 65537 -12 20.0  ;unscaled ln(I(x)) from 0 to 20.0\n\
;i1 0 10 0 8.06     \n\
i2 0 360000 ;audioin\n\
</CsScore> \n\
</CsoundSynthesizer>");
    
    
    return csdText;
}
//-------------------------------------------------
void ciCsound::initialize() {
    
    
    //;csound -d --devices -iadc3 -odac3 -g  temp.orc temp.sco -+rtmidi=alsa -Ma\n\
    //csound  --help -+rtmidi=coremidi -+rtaudio=null -d -B512 --devices --midi-devices  -iadc10 temp.orc temp.sco\n\
    
    /* make instance of csound */
    mCsData.shouldMute = false;
    mCsData.midiInEnabled = false;
    if(getNumConnectedInputs()) mCsData.useAudioInput = true;
    else mCsData.useAudioInput = false;
    
    showHelp=true;
    
    csound = new CppSound;
    csound->setCSD(csdFile());
    
    /* export csd text for performance */
    csound->exportForPerformance();
    //csound->ReadScore
    
    cout<<"getInternalBuffer->getNumFrames "<<getInternalBuffer()->getNumFrames()<<endl;
    cout<<"getFramesPerBlock "<<getFramesPerBlock()<<endl;
    csound->GetHostData();
    /* compile the instrument and score from csd text */
    csound->compile();
    
    mCsData.running = true;
    mCsData.cs=csound->getCsound();
    mCsData.nchnls=getNumChannels();
    // mCsData.ret=ret;
    
    spout = csound->GetSpout();
    spin = csound->GetSpin();
    
    
    /* start csound in it's own thread */
    csThread = new CsoundPerformanceThread(csound->getCsound());
    // start();
    listOrcChannels();
    
}
void ciCsound::uninitialize()
{
    stop();
    printf("Csound thread stopped.\n");
}
MYFLT* ciCsound::getInputChannelPtr(string * channelName, controlChannelType channelType)
{
    MYFLT *value;
    csoundGetChannelPtr(mCsData.cs, &value, channelName->c_str() ,
                        channelType | CSOUND_INPUT_CHANNEL);
    return value;
}

MYFLT* ciCsound::getOutputChannelPtr(string * channelName, controlChannelType channelType)
{
    MYFLT *value;
    csoundGetChannelPtr(mCsData.cs, &value, csound->GetOutputName() ,
                        channelType | CSOUND_OUTPUT_CHANNEL);
    return value;
}
void ciCsound::process(cinder::audio::Buffer *buffer)
{
    
    //csoundGetOutputBuffer();
    CSOUND *cs = mCsData.cs;
    // int ret = mCsData.ret;
    int nchnls = buffer->getNumChannels();
    int ksmps = csoundGetKsmps(cs);
    int slices = getFramesPerBlock()/ksmps;
    // cout<<"nchnls "<< nchnls<<" slices "<< slices<<" ksmps "<<ksmps <<endl;
    // MYFLT *spout = csoundGetSpout(cs);
    //   float coef = (float) INT_MAX / csoundGet0dBFS(cs);
    for(int i=0; i < slices; i++){
        if(mCsData.useAudioInput)
        {
            // if(csoundPerformKsmps(cs)==0)
            for (int k = 0; k < nchnls; k++)
            {
                float *channel = buffer->getChannel( k );
                for(int j=0; j < ksmps ; j++)
                {
                    // csoundSetSpinSample(cs, j, k,(MYFLT)channel[j+i*ksmps]);
                    spin[j*nchnls+k] = channel[j+i*ksmps];
                }
            }
        }
        
        csoundPerformKsmps(cs);
        for (int k = 0; k < nchnls; k++)
        {
            //  buffer = (Float32 *) ioData->mBuffers[k].mData;
            float *channel = buffer->getChannel( k );
            
            if (mCsData.shouldMute == false) {
                for(int j=0; j < ksmps ; j++)
                {
                    channel[j+i*ksmps] = spout[j*nchnls+k] ;
                }
            } else {
                //   memset(channel, 0, sizeof(double) * buffer->getNumFrames()/buffer->getNumChannels());
            }
        }
    }
    
    
}


//-------------------------------------------------
void ciCsound::start()
{
    csThread->Play();
}
//-------------------------------------------------
void ciCsound::pause()
{
    csThread->Pause();
}
//-------------------------------------------------
void ciCsound::stop()
{
    csThread->Stop();
    delete csThread;
}

CSOUND* ciCsound::getCsound()
{
    if (!mCsData.running) {
        return NULL;
    }
    return mCsData.cs;
}
void ciCsound::listOrcChannels()
{
    controlChannelInfo_t* ctrlList;
    int chSz= csound->ListChannels(ctrlList);
    if(chSz) {
        cout<< "print channels list" <<endl;
        for(int i=0; i<chSz;i++)
        {
            cout<<  ctrlList[i].name <<endl;
            cout<<  ctrlList[i].type <<endl;
        }
    }
}

//-------------------------------------------------
void ciCsound::setChan(const char *name, double value)
{
    csound->SetChannel(name, value);
}
//-------------------------------------------------
MYFLT ciCsound::getChan(const char *name)
{
    MYFLT received_myflt;
    received_myflt = csound->GetChannel(name);
    return (received_myflt);
}
void ciCsound::getTable(double *&tablePtr,int tableNum)
{
    csound->GetTable(tablePtr, tableNum);
}
MYFLT * ciCsound::getAudio(const char *name)
{
    MYFLT *received_myflt;
    csound->GetAudioChannel(name, received_myflt);
    return received_myflt;
}
//-------------------------------------------------
void ciCsound::scoreEvent(char type, const double *pFields)
{	
    csound->ScoreEvent(type, pFields, sizeof(pFields));
}



