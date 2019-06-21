//
//  ofxSoundObjectMatrixMixerRenderer.cpp
//  example-matrixMixer
//
//  Created by Roy Macdonald on 5/4/19.
//
//

#include "ofxSoundObjectMatrixMixerRenderer.h"
#include "ofxSoundPlayerObject.h"
#include "ofMain.h"
#include "ofxGui.h"

//----------------------------------------------------

void drawRect(const ofRectangle& r){
	ofSetColor(55);
	ofNoFill();
	ofDrawRectangle(r);
	ofFill();
	ofSetColor(0);
	ofDrawRectangle(r);
}
std::string getSoundFileInfo(const ofxSoundFile& f){
std:stringstream ss;
	ss << ofFilePath::getBaseName(f.getPath())  << std::endl;
	ss << "  Duration " << f.getDuration() << std::endl;
	ss << "  NumChannels " << f.getNumChannels() << std::endl;
	ss << "  SampleRate " << f.getSampleRate() << std::endl;
	ss << "  NumSamples " << f.getNumSamples();
	return ss.str();
}

//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::initOrResizeNumSliders(const float & sliderWidth){
	if(sliders.size() != obj->inObjects.size())sliders.resize(obj->inObjects.size());
	for(size_t idx =0 ; idx < obj->inObjects.size(); idx++ ){
		auto & v = obj->inObjects[idx]->channelsVolumes;
		if(sliders[idx].size() != v.size()) sliders[idx].resize(v.size());
		for(size_t i = 0; i <sliders[idx].size(); i++){
			if(sliders[idx][i].size() != v[i].size()) sliders[idx][i].resize(v[i].size());
			for(size_t o = 0; o < sliders[idx][i].size(); o++){
				if(!sliders[idx][i][o]){
					sliders[idx][i][o] = make_unique<ofxFloatSlider>(v[i][o], sliderWidth);
					bSlidersEnabled = true;
				}
			}
		}
	}
	
	if(outputSliders.size() != obj->getNumOutputChannels()) outputSliders.resize(obj->getNumOutputChannels());
	for(size_t i = 0; i < outputSliders.size(); i++){
		if(!outputSliders[i]) outputSliders[i] = make_unique<ofxFloatSlider>(obj->outputChannels[i].volume, sliderWidth);
	}
	if(!bMasterSliderSetup){
		masterSlider.setup(obj->masterVol);;
		bMasterSliderSetup = true;
	}
}

//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::enableSliders(){
	if(!bSlidersEnabled){
		bSlidersEnabled = true;
		for(auto& s: sliders){
			for(auto& i: s){
				for(auto& o: i){
					if(o){
						o->registerMouseEvents();
					}
				}
			}
		}
	}
	for(auto& o: outputSliders ){
		if(o) o->registerMouseEvents();
	}
	masterSlider.registerMouseEvents();
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::disableSliders(){
	if(bSlidersEnabled){
		bSlidersEnabled = false;
		for(auto& s: sliders){
			for(auto& i: s){
				for(auto& o: i){
					if(o){
						o->unregisterMouseEvents();
					}
				}
			}
		}
	}
	for(auto& o: outputSliders ){
		if(o) o->unregisterMouseEvents();
	}
	masterSlider.unregisterMouseEvents();	
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::toggleSliders(){
	if(bSlidersEnabled){
		disableSliders();
	}else{
		enableSliders();
	}
}
//----------------------------------------------------
bool ofxSoundMatrixMixerRenderer::isSlidersEnabled(){
	return bSlidersEnabled;
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::setNonSliderMode(bool b){
	bNonSliderMode = b;
	if(bNonSliderMode) disableSliders();
}
//----------------------------------------------------
bool ofxSoundMatrixMixerRenderer::isNonSliderMode(){
	return bNonSliderMode;
}
void vecToString(std::stringstream & ss, std::vector<size_t>& vec){
	
	for(auto& v:vec){
		ss << v << ", ";
	}
	ss << std::endl;
}
void ofxSoundMatrixMixerRenderer::setMinChannelHeight(const float& minHeight){
	this->minHeight = minHeight;
}
//----------------------------------------------------
void ofxSoundMatrixMixerRenderer::draw(const ofRectangle& mixerRect){
	
	if(obj != nullptr){
		
			
		leftR.set (mixerRect.x, mixerRect.y, leftW, mixerRect.height - bottomH);
		bottomR.set(leftR.getMaxX(), leftR.getMaxY(), mixerRect.width - leftR.width,  bottomH);
		gridR.set (leftR.getMaxX(), leftR.getMinY(), bottomR.width, leftR.height);
		bottomLeftR.set(leftR.x, bottomR.y, leftR.width, bottomR.height);
		
		
		
		ofRectangle cell;
		cell.x = gridR.getMaxX()  - chanW;
		cell.y = gridR.y;
		cell.width = gridR.width/ obj->getNumOutputChannels();
//		cell.height = gridR.height/ obj->getNumInputChannels();
		cell.height = std::max(minHeight, gridR.height/ obj->getNumInputChannels());  
		
		bool bDisableTextRendering = (cell.height < 16.0f);
		
		
		
		//TO-DO this logic needs some love 
//		if(bDisableTextRendering){
//			if(bSlidersEnabled){
//				bSlidersWereEnabled = true;
//				setNonSliderMode(true);
//			}
//		}else if(bSlidersWereEnabled && !bSlidersEnabled){
//			enableSliders();
//		}
		
		
		drawRect(leftR);
		drawRect(bottomR);
		drawRect(gridR);
		drawRect(bottomLeftR);
		
		size_t cellCount = 0;
		ofBitmapFont bf;
		ofRectangle objR(leftR.x, leftR.y, leftW - chanW, leftR.height / obj->inObjects.size());
		ofRectangle chanR(objR.getMaxX(), objR.y, chanW, cell.height);
		
		initOrResizeNumSliders(cell.width);
		
		
		// start draw output channels 
		ofRectangle outChanR = bottomR;
		outChanR.width = bottomR.width/outputSliders.size();
		glm::vec3 outPos = outChanR.getBottomLeft();
		if(outputSliders.size()) outPos.y -= outputSliders[0]->getHeight();
		for(size_t i = 0; i < outputSliders.size() && i < obj->outputChannels.size() ; i++){
			drawRect(outChanR);
			if(bNonSliderMode){
				std::stringstream vol;
				vol << "out " << i <<" : " << obj->outputChannels[i].volume.get();
				ofDrawBitmapString(vol.str(), outPos.x, outPos.y - 3);
			}else{
				if(outputSliders[i]!= nullptr){			
					if(outputSliders[i]->getPosition() != outPos){
						outputSliders[i]->setPosition(outPos);
					}	
					outputSliders[i]->draw();
				}
			}
			outPos.x += outChanR.width;
			
			if(ofxSoundMatrixMixer::getComputeRMSandPeak()){
				auto vuR = outChanR;
				vuR.y = outChanR.getMaxY();
				if(outputSliders[i]) vuR.y -= outputSliders[i]->getHeight(); 
				
				vuR.y -= chanW;
				vuR.height = chanW;
				obj->outVuMeter.drawChannel(i, vuR);
			}
			
			outChanR.x += outChanR.width;
		}
		
		
		auto mp = bottomLeftR.getBottomLeft();
		mp.y -= masterSlider.getHeight();
		if(bNonSliderMode){
			std::stringstream vol;
			vol << "Master  " << obj->masterVol.get();
			ofDrawBitmapString(vol.str(), mp.x, mp.y - 3);
		}else{
			if(masterSlider.getPosition() != mp){
				masterSlider.setPosition(mp);
			}
			masterSlider.draw();
		}		
		
		
		std::stringstream oss;
		oss << " -- OUTPUT -- " <<std::endl;
		auto * dest = obj->getSignalDestinationObject();
		if(dest){
			auto info = dest->getOutputDeviceInfo();
			oss << info.name;// <<std::endl;
		}
		ofSetColor(255);
		ofDrawBitmapString(oss.str(), bottomLeftR.x, bottomLeftR.y + 20);
		
		
		// end draw output channels
		
		// start draw grid
		for(size_t idx =0 ; idx < obj->inObjects.size(); idx++ ){
			auto & v = obj->inObjects[idx]->channelsVolumes;
			
			objR.height = cell.height * v.size();
			chanR.y = objR.y;
			
			drawRect(objR);			
			ofSetColor(255);
			
			
			
			auto sgnlSrc = obj->inObjects[idx]->obj->getSignalSourceObject();
			auto player = dynamic_cast<ofxSoundPlayerObject*>(sgnlSrc);
			
			// input object info header rect draw start
			if(player){
				if(!bDisableTextRendering){
					auto info = getSoundFileInfo(player->getSoundFile());
					auto bb = bf.getBoundingBox(info, 0,0);
					ofDrawBitmapString(info, objR.x - bb.x, objR.y - bb.y);
				}
				ofRectangle posR = objR;
				posR.height = std::min(cell.height, 10.0f);
				posR.y = objR.getMaxY() - posR.height;
				posR.width = ofMap(player->getPositionMS(), 0, player->getDurationMS(), 0, objR.width);
				
				ofSetColor(100);
				ofDrawRectangle(posR);
				
				
			}else{
				if(!bDisableTextRendering){
					auto liveScr = dynamic_cast<ofxSoundInput*>(sgnlSrc);
					if(liveScr){
						ofDrawBitmapString(ofxSoundUtils::getSoundDeviceString(liveScr->getInputDeviceInfo(), true, false), objR.x, objR.getMinY()+20);
					}else{
						ofDrawBitmapString(ofToString(v.size()), objR.x, objR.getMaxY() - 20);
					}
				}
			}
			// input object info header rect draw end
			
			// draw cells start
			objR.y += objR.height;
			ofRectangle rmsR;
			for(size_t i = 0; i < v.size() && i < sliders[idx].size(); i++){
				cell.y = gridR.y + (cell.height * (i + cellCount));
				chanR.y = cell.y;
				drawRect(chanR);
				if(obj->ofxSoundMatrixMixer::getComputeRMSandPeak()){
					obj->inObjects[idx]->vuMeter.drawChannel(i, chanR);
				}
				for(size_t j = 0; j < v[i].size() && j < sliders[idx][i].size(); j++){
					cell.x = gridR.x + cell.width*j; 	 
					drawRect(cell);
					ofSetColor(255);
					
					auto p = cell.getBottomLeft();
					p.y -= 3;// just add a tiny margin
					if(bDisableTextRendering){
						ofRectangle volR = cell;
						volR.width = ofMap(v[i][j], 0,1,0, volR.width, true);
						
						ofSetColor(100);
						ofDrawRectangle(volR);
					}else{
						if(bNonSliderMode){
							std::stringstream vol;
							vol << "[ " << cellCount+ i << ":" << j <<" ]  " << v[i][j];
							ofDrawBitmapString(vol.str(), p.x, p.y);						
						}else{
							if(sliders[idx][i][j]){
								p.y -= sliders[idx][i][j]->getHeight();
								if(sliders[idx][i][j]->getPosition() != p){
									sliders[idx][i][j]->setPosition(p);
								}
								sliders[idx][i][j]->draw();
							}
						}
					}
				}
			}
			cellCount += v.size();
			// draw cells end
			
		}
		
		std::stringstream ss;
		
		ss << "Num Output Channels : " << obj->getNumOutputChannels() << std::endl;
		ss << "Num Input Channels  : " << obj->getNumInputChannels() << std::endl;
		ss << "Num Input Objects   : " << obj->inObjects.size() << std::endl;
		
		//		vecToString(ss, obj->matrixInputChannelMap);
		//		vecToString(ss, obj->numConnectionInputChannels);
		//		vecToString(ss, obj->connectionFirstChannel);
		ofDrawBitmapStringHighlight(ss.str(), 20, 20);
	}
}
