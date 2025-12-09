#include "ofApp.h"

// --- Agentクラス ---
void Agent::setup(ofVec2f startPos) {
    pos = startPos;
    target = startPos;
    isMoving = false;
    vel.set(0, 0);
    
    float r = ofRandom(1.0);
    if (r < 0.1) {
        color = ofColor(255, 0, 100); // Neon Pink
        maxSpeed = ofRandom(3.0, 4.5);
        size = 4;
    } else if (r < 0.3) {
        color = ofColor(0, 255, 255); // Cyan
        maxSpeed = ofRandom(2.5, 3.5);
        size = 3.5;
    } else if (r < 0.4) {
        color = ofColor(255, 255, 0); // Yellow
        maxSpeed = ofRandom(1.5, 2.5);
        size = 4;
    } else {
        color = ofColor(200, 200, 255); // White-Blue
        maxSpeed = ofRandom(2.8, 3.8);
        size = 3;
    }
}

void Agent::update(bool isGreen, const vector<Agent>& others, int index, float centerRadius) {
    ofVec2f acc(0, 0);
    float distToCenter = pos.distance(ofVec2f(ofGetWidth()/2, ofGetHeight()/2));
    
    bool shouldMove = false;
    if (isGreen) {
        shouldMove = true;
    } else {
        if (distToCenter < centerRadius) shouldMove = true;
    }
    
    if (shouldMove) {
        ofVec2f desired = target - pos;
        float d = desired.length();
        
        if (d > 10) {
            desired.normalize();
            desired *= maxSpeed;
            ofVec2f steer = desired - vel;
            acc += steer * 0.1; 
        }
        
        // --- 修正箇所 1: std::max / std::min を使用 ---
        int range = 30; 
        int startI = std::max(0, index - range);
        int endI = std::min((int)others.size(), index + range);
        
        int count = 0;
        ofVec2f sepSum(0,0);
        float sepDist = 20.0;
        
        for(int i=startI; i<endI; i++){
            if(i == index) continue;
            float dist = pos.distance(others[i].pos);
            if(dist > 0 && dist < sepDist) {
                ofVec2f diff = pos - others[i].pos;
                diff.normalize();
                diff /= dist; 
                sepSum += diff;
                count++;
            }
        }
        
        if(count > 0) {
            sepSum /= count;
            sepSum.normalize();
            sepSum *= maxSpeed;
            ofVec2f steer = sepSum - vel;
            acc += steer * 1.5; 
        }
        isMoving = true;
    } else {
        vel *= 0.8; 
        isMoving = false;
    }
    
    ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
    if (pos.distance(mouse) < 100) {
        ofVec2f esc = pos - mouse;
        esc.normalize();
        acc += esc * 2.0;
        isMoving = true;
    }

    vel += acc;
    vel.limit(maxSpeed);
    pos += vel;
    
    float w = ofGetWidth();
    float h = ofGetHeight();
    if(pos.x < -50) pos.x += w+100;
    if(pos.x > w+50) pos.x -= w+100;
    if(pos.y < -50) pos.y += h+100;
    if(pos.y > h+50) pos.y -= h+100;
}

void Agent::draw() {
    ofSetColor(color);
    ofDrawCircle(pos, size);
    if (isMoving && vel.length() > 1.0) {
        ofSetColor(color, 100);
        ofDrawLine(pos, pos - vel * 3.0);
    }
}


// --- ofApp ---

void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(0); 
    ofSetCircleResolution(30);
    
    isGreen = false;
    signalTimer = 0;
    
    roadWidth = ofGetHeight() * 0.55; 
    centerRadius = roadWidth * 0.6; 
    
    agents.resize(800); 
    
    float w = ofGetWidth();
    float h = ofGetHeight();
    
    for(auto &a : agents) {
        int corner = (int)ofRandom(4);
        float x, y;
        float rw = roadWidth/2;
        if(corner == 0) { x = ofRandom(0, w/2-rw); y = ofRandom(0, h/2-rw); }
        else if(corner == 1) { x = ofRandom(w/2+rw, w); y = ofRandom(0, h/2-rw); }
        else if(corner == 2) { x = ofRandom(w/2+rw, w); y = ofRandom(h/2+rw, h); }
        else { x = ofRandom(0, w/2-rw); y = ofRandom(h/2+rw, h); }
        a.setup(ofVec2f(x, y));
    }
}

void ofApp::update(){
    signalTimer += ofGetLastFrameTime();
    
    if(signalTimer > 8.0) {
        signalTimer = 0;
        isGreen = !isGreen;
        
        if(isGreen) {
            float w = ofGetWidth();
            float h = ofGetHeight();
            float rw = roadWidth/2 + 50;
            
            for(auto &a : agents) {
                int targetArea = (int)ofRandom(4);
                ofVec2f dest;
                
                if(targetArea == 0) dest.set(ofRandom(0, w/2-rw), ofRandom(0, h/2-rw));
                else if(targetArea == 1) dest.set(ofRandom(w/2+rw, w), ofRandom(0, h/2-rw));
                else if(targetArea == 2) dest.set(ofRandom(w/2+rw, w), ofRandom(h/2+rw, h));
                else dest.set(ofRandom(0, w/2-rw), ofRandom(h/2+rw, h));
                
                if(a.pos.distance(dest) < roadWidth) {
                     dest.x = w - dest.x; 
                     dest.y = h - dest.y;
                }
                a.target = dest;
            }
        }
    }
    
    for(int i=0; i<agents.size(); i++) {
        agents[i].update(isGreen, agents, i, centerRadius);
    }
}

void ofApp::drawWireframeBuilding(float x, float y, float w, float h, ofColor c) {
    ofNoFill();
    ofSetLineWidth(1);
    ofSetColor(c, 150);
    ofDrawRectangle(x, y, w, h);
    
    float step = 20;
    ofSetColor(c, 50);
    for(float i=0; i<w; i+=step) ofDrawLine(x+i, y, x+i, y+h);
    for(float j=0; j<h; j+=step) ofDrawLine(x, y+j, x+w, y+j);
    
    ofFill();
    if(ofRandom(1.0) < 0.1) {
        ofSetColor(c, 200);
        float rx = ((int)ofRandom(w/step)) * step;
        float ry = ((int)ofRandom(h/step)) * step;
        ofDrawRectangle(x+rx, y+ry, step, step);
    }
}

void ofApp::drawNeonSign(float x, float y, string text, ofColor c, bool vertical) {
    ofSetColor(c);
    if(vertical) {
        for(int i=0; i<text.length(); i++) {
            string s = text.substr(i, 1);
            ofDrawBitmapString(s, x, y + i * 15);
        }
        ofNoFill();
        ofDrawRectangle(x-5, y-15, 20, text.length()*15 + 10);
    } else {
        ofDrawBitmapString(text, x, y);
        ofNoFill();
        ofDrawRectangle(x-5, y-12, text.length()*8 + 10, 20);
    }
}

void ofApp::drawCyberCrosswalk(float x1, float y1, float x2, float y2, float width) {
    ofVec2f start(x1, y1);
    ofVec2f end(x2, y2);
    ofVec2f dir = end - start;
    float len = dir.length();
    dir.normalize();
    ofVec2f perp(-dir.y, dir.x);
    
    int numStripes = (int)(len / 40);
    float time = ofGetElapsedTimef(); // 時間を取得
    
    ofSetLineWidth(width);
    
    for(int i=0; i<numStripes; i++) {
        float t = ofMap(i, 0, numStripes, 0, 1);
        ofVec2f pCenter = start.getInterpolated(end, t);
        
        ofColor col;
        if(isGreen) col = ofColor(0, 255, 255, 100); 
        else col = ofColor(255, 0, 50, 50); 
        
        float distToAgent = 1000;
        for(auto& a : agents) {
            float d = a.pos.distance(pCenter);
            if(d < distToAgent) distToAgent = d;
        }
        
        if(distToAgent < 50) {
            col = ofColor(255, 255, 255, 200);
            width = 6.0; 
        } else {
            width = 4.0;
        }

        ofSetColor(col);
        
        ofPolyline line;
        int res = 10;
        for(int j=0; j<=res; j++) {
            float sideT = ofMap(j, 0, res, -0.5, 0.5);
            ofVec2f point = pCenter + perp * (width * 8.0) * sideT;
            
            // --- 修正箇所 2: time変数を使って波打つように ---
            if(isGreen) {
                // timeを使うことで時間経過でアニメーションします
                point += dir * sin(time * 5.0 + t * 5.0) * 3.0;
            }
            
            line.addVertex(point.x, point.y);
        }
        line.draw();
    }
}

void ofApp::draw(){
    ofBackground(5, 5, 10);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    float w = ofGetWidth();
    float h = ofGetHeight();
    float hw = w/2;
    float hh = h/2;
    float rw = roadWidth/2;
    
    ofSetColor(20, 20, 40);
    ofSetLineWidth(2);
    ofDrawLine(hw-rw, 0, hw-rw, h);
    ofDrawLine(hw+rw, 0, hw+rw, h);
    ofDrawLine(0, hh-rw, w, hh-rw);
    ofDrawLine(0, hh+rw, w, hh+rw);
    
    ofPushMatrix();
    ofTranslate(hw - rw - 20, hh - rw - 20);
    drawWireframeBuilding(-200, -200, 200, 200, ofColor(0, 100, 255)); 
    drawNeonSign(-150, -100, "SHIBUYA VISION", ofColor::cyan);
    if(ofRandom(1.0) < 0.1) {
        ofSetColor(255, 200);
        ofDrawRectangle(-180, -180, 160, 100);
    }
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(hw + rw + 20, hh - rw - 20);
    drawWireframeBuilding(0, -150, 150, 150, ofColor(0, 255, 100)); 
    drawNeonSign(20, -100, "STATION", ofColor::lime);
    ofNoFill();
    ofSetColor(0, 255, 0, 100);
    ofDrawRectangle(10, -50, 100, 40);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(hw - rw - 20, hh + rw + 20);
    ofNoFill();
    ofSetColor(255, 0, 100, 150);
    for(int i=0; i<5; i++) ofDrawCircle(-60, 60, 40 + i*10);
    drawNeonSign(-80, 60, "109", ofColor::magenta);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(hw + rw + 20, hh + rw + 20);
    drawWireframeBuilding(0, 0, 180, 180, ofColor(255, 100, 0)); 
    drawNeonSign(20, 30, "CENTER-GAI", ofColor::yellow);
    drawNeonSign(80, 60, "KARAOKE", ofColor::cyan);
    drawNeonSign(40, 90, "DRUG STORE", ofColor::white);
    ofPopMatrix();
    
    float zw = 4.0;
    drawCyberCrosswalk(hw-rw+10, hh-rw-10, hw+rw-10, hh-rw-10, zw);
    drawCyberCrosswalk(hw-rw+10, hh+rw+10, hw+rw-10, hh+rw+10, zw);
    drawCyberCrosswalk(hw-rw-10, hh-rw+10, hw-rw-10, hh+rw-10, zw);
    drawCyberCrosswalk(hw+rw+10, hh-rw+10, hw+rw+10, hh+rw-10, zw);
    drawCyberCrosswalk(hw-rw+10, hh-rw+10, hw+rw-10, hh+rw-10, zw);
    drawCyberCrosswalk(hw+rw-10, hh-rw+10, hw-rw+10, hh+rw-10, zw);

    for(auto &a : agents) {
        a.draw();
    }
    
    ofPushMatrix();
    ofTranslate(hw, hh);
    
    float pulse = (sin(ofGetElapsedTimef() * 5.0) + 1.0) * 0.5;
    if(isGreen) {
        ofSetColor(0, 255, 255, 50 + pulse * 100);
        ofDrawCircle(0, 0, roadWidth * 0.2); 
        ofSetColor(0, 255, 255);
        ofDrawBitmapString("WALK", -15, 5);
        
        float angle = ofMap(signalTimer, 0, 8.0, 360, 0);
        ofNoFill();
        ofSetLineWidth(3);
        ofPath arc;
        arc.arc(0, 0, 60, 60, 0, angle);
        arc.setStrokeColor(ofColor(0, 255, 255));
        arc.setStrokeWidth(2);
        arc.setFilled(false);
        arc.draw();
        
    } else {
        ofSetColor(255, 0, 50, 50 + pulse * 100);
        ofDrawCircle(0, 0, roadWidth * 0.2);
        ofSetColor(255, 0, 50);
        ofDrawBitmapString("STOP", -15, 5);
        
        ofSetLineWidth(2);
        ofDrawLine(-40, -40, 40, 40);
        ofDrawLine(40, -40, -40, 40);
    }
    ofPopMatrix();
    
    ofDisableBlendMode();
    
    ofNoFill();
    ofSetColor(255, 100);
    ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);
}

void ofApp::mousePressed(int x, int y, int button){
    isGreen = !isGreen;
    signalTimer = 0;
}