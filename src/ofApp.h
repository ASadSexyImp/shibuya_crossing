#pragma once

#include "ofMain.h"
#include <algorithm> // これを追加して min/max エラーを回避

class Agent {
public:
    ofVec2f pos, vel, target;
    float maxSpeed;
    ofColor color;
    float size;
    bool isMoving; 
    
    void setup(ofVec2f startPos);
    void update(bool isGreen, const vector<Agent>& others, int index, float centerRadius);
    void draw();
};

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void mousePressed(int x, int y, int button);
    
    void drawCyberCrosswalk(float x1, float y1, float x2, float y2, float width);
    void drawWireframeBuilding(float x, float y, float w, float h, ofColor c);
    void drawNeonSign(float x, float y, string text, ofColor c, bool vertical=false);

    vector<Agent> agents;
    bool isGreen;
    float signalTimer;
    float roadWidth;
    float centerRadius;
};