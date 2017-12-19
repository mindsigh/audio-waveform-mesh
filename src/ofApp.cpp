#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(20);
    ofEnableAlphaBlending();
    ofSetFrameRate(60);
    ofSetBackgroundAuto(false);
    
    resetOn = true;
    //audio
    int bufferSize = 256;
    
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    volHistory.assign(400, 0.0);
    
    bufferCounter	= 0;
    drawCounter		= 0;
    smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
    
    decayRate = 0.1;
    minimumThreshold = 0.01;
    threshold = minimumThreshold;
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    
    //mesh
    mesh.setMode(OF_PRIMITIVE_POINTS);
    mesh.enableColors();
    mesh.enableIndices();
    
    mesh2.setMode(OF_PRIMITIVE_LINES);
    mesh2.enableColors();
    mesh2.enableIndices();

    //image
    image.load("http://78.media.tumblr.com/b73d2ca20347358353a0e3f4a57c11d4/tumblr_ozmz3imyji1vesf5to1_1280.jpg");
    image.resize(25, 25);
    
    float intensityThreshold = 100.0;
    int w = image.getWidth();
    int h = image.getHeight();
    for (int x=0; x<w; ++x) {
        for (int y=0; y<h; ++y) {
                ofColor c = image.getColor(x, y);
                float intensity = c.getLightness();
                if (intensity >= intensityThreshold) {
                    float saturation = c.getSaturation();
                    float z = ofMap(saturation, 0, 255, -200, 200);
                    ofVec3f pos(x*5,y*5,z);
                    
                    c.a = saturation;
                    mesh.addColor(c);
                    
                    offsets.push_back(ofVec3f(ofRandom(0,100000), ofRandom(0,100000), ofRandom(0,100000)));
            }
        }
    }
    
    for (int x=0; x<w; ++x) {
        for (int y=0; y<h; ++y) {
            ofColor c = image.getColor(x, y);
            float intensity = c.getLightness();
            if (intensity >= intensityThreshold) {
                float saturation = c.getSaturation();
                float z = ofMap(saturation, 0, 255, -200, 200);
                ofVec3f pos(x*5,y*5,z);
                //mesh2.addVertex(pos);

                c.a = saturation;
                
                mesh2.addColor(c);
                
                offsets.push_back(ofVec3f(ofRandom(0,100000), ofRandom(0,100000), ofRandom(0,100000)));
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    for (unsigned int i = 0; i < left.size(); i++){
        float x =  ofMap(i, 0, left.size(), 0, ofGetWidth(), true);
        mesh.addVertex(ofVec3f(x, left[i]*180.0f, 0));
        if (mesh.getNumVertices() > 300) {
            mesh.removeVertex(0);
        }
    }
    
    
    for (unsigned int i = 0; i < left.size(); i++){
        float x =  ofMap(i, 0, left.size(), 0, ofGetWidth(), true);
        mesh2.addVertex(ofVec3f(x, left[i]*180.0f, 0));
        if (mesh2.getNumVertices() > 300) {
            mesh2.removeVertex(0);
        }
    }
    
    int numVerts = mesh.getNumVertices();
    int numVerts2 = mesh2.getNumVertices();

    float connectionDistance = 10;
    for (int a=0; a<numVerts; ++a) {
        ofVec3f verta = mesh.getVertex(a);
        for (int b=a+1; b<numVerts; ++b) {
            ofVec3f vertb = mesh.getVertex(b);
            float distance = verta.distance(vertb);
            if (distance <= connectionDistance) {
                if (threshold>minimumThreshold) {
                    mesh.addIndex(a);
                    mesh.addIndex(b);
                }
            }
        }
    }

    if (mesh.getNumIndices() > 200) mesh.removeIndex(0);

    for (int a=0; a<numVerts2; ++a) {
        ofVec3f verta = mesh2.getVertex(a);
        for (int b=a+1; b<numVerts2; ++b) {
            ofVec3f vertb = mesh2.getVertex(b);
            float distance = verta.distance(vertb);
            for (int i = 0; i < 256; i ++) {
                float connectionDistance2 = left[i] * 20;
                if (distance <= connectionDistance2) {
                    mesh2.addIndex(a);
                    mesh2.addIndex(b);
            }
            }
        }
    }

    if (mesh2.getNumIndices() > 100) mesh2.removeIndex(0);
    
    if (resetOn) {
        if (threshold < 0.1) mesh2.clearIndices();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(20,10);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    if (threshold > .12) {
        ofPushMatrix();
        ofTranslate(0, ofGetHeight()/2);
        mesh2.draw();
        ofPopMatrix();

    }

    ofPushMatrix();
    ofTranslate(0, ofGetHeight()/2);
    
        mesh.draw();
    ofPopMatrix();
    
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(0, ofGetHeight()/2, 0);
    ofNoFill();
    ofSetColor(225);

    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);

    ofPopMatrix();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    float curVol = 0.0;
    
    int numCounted = 0;
    
    for (int i = 0; i < bufferSize; i++) {
        left[i]		= input[i*2]*1.8;
        right[i]	= input[i*2+1]*1.8;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    curVol /= (float)numCounted;
    
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    bufferCounter++;
    
    threshold = ofLerp(threshold, minimumThreshold, decayRate);
    
    if(curVol > threshold) {
        threshold = curVol;
    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==' ') {
        if (resetOn) {
            resetOn = false;
    } else {
        resetOn = true;
    }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
