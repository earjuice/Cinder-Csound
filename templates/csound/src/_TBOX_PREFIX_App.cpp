
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#import "ciCsound.h"
#include "AudioDrawUtils.h"

#define NUM_AUDIO_INPUT 2

using namespace ci;
using namespace ci::app;
using namespace std;


class _TBOX_PREFIX_App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void keyDown(KeyEvent event ) override;
    void update() override;
    void draw() override;
    void play() ;
    void setupMultichannelDevice();
    
    ciCsoundRef csNode;
    audio::MonitorNodeRef mMonitor;
    
};

void _TBOX_PREFIX_App::setup()
{
    setupMultichannelDevice();
    auto ctx = audio::Context::master();
    audio::Node::Format nfrmt;
    nfrmt.channels(2);
    csNode=ctx->makeNode( new ciCsound( nfrmt ));
    audio::MonitorNode::Format mnfrmt;
    mnfrmt.autoEnable();
    mMonitor = ctx->makeNode( new audio::MonitorNode(audio::MonitorNode::Format().channels(NUM_AUDIO_INPUT) ) );
    csNode>>mMonitor;
    mMonitor>>ctx->getOutput();
    csNode->enable();
    mMonitor->enable();
    ctx->enable();
    
}
void _TBOX_PREFIX_App::setupMultichannelDevice()
{
    //console() << audio::Device::printDevicesToString() << endl;
    auto outputDevice = ci::audio::Device::getDefaultOutput();
    console() << "Output device "<<endl << outputDevice->getName() << " has "<< outputDevice->getNumOutputChannels() <<"channels: "  << endl;
    cout<<"Output device getFramesPerBlock "<<outputDevice->getFramesPerBlock() <<endl;
    cout<<"Output device getSampleRate "<<outputDevice->getSampleRate() <<endl;
    auto ctx = audio::master();
    audio::OutputDeviceNodeRef multichannelOutputDeviceNode = ctx->createOutputDeviceNode( outputDevice, audio::Node::Format().channels( outputDevice->getNumOutputChannels() ) );
    ctx->setOutput( multichannelOutputDeviceNode );
}
void _TBOX_PREFIX_App::play()
{
    double myArray[] = {1., 0., 1., 1.};
    csNode->scoreEvent('i', myArray);
}
void _TBOX_PREFIX_App::mouseDown( MouseEvent event )
{
    
}
void _TBOX_PREFIX_App::keyDown(KeyEvent event )
{
    double D6add9A[]{1, 0, 15, 0, 6.02, 1.7, 0.5};
    double D6add9B[]{1, 0, 15, 0, 6.09, 1.7, 0.5};
    double D6add9C[]{1, 0, 15, 0, 7.02, 1.7, 0.5};
    double D6add9G[]{1.f,  0, 15.f, 0.f, 7.04f, 1.7f, 0.5f};
    double D6add9D[]{1, 0, 15, 0, 7.06, 1.7, 0.5};
    double D6add9E[]{1, 0, 15, 0, 8.04, 1.1, 1.1};
    double D6add9F[]{1.f, 0,  15.f, 0.f, 6.11f, 1.7f, 0.5f};
    
    
    double myArrayC[] = {1, 0, 15, 0, 6.02, 1.7, 0.5};
    
    switch( event.getCode() ) {
        case KeyEvent::KEY_ESCAPE:
            csNode->stop();
            printf("Csound thread stopped.\n");
            quit();
            break;
        case KeyEvent::KEY_1:
            csNode->setChan("freq", 100);
            break;
        case KeyEvent::KEY_2:
            csNode->setChan("freq", 200);
            break;
        case KeyEvent::KEY_3:
            csNode->scoreEvent('i', myArrayC);
            break;
        case KeyEvent::KEY_4:
            csNode->scoreEvent('i', D6add9A);
            csNode->scoreEvent('i', D6add9B);
            csNode->scoreEvent('i', D6add9C);
            csNode->scoreEvent('i', D6add9D);
            break;
        case KeyEvent::KEY_5:
            csNode->scoreEvent('i', D6add9E);
            csNode->scoreEvent('i', D6add9B);
            csNode->scoreEvent('i', D6add9C);
            csNode->scoreEvent('i', D6add9D);
            break;
        case KeyEvent::KEY_6:
            csNode->scoreEvent('i', D6add9F);
            csNode->scoreEvent('i', D6add9B);
            csNode->scoreEvent('i', D6add9A);
            csNode->scoreEvent('i', D6add9D);
            break;
        case KeyEvent::KEY_7:
            csNode->scoreEvent('i', D6add9G);
            csNode->scoreEvent('i', D6add9B);
            csNode->scoreEvent('i', D6add9C);
            csNode->scoreEvent('i', D6add9D);
            break;
        case KeyEvent::KEY_8:
            csNode->scoreEvent('i', D6add9G);
            csNode->scoreEvent('i', D6add9A);
            break;
        case KeyEvent::KEY_9:
            csNode->scoreEvent('i', D6add9F);
            csNode->scoreEvent('i', D6add9C);
            break;
        case KeyEvent::KEY_s:
            csNode->start();
            break;
        case KeyEvent::KEY_SPACE:
            csNode->pause();
            break;
    }
    
}

void _TBOX_PREFIX_App::update()
{
}

void _TBOX_PREFIX_App::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    Rectf rect=Rectf(getWindowBounds());
    drawAudioBuffer( mMonitor->getBuffer(), rect, 0,false,Color(1.,1.,1.) );
}

CINDER_APP( _TBOX_PREFIX_App, RendererGl )
