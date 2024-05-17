#include <Novice.h>
#define _USE_MATH_DEFINES
#include<cmath>
#include"Vector3.h"
#include"Matrix4x4.h"
#include<assert.h>
#include<imgui.h>


const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_タイトル";

struct Segment{
	Vector3 origin;
	Vector3 diff;
};

struct Sphere {
	Vector3 center;
	float radius;
};

struct Plane {
	Vector3 narmal;
	float distance;
};

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

Vector3 Project(const Vector3& v1, const Vector3& v2);

Vector3 ClosesPoint(const Vector3& point, const Segment& segment);

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color);

bool IsColligion(const Sphere& sphere, const Plane& plane);

Vector3 Perpendicular(const Vector3& vector);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	/*Segment segment{ {-2.0f,-1.0f,0.0f},{3.0f,2.0f,2.0f} };
	Vector3 point{ -1.5f,0.6f,0.6f };*/
	Plane plane;
	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("Plane.Normal", &.x, 0.01f);
		ImGui::End();
		Matrix4x4 camelaMatrix = Matrix4x4::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Matrix4x4::Inverse(camelaMatrix);
		Matrix4x4 projectionMatrix = Matrix4x4::MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 ViewProjectionMatrix =viewMatrix*projectionMatrix;
		Matrix4x4 viewportMatrix = Matrix4x4::MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(ViewProjectionMatrix, viewportMatrix);
		
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;//Gridの半分の幅
	const uint32_t kSubdivision = 10;//分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);//1つ分の長さ
	Vector3 zLineStart;
	Vector3 zLineEnd;
	Vector3 xLineStart;
	Vector3 xLineEnd;
	//奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		//上の情報を使ってワールド座標系の始点と終点を求める
		   // ワールド座標系でのグリッドの始点と終点を設定
		zLineStart = Vector3(xIndex * kGridEvery - kGridHalfWidth, 0, kGridHalfWidth);
		zLineEnd = Vector3(xIndex * kGridEvery - kGridHalfWidth, 0, -kGridHalfWidth);

		 //アフィン変換
		 Matrix4x4  zLineStartMatrix = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, zLineStart);
		 Matrix4x4 zLineEndMatrix = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, zLineEnd);
		 //wvpMatrix
		 Matrix4x4  zLineStartwvpMatrix = zLineStartMatrix* viewProjectionMatrix;
		 Matrix4x4  zLineEndwvpMatrix = zLineEndMatrix*viewProjectionMatrix;
		 //Screen変換
		 Vector3  zLineScreenStart = Matrix4x4::ScreenTransform(Vector3{}, zLineStartwvpMatrix, viewportMatrix);
		 Vector3  zLineScreenEnd = Matrix4x4::ScreenTransform(Vector3{}, zLineEndwvpMatrix, viewportMatrix);
		 
		 if (xIndex==kSubdivision /2) {
			 Novice::DrawLine(int(zLineScreenStart.x), int(zLineScreenStart.y), int(zLineScreenEnd.x), int(zLineScreenEnd.y), 0x000000FF);

		 }
		 else {
			 Novice::DrawLine(int(zLineScreenStart.x), int(zLineScreenStart.y), int(zLineScreenEnd.x), int(zLineScreenEnd.y), 0xAAAAAAFF);
		 }
		}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		//上の情報を使ってワールド座標系の始点と終点を求める
		   // ワールド座標系でのグリッドの始点と終点を設定
		xLineStart = Vector3(-kGridHalfWidth, 0, zIndex * kGridEvery - kGridHalfWidth);
		xLineEnd = Vector3(kGridHalfWidth, 0, zIndex * kGridEvery - kGridHalfWidth);

		//アフィン変換
		Matrix4x4 	xLineStartMatrix = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, xLineStart);
		Matrix4x4 xLineEndMatrix = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, xLineEnd);
		//wvpMatrix
		Matrix4x4 	xLineStartwvpMatrix =xLineStartMatrix* viewProjectionMatrix;
		Matrix4x4 xLineEndwvpMatrix =xLineEndMatrix* viewProjectionMatrix;
		//Screen変換
		Vector3 xLineScreenStart = Matrix4x4::ScreenTransform(Vector3{}, xLineStartwvpMatrix, viewportMatrix);
		Vector3 xLineScreenEnd = Matrix4x4::ScreenTransform(Vector3{}, xLineEndwvpMatrix, viewportMatrix);
		if (zIndex == kSubdivision / 2) {
			Novice::DrawLine(int(xLineScreenStart.x), int(xLineScreenStart.y), int(xLineScreenEnd.x), int(xLineScreenEnd.y), 0x000000FF);
		}
		else {
			Novice::DrawLine(int(xLineScreenStart.x), int(xLineScreenStart.y), int(xLineScreenEnd.x), int(xLineScreenEnd.y), 0xAAAAAAFF);
		}
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color) {
	float pi = float(M_PI);
	const uint32_t kSbudivision = 10;//分割数
	const float kLatEvery = pi / kSbudivision;//緯度分割1つ分の角度
	const float kLonEvery = (2 * pi) / kSbudivision;//経度分割1つ分の角度
	//world座標系でのa,b,cを求める
	Vector3 a, b, c;
	//緯度の方向に分割　-π/2～π/2
	for (uint32_t latindex = 0; latindex < kSbudivision; ++latindex) {
		sphere;
		float lat = -pi / 2.0f + kLatEvery * latindex;//現在の緯度
		//経度の方向に分割0～2π
		for (uint32_t lonIndex = 0; lonIndex < kSbudivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;//現在の経度
			// ワールド座標系での頂点を求める
			a = {
			   (sphere.center.x + (sphere.radius * (std::cos(lat) * std::cos(lon)))),
			   (sphere.center.y + (sphere.radius * std::sin(lat))),
			   (sphere.center.z + (sphere.radius * (std::cos(lat) * std::sin(lon))))
			};

			b = {
			   (sphere.center.x + (sphere.radius* (std::cos(lat + kLatEvery) * std::cos(lon)))),
			   (sphere.center.y + (sphere.radius* std::sin(lat + kLatEvery))),
			   (sphere.center.z + (sphere.radius* (std::cos(lat + kLatEvery) * std::sin(lon))))
			};					  
								  
			c = {				  
			   (sphere.center.x + (sphere.radius* (std::cos(lat) * std::cos(lon + kLonEvery)))),
			   (sphere.center.y + (sphere.radius* std::sin(lat))),
			   (sphere.center.z + (sphere.radius* (std::cos(lat) * std::sin(lon + kLonEvery))))
			};

			Matrix4x4 MatrixA = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, a);
			Matrix4x4 MatrixB = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, b);
			Matrix4x4 MatrixC = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, c);
			Matrix4x4 wvpMatrixA = MatrixA*viewProjectionMatrix;
			Matrix4x4 wvpMatrixB = MatrixB* viewProjectionMatrix;
			Matrix4x4 wvpMatrixC = MatrixC* viewProjectionMatrix;
			Vector3 screenA = Matrix4x4::ScreenTransform(Vector3{}, wvpMatrixA, viewprtMatrix);
			Vector3 screenB = Matrix4x4::ScreenTransform(Vector3{}, wvpMatrixB, viewprtMatrix);
			Vector3 screenC = Matrix4x4::ScreenTransform(Vector3{}, wvpMatrixC, viewprtMatrix);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x), int(screenC.y), color);
		}
	}
}


//Vector3 Project(const Vector3& v1, const Vector3& v2) {
//	Vector3 normalizeB = Normnalize(v2);
//	float dot = Dot(v1, normalizeB);
//	return normalizeB*dot;
//}
//
//Vector3 ClosesPoint(const Vector3& point, const Segment& segment) {
//	
//	Vector3 projection = Project(Vector3(point.x - segment.origin.x, point.y - segment.origin.y, point.z - segment.origin.z), segment.diff);
//
//	return Vector3{ segment.origin.x + projection.x,segment.origin.y + projection.y,segment.origin.z + projection.z }; 
//}

Vector3 Perpendicular(const Vector3& vector) {

}