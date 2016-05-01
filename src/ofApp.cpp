#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    vidPlayer.load("ricochet_video_cube2.mp4");
    vidPlayer.play();
    vidPlayer.setLoopState(OF_LOOP_NORMAL);
    
    colorImg.allocate(320,240);
    grayImage.allocate(320,240);
    grayBg.allocate(320,240);
    grayDiff.allocate(320,240);
    
    //// GUI
    ofSetVerticalSync(true);
    
    gui.setup("Ricochet - DEBUG", 0, 0, ofGetWidth(), ofGetHeight());
    gui.addPanel("Ricochet - PARAMETERS", 4, false);
    
    // Footer
    FPS = " FPS : ";
    gui.setStatusMessage(FPS);

    
    //---------
    //--------- PANEL 1 : DEBUG
    gui.setWhichPanel(0);
    //--- Colomn1 :
    gui.setWhichColumn(0);
    gui.addDrawableRect("Render", &vidPlayer, 680, 480);

    //--- Colomn 2 : STATS
    gui.setWhichColumn(1);
    gui.addChartPlotter(appFrameRate, 30, 80);
    statsCV.setName("Stats");
    statsCV.add( nBlobs.set("Blobs founds", 0) );
    statsCV.add( nCubes.set("Cubes founds", 0) );
    gui.addVariableLister(statsCV);

    
    //---------
    //--------- PANEL 2 : PARAMS
    gui.setWhichPanel(1);
    
    //--- Colomn 1 : Video
    gui.setWhichColumn(0);
    gui.addDrawableRect("video", &vidPlayer, 200, 150);
    
    //--- Colomn 2 :OPENCV Render
    gui.setWhichColumn(1);
    gui.addDrawableRect("colorCV", &colorImg, 200, 150);
    gui.addDrawableRect("grayImg", &grayImage, 200, 150);
    gui.addDrawableRect("grayBg", &grayBg, 200, 150);

    
    //--- Colomn 3 :OPENCV Render 2
    gui.setWhichColumn(2);
    gui.addDrawableRect("grayDiff", &grayDiff, 200, 150);
    gui.addDrawableRect("contourFinder", &grayDiff, 200, 150);
    
    //--- Colomn 4 : Stats
    gui.setWhichColumn(3);
    
    cvControls.setName("cv controls");
    cvControls.add(bUseBackground.set("Use Bg comparator", true));
    cvControls.add(bLearnBackground.set("Update Bg", true));
    cvControls.add(bInvert.set("invert", false));
    cvControls.add(threshold.set("threshold", 29.0, 1.0, 255.0));
    gui.addGroup( cvControls );
    
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(100,100,100);
    
    // GUI UPDATE
    gui.update();
    appFrameRate = ofGetFrameRate();
    FPS = " FPS : " + ofToString(appFrameRate);

    // OPENCV UPDATE
    bool bNewFrame = false;
    
    vidPlayer.update();
    bNewFrame = vidPlayer.isFrameNew();
    nBlobs = contourFinder.nBlobs;
    
    if (bNewFrame){
        
        colorImg.setFromPixels(vidPlayer.getPixels());
        
        grayImage = colorImg;

        if (bLearnBackground){
            grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBackground = false;
        }
        
        if(bUseBackground) {
            // take the abs value of the difference between background and incoming and then threshold:
            grayDiff.absDiff(grayBg, grayImage);
        } else {
            grayDiff =  colorImg;
        }
        grayDiff.threshold(gui.getValueI("cv_controls:threshold"), bInvert);

        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayDiff, 20, (680*480), 10, true);	// find holes
    }
    

}

//--------------------------------------------------------------
void ofApp::draw(){
    gui.draw();
    
    switch(gui.getSelectedPanel()) {
        case 0:
            drawContourFinder(10,70,680,480);
            break;
        case 1:
            //TODO afficher le contourFinder
            break;
        default:
            break;
    }

    
}

//--------------------------------------------------------------
void ofApp::drawContourFinder(int x, int y, int w, int h) {
    ofNoFill();
    //ofDrawRectangle(x, y, w, h);
    // we could draw the whole contour finder
    contourFinder.draw(x, y, w, h);
    
    // or, instead we can draw each blob individually from the blobs vector,
    // this is how to get access to them:
    //    for (int i = 0; i < contourFinder.nBlobs; i++){
    //        contourFinder.blobs[i].draw(x, y);
    //
    //        // draw over the centroid if the blob is a hole
    //        ofSetColor(255);
    //        if(contourFinder.blobs[i].hole){
    //            ofDrawBitmapString("hole",
    //                               contourFinder.blobs[i].boundingRect.getCenter().x + x,
    //                               contourFinder.blobs[i].boundingRect.getCenter().y + y);
    //        }
    //    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){}

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
