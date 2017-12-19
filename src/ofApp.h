#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    void audioIn(float * input, int bufferSize, int nChannels);
    
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int 	bufferCounter;
    int 	drawCounter;
    
    float threshold;
    float decayRate;
    float minimumThreshold;
    float smoothedVol;
    float scaledVol;
    float lineDist;
    float speedVol;
    
    ofSoundStream soundStream;

    ofImage image;
    ofMesh mesh;
    ofMesh mesh2;
    
    vector<ofVec3f> offsets;
    
    bool resetOn;
//    
//    int nBandsToGet;
//    float * fftSmoothed;
//    
//    ofSoundPlayer sound;

};
