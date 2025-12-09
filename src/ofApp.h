#pragma once

#include "ofMain.h"

// 人クラスを定義
class Person {
public:
    ofVec2f pos;       // 現在位置
    ofVec2f target;    // 目的地
    float speed;       // 歩く速さ
    ofColor color;     // 服の色
    
    // コンストラクタ
    Person() {
        speed = ofRandom(1.5, 3.5);
        // 服の色をランダムに（少し暗めでおしゃれに）
        color = ofColor::fromHsb(ofRandom(255), 150, 200);
    }
    
    // 位置を更新する関数
    void update(bool isGreenLight) {
        if (isGreenLight) {
            // 目的地に向かって進むベクトル計算
            ofVec2f dir = target - pos;
            float dist = dir.length();
            
            // 目的地に近づいたらゆっくりにする、遠ければ進む
            if (dist > 5.0) {
                dir.normalize(); // ベクトルの長さを1にする
                pos += dir * speed;
                
                // 少しノイズ（ふらつき）を入れて人間らしく
                pos.x += ofRandom(-0.5, 0.5);
                pos.y += ofRandom(-0.5, 0.5);
            }
        } else {
            // 赤信号の時は待機（少しモジモジさせる）
            pos.x += ofRandom(-0.2, 0.2);
            pos.y += ofRandom(-0.2, 0.2);
        }
    }
    
    void draw() {
        ofSetColor(color);
        ofDrawCircle(pos, 3); // 人を小さな円で描画
    }
};


class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		// 必要な変数
    	vector<Person> people;  // 人のリスト
    	bool isGreenLight;      // 信号の状態
    	float timer;            // 時間計測用
    
    	// 交差点の4つの角の座標（待機場所）
    	vector<ofVec2f> corners;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
		
};
