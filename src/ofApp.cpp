#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    vidPlayer.load("videoKinectCapture.mp4");
    vidPlayer.play();
    vidPlayer.setLoopState(OF_LOOP_NORMAL);
    
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
    gui.addDrawableRect("Render", &vidPlayer, WIDTH, HEIGHT);

    //--- Colomn 2 : STATS
    gui.setWhichColumn(1);
    gui.addChartPlotter(appFrameRate, 30, 80);
    statsCV.setName("Stats");
    statsCV.add( nBlobs.set("Blobs founds", 0) );
    statsCV.add( nCubes.set("Cubes founds", 0) );
    gui.addVariableLister(statsCV);
    
    viewControl.setName("Debugs");
    viewControl.add(bShowContour.set("Show controls", true));
    gui.addGroup(viewControl);

    
    //---------
    //--------- PANEL 2 : PARAMS
    gui.setWhichPanel(1);
    
    //--- Colomn 1 : Kinect render
    gui.setWhichColumn(0);
    gui.addDrawableRect("Kinect Video", &vidPlayer, OC_WIDTH, OC_HEIGHT);
    gui.addDrawableRect("Kinect Depth", &vidPlayer, OC_WIDTH, OC_HEIGHT);
    
    //--- Colomn 2 :OPENCV Render
    gui.setWhichColumn(1);
    gui.addDrawableRect("OpenCV colorRender", &colorImg, OC_WIDTH, OC_HEIGHT);
    gui.addDrawableRect("OpenCV grayRender", &grayImage, OC_WIDTH, OC_HEIGHT);
    gui.addDrawableRect("OpenCV grayBackground", &grayBg, OC_WIDTH, OC_HEIGHT);

    
    //--- Colomn 3 :OPENCV Render 2
    gui.setWhichColumn(2);
    gui.addDrawableRect("OpenCV grayReworked", &grayDiff, OC_WIDTH, OC_HEIGHT);
    gui.addDrawableRect("OpenCV result", &grayRender, OC_WIDTH, OC_HEIGHT);
    
    //--- Colomn 4 : Stats
    gui.setWhichColumn(3);
    
    gui.addVariableLister(statsCV);
    
    buttonControls.setName("Buttons");
    buttonControls.add(bPlay.set("Play/Pause", true));
    buttonControls.add(bLearnBackground.set("Update Background", true));
    gui.addGroup(buttonControls);
    
    reworkControls.setName("OpenCV grayReworked");
    reworkControls.add(bUseBackground.set("Compart with Bg", true));
    reworkControls.add(bContrast.set("contrastStretch", true));
    reworkControls.add(bBlur.set("blurHeavily", false));
    reworkControls.add(minRange.set("minRange", 0.0, 0.0, 255.0));
    reworkControls.add(maxRange.set("maxRange", 255.0, 0.0, 255.0));
    reworkControls.add(brightness.set("brightness", 0.0, 0.0, 3.0));
    reworkControls.add(contrast.set("contrast", 0.0, 0.0, 3.0));
    gui.addGroup( reworkControls );
    
    resultControls.setName("OpenCV result");
    resultControls.add(nearThreshold.set("nearThreshold", 152.0, 1.0, 255.0));
    resultControls.add(farThreshold.set("farThreshold", 145.0, 1.0, 255.0));
    resultControls.add(minArea.set("minArea", 800.0, 1.0, 3000.0));
    resultControls.add(maxArea.set("maxArea", 3000.0, 1.0, (OC_WIDTH*OC_HEIGHT)));
    resultControls.add(nBlobMax.set("nBlobMax", 10.0, 1.0, 100.0));
    gui.addGroup(resultControls);


    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(100,100,100);
    
    // GUI UPDATE
    gui.update();
    appFrameRate = ofGetFrameRate();
    FPS = " FPS : " + ofToString(appFrameRate);
    
    // VIDEO UPDATE
    bool bNewFrame = true;
    if(bPlay) {
        vidPlayer.update();
        bNewFrame = vidPlayer.isFrameNew();
    }

    // OPENCV UPDATE
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
        
        // EFFECTS
        // convertToRange() useless.
        grayDiff.convertToRange(gui.getValueI("OpenCV_grayReworked:minRange"),
                                gui.getValueI("OpenCV_grayReworked:maxRange"));
        // ContrastStretch
        if (bContrast) grayDiff.contrastStretch();
        // Brightness/Contrast useless.
        grayDiff.brightnessContrast(gui.getValueI("OpenCV_grayReworked:brightness"),
                                    gui.getValueI("OpenCV_grayReworked:contrast"));
        // blurHeavily()
        if(bBlur) grayDiff.blurHeavily();
        
        grayThreshNear = grayDiff;
        grayThreshFar = grayDiff;
        grayRender = grayDiff;
        grayThreshNear.threshold(gui.getValueI("OpenCV_result:nearThreshold"), true);
        grayThreshFar.threshold(gui.getValueI("OpenCV_result:farThreshold"));
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayRender.getCvImage(), NULL);
        
        // Render
        //grayRender = grayDiff;
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayRender,
                                   gui.getValueI("OpenCV_result:minArea"),
                                   gui.getValueI("OpenCV_result:maxArea"),
                                   gui.getValueI("OpenCV_result:nBlobMax"),
                                   false);	// find holes
    }
    

}

//--------------------------------------------------------------
void ofApp::draw(){
    gui.draw();
    
    switch(gui.getSelectedPanel()) {
        case 0:
            if(bShowContour) drawContourFinder(10.0, 70.0, WIDTH, HEIGHT);
            break;
        case 1:
            drawContourFinder(443.0, 247.0, OC_WIDTH, OC_HEIGHT);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::drawContourFinder(float x, float y, float w, float h) {
    ofNoFill();
    ofDrawRectangle(x, y, w, h);
    // we could draw the whole contour finder
    contourFinder.draw(x, y, w, h);
    
    // or, instead we can draw each blob individually from the blobs vector,
    // this is how to get access to them:
//    for(int i = 0; i < contourFinder.nBlobs; i++) {
//        ofRectangle r = contourFinder.blobs.at(i).boundingRect;
//        r.x = x + (w*r.x)/320;
//        r.y = y + (h*r.y)/240;
//        r.width =  (w*r.width)/320;
//        r.height = (h*r.height)/240;
//        
//        ofSetColor(255);
//        ofDrawRectangle(r);
//        
//        // TODO Analyse blob
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
