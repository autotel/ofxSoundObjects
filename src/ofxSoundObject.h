#pragma once

#include "ofConstants.h"
#include "ofBaseTypes.h"
#include "ofSoundBuffer.h"
#include "ofxSoundObjectsConstants.h"
#include "ofSoundStream.h"
// this #define is for use by addon writers, to conditionally support sound objects in addons
// (while allowing backwards compatibility with previous versions of openFrameworks)
#define OF_SOUND_OBJECT_AVAILABLE

//--------------------------------------------------------------
//  ofxSoundObject
//--------------------------------------------------------------
/**
 * ofxSoundObject is a node in your dsp chain. It can have one input,
 * and one output. If it doesn't have an input, it's the beginning
 * of the dsp chain (i.e. an oscillator or a microphone input).
 */
class ofSoundBuffer;
class ofxSoundInputMultiplexer;

class ofxSoundObject: public ofBaseSoundOutput, public ofBaseSoundInput {
public:
	ofxSoundObject();
	ofxSoundObject(ofxSoundObjectsType);
	virtual ~ofxSoundObject() {
		disconnect();
	}

	friend class ofxSoundInputMultiplexer; 
	
	virtual std::string getName() {return "ofxSoundObject";}
	/// Connects the output of this ofxSoundObject to the input of the parameter ofxSoundObject
	ofxSoundObject &connectTo(ofxSoundObject &soundObject);
    void disconnect();
	virtual void disconnectInput(ofxSoundObject * input);

	/// This is the method you implement to process the signal from inputs to outputs.
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) {
		// default behaviour is pass-through.
		input.copyTo(output);
	}

	/// this pulls the audio through from earlier links in the chain.
	/// you can override this to add more interesting functionality
	/// like signal splitters, sidechains etc.
	virtual void audioOut(ofSoundBuffer &output) override;

	virtual void audioIn(ofSoundBuffer &input) override;
	
	
	/// this checks the dsp chain to ensure there are no infinite loops
	/// - might want to override this if you make a splitter
	/// returns true if there are no infinite loops.
	virtual bool checkForInfiniteLoops();
	ofxSoundObject *getInputObject();
	
	ofxSoundObject *getOutputObject();
	///this returns the object that is at the begining of the objects chain. It should be an audio input, a sound file player or some kind of signal generator, like a synth.
	ofxSoundObject * getSignalSourceObject();
	
	
	///this returns the object that is at the end of the objects chain. It should be an audio output or a sound file writer
	ofxSoundObject * getSignalDestinationObject();
	
	
	
	
    /// This gets the number of channels that this sound object should process.
    /// By default it will use the number of channels from the ofSoundBuffer passed by the previous link in the chain.
	virtual size_t getNumChannels();

    ofSoundBuffer& getBuffer();
    const ofSoundBuffer& getBuffer() const;
	
	
	ofxSoundObjectsType getType(){return type;}
	
	
	
	void setOutputStream(ofSoundStream& stream);
	void setOutputStream(ofSoundStream* stream);
	ofSoundStream* getOutputStream();
	
	void setInputStream(ofSoundStream& stream);
	void setInputStream(ofSoundStream* stream);
	ofSoundStream* getInputStream();

	/// Returns info about the device that is connected to this output
	virtual ofSoundDevice getInputDeviceInfo();
	virtual ofSoundDevice getOutputDeviceInfo();
	
	/// Returns the device ID that is connected
	virtual int getOutputDeviceId();
	virtual int getInputDeviceId();
	
	
	std::string getSignalFlowModeAsString(){
		switch(signalFlowMode){
			case OFX_SOUND_OBJECT_PULL: return "OFX_SOUND_OBJECT_PULL";
			case OFX_SOUND_OBJECT_PUSH: return "OFX_SOUND_OBJECT_PUSH";
		}
		return "";
	}
	
protected:

	// this is the previous dsp object in the chain
	// that feeds this one with input.
	ofxSoundObject *inputObject = nullptr;
    ofxSoundObject *outputObject = nullptr;
    virtual void setInput(ofxSoundObject *obj);
	
	ofxSoundObjectsType type = OFX_SOUND_OBJECT_PROCESSOR;
	
	ofxSoundObjectsChannelCountModifier chanMod = OFX_SOUND_OBJECT_CHAN_UNCHAGED; 
	 
	
private:
	ofSoundStream* inputStream = nullptr;
	ofSoundStream* outputStream = nullptr;
	
	// a spare buffer to pass from one sound object to another
	ofSoundBuffer workingBuffer;
	ofSoundBuffer inputBuffer;
	
	enum ofxSoundObjectsMode{
		//This is the default mode 
		OFX_SOUND_OBJECT_PULL = 0,
		OFX_SOUND_OBJECT_PUSH,
//		OFX_SOUND_OBJECT_INDEPENDENT,
//		OFX_SOUND_OBJECT_OFFLINE
	} signalFlowMode = OFX_SOUND_OBJECT_PULL;

	void setSignalFlowMode(const  ofxSoundObjectsMode & newMode);
	void checkSignalFlowMode();
	
	
};

//--------------------------------------------------------------
//  ofxSoundInput
//--------------------------------------------------------------
/**
 * This class represents input from the sound card in your dsp chain.
 */
class ofxSoundInput: public ofxSoundObject {
public:
	ofxSoundInput():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE) {}
	virtual size_t getNumChannels() override;
	virtual void audioIn(ofSoundBuffer &input) override;
	virtual void audioOut(ofSoundBuffer &output) override;

protected:
	ofSoundBuffer inputBuffer;
	
};

/**
 * This class represents the output in your dsp chain.
 */
class ofxSoundOutput: public ofxSoundObject {
public:
	ofxSoundOutput():ofxSoundObject(OFX_SOUND_OBJECT_DESTINATION) {}
//	virtual size_t getNumChannels() override;
	
};


