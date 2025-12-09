#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(10); // 深い黒
    
    isGreen = false;
    signalTimer = 0;
    
    // エージェント生成（少なめでスタイリッシュに）
    agents.resize(400);
    
    float w = ofGetWidth();
    float h = ofGetHeight();
    
    // 初期配置
    for(auto &a : agents) {
        // 四隅のエリアに適当に配置
        float x = (ofRandom(1.0) > 0.5) ? ofRandom(w*0.8, w) : ofRandom(0, w*0.2);
        float y = (ofRandom(1.0) > 0.5) ? ofRandom(h*0.8, h) : ofRandom(0, h*0.2);
        a.setup(ofVec2f(x, y));
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    // 信号タイマー
    signalTimer += ofGetLastFrameTime();
    if(signalTimer > maxTime) {
        signalTimer = 0;
        isGreen = !isGreen;
        
        // 信号切り替え時にターゲット再設定
        if(isGreen) {
            for(auto &a : agents) {
                // 画面の反対側を目指す
                a.target.x = ofGetWidth() - a.pos.x + ofRandom(-100, 100);
                a.target.y = ofGetHeight() - a.pos.y + ofRandom(-100, 100);
            }
        }
    }
    
    // エージェント更新
    for(int i=0; i<agents.size(); i++) {
        agents[i].update(isGreen, agents, i);
        
        // 衝撃波の影響
        for(auto &r : ripples) {
            float d = agents[i].pos.distance(r.pos);
            if(d > r.radius - 20 && d < r.radius + 20) {
                ofVec2f push = agents[i].pos - r.pos;
                push.normalize();
                agents[i].vel += push * 5.0; // 吹き飛ばす
            }
        }
    }
    
    // 衝撃波の更新
    for(int i=ripples.size()-1; i>=0; i--) {
        ripples[i].radius += 10.0; // 広がる速度
        ripples[i].alpha -= 2.0;   // フェードアウト
        if(ripples[i].alpha <= 0) ripples.erase(ripples.begin() + i);
    }
}

//--------------------------------------------------------------
// サイバー横断歩道を描く関数
void ofApp::drawCyberCrosswalk(float centerX, float centerY, float w, float h, bool vertical) {
    float time = ofGetElapsedTimef();
    ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
    
    int numStripes = 15;
    float step = (vertical ? w : h) / numStripes;
    
    // 色の設定：青ならシアン、赤なら赤紫色
    ofColor stripeColor = isGreen ? ofColor(0, 255, 255, 150) : ofColor(255, 0, 100, 150);
    
    for(int i = 0; i < numStripes; i++) {
        ofPolyline line;
        
        float basePos = (vertical ? centerX - w/2 : centerY - h/2) + i * step;
        
        // 線の分割数（細かくすると滑らかに曲がる）
        int resolution = 20;
        
        for(int j = 0; j <= resolution; j++) {
            float t = (float)j / resolution;
            
            // 本来の直線上の座標
            float lx = vertical ? basePos : centerX - w/2 + w * t;
            float ly = vertical ? centerY - h/2 + h * t : basePos;
            
            // 変形ロジック
            ofVec2f point(lx, ly);
            
            // 1. マウスとの反発（空間歪曲）
            float dist = point.distance(mouse);
            if(dist < 250) {
                ofVec2f dir = point - mouse;
                dir.normalize();
                // ガウス関数的な変形
                float offset = 50.0 * exp(-dist * dist / (2 * 60 * 60));
                point += dir * offset;
            }
            
            // 2. 信号が青の時の「流れ」アニメーション
            if(isGreen) {
                float flow = sin(t * 10 + time * 5) * 5.0; // 波打つ
                if(vertical) point.x += flow;
                else point.y += flow;
                
                // ストライプ自体が流れる演出
                if(vertical) point.x += sin(time*2 + i)*2;
            } else {
                // 赤信号の時はグリッチ（ノイズ）のような揺れ
                if(ofRandom(1.0) < 0.05) {
                    if(vertical) point.x += ofRandom(-5, 5);
                    else point.y += ofRandom(-5, 5);
                }
            }
            
            // --- 修正箇所：ここを変更しました ---
            // pointをそのまま渡さず、xとyに分解して渡します
            line.addVertex(point.x, point.y);
        }
        
        ofSetColor(stripeColor);
        ofSetLineWidth(3);
        line.draw();
        
        // 飾りのドット
        if(isGreen && i % 2 == 0) {
            ofSetColor(255, 255);
            // getPointAtPercentはofVec3fを返すのでそのまま使えるはずですが念の為
            auto p = line.getPointAtPercent(fmod(time * 0.5 + i*0.1, 1.0));
            ofDrawCircle(p.x, p.y, 2);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // 加算合成で発光感を出す
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    float w = ofGetWidth();
    float h = ofGetHeight();
    
    // 1. 地面のグリッドを描画（背景）
    ofSetColor(30);
    ofSetLineWidth(1);
    float gridSize = 50;
    // マウスでグリッドも歪ませる
    for(float x=0; x<w; x+=gridSize) {
        for(float y=0; y<h; y+=gridSize) {
            ofVec2f p(x, y);
            float d = p.distance(ofVec2f(ofGetMouseX(), ofGetMouseY()));
            if(d < 200) p += (p - ofVec2f(ofGetMouseX(), ofGetMouseY())).normalize() * (200-d)*0.2;
            ofDrawCircle(p.x, p.y, 1);
        }
    }
    
    // 2. サイバー横断歩道の描画
    float roadW = 300;
    // 縦（上）
    drawCyberCrosswalk(w/2, h/2 - roadW, roadW, roadW, true);
    // 縦（下）
    drawCyberCrosswalk(w/2, h/2 + roadW, roadW, roadW, true);
    // 横（左）
    drawCyberCrosswalk(w/2 - roadW, h/2, roadW, roadW, false);
    // 横（右）
    drawCyberCrosswalk(w/2 + roadW, h/2, roadW, roadW, false);
    
    
    // 3. 中央のフロア一体型信号機（巨大なリング）
    ofPushMatrix();
    ofTranslate(w/2, h/2);
    
    // タイマーゲージ
    ofNoFill();
    ofSetLineWidth(5);
    ofColor signalColor = isGreen ? ofColor::cyan : ofColor::red;
    ofSetColor(signalColor);
    
    float radius = 120;
    // ゲージのアニメーション（青なら減っていく、赤なら増えていく）
    float angle = isGreen ? ofMap(signalTimer, 0, maxTime, 360, 0) : ofMap(signalTimer, 0, maxTime, 0, 360);
    
    ofPath arc;
    arc.setCircleResolution(60);
    arc.arc(0, 0, radius, radius, 0, angle);
    arc.setStrokeColor(signalColor);
    arc.setStrokeWidth(5);
    arc.setFilled(false);
    arc.draw();
    
    // テキスト
    ofFill();
    ofSetColor(255);
    string text = isGreen ? "GO" : "STOP";
    ofDrawBitmapString(text, -10, 5);
    
    // 回転する装飾リング
    ofRotateDeg(ofGetElapsedTimef() * 50);
    ofSetColor(signalColor, 100);
    ofDrawCircle(radius + 20, 0, 5);
    ofDrawCircle(-(radius + 20), 0, 5);
    
    ofPopMatrix();
    
    
    // 4. エージェント（人）の描画
    for(auto &a : agents) {
        a.draw();
    }
    
    // 5. 衝撃波の描画
    ofSetLineWidth(2);
    ofNoFill();
    for(auto &r : ripples) {
        ofSetColor(255, r.alpha);
        ofDrawCircle(r.pos, r.radius);
    }
    ofFill();
    
    ofDisableBlendMode();
    
    // UI情報
    ofSetColor(255);
    ofDrawBitmapString("Click to create Shockwave", 20, 20);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    // クリックで衝撃波を追加
    Ripple r;
    r.pos = ofVec2f(x, y);
    r.radius = 10;
    r.alpha = 255;
    ripples.push_back(r);
}