/*
 Copyright (c) 2014, The Cinder Project
 
 This code is intended to be used with the Cinder C++ library, http://libcinder.org
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */
#include "AudioDrawUtils.h"


using namespace std;
using namespace ci;


void drawAudioBuffer( const audio::Buffer &buffer, const Rectf &bounds,int channel,  bool drawFrame, const ci::ColorA &color )
{
    gl::ScopedGlslProg glslScope( getStockShader( gl::ShaderDef().color() ) );
    
    if( drawFrame ) {
        gl::color( color.r, color.g, color.b, color.a * 0.3f );
        float reBoundWidth = bounds.getWidth();
        
        for(int i = 1; i<8;i++){
            gl::drawLine(vec3(reBoundWidth*0.125*i+bounds.x1,bounds.y1 ,0),  vec3(reBoundWidth*0.125*i+bounds.x1, bounds.y2,0 ));
        }
        gl::drawLine(vec3(bounds.x1,bounds.getHeight() * 0.25 + bounds.y1 ,0),  vec3(bounds.x2, bounds.getHeight() * 0.25 + bounds.y1,0 ));
        gl::drawLine(vec3(bounds.x1,bounds.getHeight() * 0.75 + bounds.y1 ,0),  vec3(bounds.x2, bounds.getHeight() * 0.75 + bounds.y1,0 ));
        
        gl::color( 0.0f,0.0f,0.0f, 0.2f );
        gl::drawSolidRoundedRect( bounds, (bounds.getHeight()*0.1), 4 );
        
        gl::color( color.r, color.g, color.b, color.a * 0.6f );
        gl::drawStrokedRoundedRect( bounds, (bounds.getHeight()*0.1), 4 );
        
        
    }
    
    gl::color( color );
    gl::lineWidth( 4.0f );
    
    const float waveHeight = bounds.getHeight();
    
    const float xScale = bounds.getWidth() / ((float)buffer.getNumFrames()*0.25);
    
    float yOffset = bounds.y1;
    for( size_t ch = 0; ch < buffer.getNumChannels(); ch++ ) {
        if(ch == channel){
            PolyLine2f waveform;
            
            const float *channel = buffer.getChannel( ch );
            float x = bounds.x1;
            for( size_t i = 0; i < buffer.getNumFrames(); i=i+4 ) {
                x += xScale;
                float y = ( 1 - ( channel[i] + 0.5f ) ) * waveHeight + yOffset;
                
                waveform.push_back( vec2( x, y ) );
            }

            
            if( ! waveform.getPoints().empty() ){
                gl::draw( waveform );
                
                gl::pushMatrices();
                
                gl::draw( waveform );
                
                gl::popMatrices();
                
            }
            
            yOffset += waveHeight;
        }
    }
    gl::lineWidth( 1.0f );
    
    
}
