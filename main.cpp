#include <Novice.h>
#define _USE_MATH_DEFINES
#include<cmath>
#include<algorithm>
#include"Vector3.h"
#include"Matrix4x4.h"
#include<assert.h>
#include<imgui.h>


const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_タイトル";

struct Sphere {
	Vector3 center;
	float radius;
};

struct Spring {
	//アンカー、固定された端の位置
	Vector3 anchor;
	float naturalLength;//自然長
	float stiffness;//バネ定数k
	float dampingCoefficient;//減衰係数
};

struct Ball {
	Vector3 pos;
	Vector3 velocity;
	Vector3 acceleration;
	float mass;
	float radius;
	unsigned int color;
};

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

Vector3 Perpendicular(const Vector3& vector);

float Clamp(float n, float min, float max);
size_t Clamp(size_t n, size_t min, size_t max);
Vector3 Lerp(const Vector3& start, const Vector3& end, float t);


void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color);
void LineDraw(const Vector3& startPos, const Vector3& endPos, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraTranslate{ 0.0f,0.0f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };
	//バネ
	Spring spring{};
	spring.anchor = { 0.0f,1.0f,0.0f };
	spring.naturalLength = 0.7f;
	spring.stiffness = 100.0f;
	spring.dampingCoefficient = 2.0f;
	//ボール
	Ball ball{};
	ball.pos = { 0.8f,0.2f,0.0f };
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = BLUE;
	const Vector3 kGravity{ 0.0f,-9.8f,0.0f };
	//デルタタイム
	float deltaTime = 1.0f / 60.0f;
	Vector3 restoringForce;
	Sphere sphere;
	bool isstart=false;
	
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
		Matrix4x4 camelaMatrix = Matrix4x4::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, {}, cameraTranslate);
		camelaMatrix = camelaMatrix * Matrix4x4::MakeRotateXMatrix(cameraRotate.x);
		camelaMatrix = camelaMatrix * Matrix4x4::MakeRotateYMatrix(cameraRotate.y);
		camelaMatrix = camelaMatrix * Matrix4x4::MakeRotateZMatrix(cameraRotate.z);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::Text("Push:SPACE");
		ImGui::End();

		if (keys[DIK_SPACE]&&!isstart) {
			isstart = true;
		}

		Matrix4x4 viewMatrix = Matrix4x4::Inverse(camelaMatrix);
		Matrix4x4 projectionMatrix = Matrix4x4::MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 ViewProjectionMatrix = viewMatrix * projectionMatrix;
		Matrix4x4 viewportMatrix = Matrix4x4::MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		Vector3 diff = ball.pos - spring.anchor;
		float length = Length(diff);
		if (length != 0.0f) {
			Vector3 direction = Normnalize(diff);
			Vector3 restPosition = spring.anchor + direction * spring.naturalLength;
			Vector3 displacement = (ball.pos - restPosition) * length;
			 restoringForce = displacement * -spring.stiffness;
			 //減衰抵抗を計算する
			 Vector3 dampingForce = ball.velocity * -spring.dampingCoefficient;
			 //減衰抵抗も加味して、物体にかかる力を決定する
			 Vector3 force = restoringForce + dampingForce+kGravity;

			ball.acceleration = force / ball.mass;
		}

		if (isstart) {
			//加速度も速度もどちらも秒を基準とした値である
			//それが、1/60秒間（deltaTime）適応されたと考えられる
			ball.velocity += ball.acceleration * deltaTime;
			ball.pos += ball.velocity * deltaTime;
		}
		sphere.center = ball.pos;
		sphere.radius = ball.radius;
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		DrawGrid(ViewProjectionMatrix, viewportMatrix);
		LineDraw(Vector3{0,1.0,0}, ball.pos, ViewProjectionMatrix, viewportMatrix, WHITE);
		DrawSphere(sphere, ViewProjectionMatrix, viewportMatrix, ball.color);


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
		Matrix4x4  zLineStartwvpMatrix = zLineStartMatrix * viewProjectionMatrix;
		Matrix4x4  zLineEndwvpMatrix = zLineEndMatrix * viewProjectionMatrix;
		//Screen変換
		Vector3  zLineScreenStart = Matrix4x4::ScreenTransform(Vector3{}, zLineStartwvpMatrix, viewportMatrix);
		Vector3  zLineScreenEnd = Matrix4x4::ScreenTransform(Vector3{}, zLineEndwvpMatrix, viewportMatrix);

		if (xIndex == kSubdivision / 2) {
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
		Matrix4x4 	xLineStartwvpMatrix = xLineStartMatrix * viewProjectionMatrix;
		Matrix4x4 xLineEndwvpMatrix = xLineEndMatrix * viewProjectionMatrix;
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
			   (sphere.center.x + (sphere.radius * (std::cos(lat + kLatEvery) * std::cos(lon)))),
			   (sphere.center.y + (sphere.radius * std::sin(lat + kLatEvery))),
			   (sphere.center.z + (sphere.radius * (std::cos(lat + kLatEvery) * std::sin(lon))))
			};

			c = {
			   (sphere.center.x + (sphere.radius * (std::cos(lat) * std::cos(lon + kLonEvery)))),
			   (sphere.center.y + (sphere.radius * std::sin(lat))),
			   (sphere.center.z + (sphere.radius * (std::cos(lat) * std::sin(lon + kLonEvery))))
			};

			Matrix4x4 MatrixA = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, a);
			Matrix4x4 MatrixB = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, b);
			Matrix4x4 MatrixC = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, c);
			Matrix4x4 wvpMatrixA = MatrixA * viewProjectionMatrix;
			Matrix4x4 wvpMatrixB = MatrixB * viewProjectionMatrix;
			Matrix4x4 wvpMatrixC = MatrixC * viewProjectionMatrix;
			Vector3 screenA = Matrix4x4::ScreenTransform(Vector3{}, wvpMatrixA, viewprtMatrix);
			Vector3 screenB = Matrix4x4::ScreenTransform(Vector3{}, wvpMatrixB, viewprtMatrix);
			Vector3 screenC = Matrix4x4::ScreenTransform(Vector3{}, wvpMatrixC, viewprtMatrix);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x), int(screenC.y), color);
		}
	}
}

void LineDraw(const Vector3& startPos, const Vector3& endPos, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color) {
	Matrix4x4 MatrixA = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, startPos);
	Matrix4x4 MatrixB = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, endPos);

	Matrix4x4 wvpMatrixA = MatrixA * viewProjectionMatrix;
	Matrix4x4 wvpMatrixB = MatrixB * viewProjectionMatrix;

	Vector3 screenA = Matrix4x4::ScreenTransform(Vector3{}, wvpMatrixA, viewprtMatrix);
	Vector3 screenB = Matrix4x4::ScreenTransform(Vector3{}, wvpMatrixB, viewprtMatrix);

	Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);

}


Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return{ -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}




Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {

	Vector3 result;

	result.x = t * start.x + (1.0f - t) * end.x;
	result.y = t * start.y + (1.0f - t) * end.y;
	result.z = t * start.z + (1.0f - t) * end.z;

	return result;
}


float Clamp(float n, float min, float max) {
	if (n > max) {
		return max;
	}
	if (n < min) {
		return min;
	}
	return n;
}

size_t Clamp(size_t n, size_t min, size_t max) {
	if (n > max) {
		return max;
	}
	if (n < min) {
		return min;
	}
	return n;
}

