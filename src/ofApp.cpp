#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(30); // アスファルトっぽい色
    
    isGreenLight = false;
    timer = 0;
    
    // 4つの角（待機エリア）を定義
    float w = ofGetWidth();
    float h = ofGetHeight();
    float padding = 100;
    
    // 左上、右上、右下、左下
    corners.push_back(ofVec2f(padding, padding));
    corners.push_back(ofVec2f(w - padding, padding));
    corners.push_back(ofVec2f(w - padding, h - padding));
    corners.push_back(ofVec2f(padding, h - padding));
    
    // 1000人の人を生成
    for(int i = 0; i < 1000; i++){
        Person p;
        
        // ランダムな角を選んで配置する
        int startCornerIndex = (int)ofRandom(4);
        ofVec2f startBase = corners[startCornerIndex];
        
        // 角の周辺にばらけさせる（待機している集団）
        p.pos = startBase + ofVec2f(ofRandom(-80, 80), ofRandom(-80, 80));
        
        // 最初のターゲットは自分の位置（動かない）
        p.target = p.pos;
        
        people.push_back(p);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    // タイマー更新
    float timePerCycle = 8.0; // 8秒ごとに信号が変わる
    float currentTime = ofGetElapsedTimef();
    
    // 周期的に信号を切り替える
    bool nextState = (fmod(currentTime, timePerCycle * 2) < timePerCycle);
    
    // 信号が「赤」から「青」に変わった瞬間だけターゲットを再設定
    if(nextState == true && isGreenLight == false){
        for(auto &p : people){
            // 自分の今いる場所とは違う角をランダムに目指す（スクランブル！）
            int targetIndex = (int)ofRandom(4);
            ofVec2f targetBase = corners[targetIndex];
            
            // ターゲット位置も少しばらけさせる
            p.target = targetBase + ofVec2f(ofRandom(-80, 80), ofRandom(-80, 80));
        }
    }
    
    isGreenLight = nextState;
    
    // 全員の状態を更新
    for(auto &p : people){
        p.update(isGreenLight);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    float w = ofGetWidth();
    float h = ofGetHeight();
    
    // 1. 横断歩道を描く（雰囲気）
    ofSetColor(255, 100); // 半透明の白
    ofSetLineWidth(50);
    
    // バッテン（スクランブル）
    ofDrawLine(100, 100, w-100, h-100);
    ofDrawLine(w-100, 100, 100, h-100);
    
    // 四角い枠
    ofSetLineWidth(10);
    ofNoFill();
    ofDrawRectangle(100, 100, w-200, h-200);
    ofFill(); // 塗りつぶし設定に戻す
    
    // 2. 人を描く
    for(auto &p : people){
        p.draw();
    }
    
    // 3. 信号機を描画（画面中央上）
    if(isGreenLight){
        ofSetColor(0, 255, 100); // 青信号
        ofDrawBitmapString("WALK", w/2 - 20, 50);
    } else {
        ofSetColor(255, 0, 50); // 赤信号
        ofDrawBitmapString("WAIT", w/2 - 20, 50);
    }
    ofDrawCircle(w/2, 30, 10);
    
    // 4. 情報表示
    ofSetColor(255);
    ofDrawBitmapString("People: " + ofToString(people.size()), 20, 20);
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

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
