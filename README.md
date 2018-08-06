# cinderCsound
csound wrapper block for cinder

------------------ Info: ------------------

CsoundAPI wrapper Cinder block by Malfunkn

------------------ Install: ------------------

1.) Download and install Csound6: 
https://csound.com/download.html
2.) clone this repo into cinder/blocks folder


------------------ Usage: ------------------

1.) run the cinder:basic template in TinderBox and create new Xcode project
2.) drag the CsoundLib64.framework from Library/frameworks/ into Xcode
3.) drag libcsnd6.6.0.dylib from Library/frameworks/CsoundLib64.framework/Versions/6.0/ into Xcode 
4.) Done.



------------------ Notes: ------------------

 	- -+rtaudio is set to =null to pull MYFLT spout into cinder’s audio Node.
	(use -+rtaudio=coreaudio to directly connect with the system audio driver and -odac to select which driver)

Todo:
	- include the CsoundLib.framework to skip dragging operation
	- figure out msw support (.dll,.lib) equivalents
	- improve options setup, orchestra, and score modularity.

# Cinder-Csound
