#pragma once

#include "ofMain.h"
#include <algorithm>

// 人の定義
class Agent {
public:
    ofVec2f pos, vel, target;
    float speed;
    ofColor color;
    float size;
    
    void setup(ofVec2f startPos) {
        pos = startPos;
        target = startPos;
        speed = ofRandom(2.0, 4.0);
        size = ofRandom(3, 6);
        
        // サイバーパンクな色使い
        if(ofRandom(1.0) < 0.1) color = ofColor::cyan;
        else if(ofRandom(1.0) < 0.2) color = ofColor::magenta;
        else color = ofColor(200, 255, 255, 200);
    }
    
    void update(bool isGreen, const vector<Agent>& others, int index) {
        ofVec2f acc(0,0);
        
        // 1. ターゲットへの力
        if(isGreen) {
            ofVec2f dir = target - pos;
            float dist = dir.length();
            if(dist > 10) {
                dir.normalize();
                acc += (dir * speed - vel) * 0.1;
            }
            
            // 2. 分離（人同士）
            for(int i=0; i<others.size(); i++){
                if(i == index) continue;
                float d = pos.squareDistance(others[i].pos);
                if(d < 400 && d > 0) { // 半径20px以内
                    ofVec2f diff = pos - others[i].pos;
                    diff.normalize();
                    acc += diff * 1.5;
                }
            }
        } else {
            // 待機中はゆらゆらする
            acc += ofVec2f(ofRandom(-0.5,0.5), ofRandom(-0.5,0.5));
        }
        
        // 3. マウスからの干渉（空間の歪み）
        ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
        float distToMouse = pos.distance(mouse);
        if(distToMouse < 200) {
            ofVec2f esc = pos - mouse;
            esc.normalize();
            acc += esc * (200 - distToMouse) * 0.05;
        }

        vel += acc;
        vel.limit(isGreen ? speed * 1.5 : 1.0);
        pos += vel;
        
        // 画面端処理
        if(pos.x < 0) pos.x += ofGetWidth();
        if(pos.x > ofGetWidth()) pos.x -= ofGetWidth();
        if(pos.y < 0) pos.y += ofGetHeight();
        if(pos.y > ofGetHeight()) pos.y -= ofGetHeight();
    }
    
    void draw() {
        ofSetColor(color);
        ofDrawCircle(pos, size);
    }
};

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void drawCyberCrosswalk(float x, float y, float w, float h, bool vertical);
    void mousePressed(int x, int y, int button);

    vector<Agent> agents;
    bool isGreen;
    float signalTimer;
    float maxTime = 10.0;
    
    // 衝撃波エフェクト用
    struct Ripple {
        ofVec2f pos;
        float radius;
        float alpha;
    };
    vector<Ripple> ripples;
};