#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxControlPanel.h"

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

        ofVideoPlayer 		    vidPlayer;

        ofxCvColorImage			colorImg;
    
        ofxCvGrayscaleImage 	grayImage;
        ofxCvGrayscaleImage 	grayBg;
        ofxCvGrayscaleImage 	grayDiff;
    
        ofxCvContourFinder 	contourFinder;
    
    
    // GUI
    ofxControlPanel gui;
    ofParameter <string> FPS;
    ofParameter <float> appFrameRate;
    
    ofParameterGroup statsCV;
    ofParameter <int> nBlobs;
    ofParameter <int> nCubes;
    
    // OpenCV controls
    ofParameterGroup cvControls;
    ofParameter <bool> bInvert;
    ofParameter <bool> bUseBackground;
    ofParameter <bool> bLearnBackground;
    ofParameter <int> threshold;

    void drawContourFinder(int x, int y, int w, int h);

    
};
