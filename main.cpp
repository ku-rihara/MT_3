#include <Novice.h>
#define _USE_MATH_DEFINES
#include<cmath>
#include"Vector3.h"
#include"Matrix4x4.h"
#include<assert.h>
#include<imgui.h>


const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_タイトル";



struct AABB {
	Vector3 min;
	Vector3 max;
};

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

Vector3 Perpendicular(const Vector3& vector);

bool IsCollision(const AABB& aabb1, const AABB& aabb2);

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraTranslate{ 0.0f,0.0f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	AABB aabb1{
		.min{-0.5f,-0.5f,-0.5f},
		.max{0.0f,0.0f,0.0f},
	};
	AABB aabb2{
		.min{0.2f,0.2f,0.2f},
		.max{1.0f,1.0f,1.0f},
	};

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
		ImGui::End();

		ImGui::Begin("aabb1");
		aabb1.min.x = (std::min)(aabb1.min.x, aabb1.max.x);
		aabb1.max.x = (std::max)(aabb1.min.x, aabb1.max.x);
		aabb1.min.y = (std::min)(aabb1.min.y, aabb1.max.y);
		aabb1.max.y = (std::max)(aabb1.min.y, aabb1.max.y);
		aabb1.min.z = (std::min)(aabb1.min.z, aabb1.max.z);
		aabb1.max.z = (std::max)(aabb1.min.z, aabb1.max.z);
		ImGui::DragFloat3("aabb1Max", &aabb1.max.x, 0.01f);
		ImGui::DragFloat3("aabb1Min", &aabb1.min.x, 0.01f);
		ImGui::End();

		ImGui::Begin("aabb2");
		ImGui::DragFloat3("aabb2Max", &aabb2.max.x, 0.01f);
		ImGui::DragFloat3("aabb2Min", &aabb2.min.x, 0.01f);
		aabb2.min.x = (std::min)(aabb2.min.x, aabb2.max.x);
		aabb2.max.x = (std::max)(aabb2.min.x, aabb2.max.x);
		aabb2.min.y = (std::min)(aabb2.min.y, aabb2.max.y);
		aabb2.max.y = (std::max)(aabb2.min.y, aabb2.max.y);
		aabb2.min.z = (std::min)(aabb2.min.z, aabb2.max.z);
		aabb2.max.z = (std::max)(aabb2.min.z, aabb2.max.z);
		ImGui::End();

		Matrix4x4 viewMatrix = Matrix4x4::Inverse(camelaMatrix);
		Matrix4x4 projectionMatrix = Matrix4x4::MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 ViewProjectionMatrix = viewMatrix * projectionMatrix;
		Matrix4x4 viewportMatrix = Matrix4x4::MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(ViewProjectionMatrix, viewportMatrix);

		if (IsCollision(aabb1, aabb2)) {
			DrawAABB(aabb1, ViewProjectionMatrix, viewportMatrix, RED);
		}
		else {
			DrawAABB(aabb1, ViewProjectionMatrix, viewportMatrix, WHITE);
		}

		DrawAABB(aabb2, ViewProjectionMatrix, viewportMatrix, WHITE);

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


Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return{ -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	}
	else {
		return false;
	}
}

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {

	Vector3 verticies[8];
	Vector3 verticiesScreen[8];
	//上の面
	verticies[0] = { aabb.min.x,aabb.max.y,aabb.max.z };//左奥
	verticies[1] = { aabb.max.x,aabb.max.y,aabb.max.z };//右奥
	verticies[2] = { aabb.min.x,aabb.max.y,aabb.min.z };//左手前
	verticies[3] = { aabb.max.x,aabb.max.y,aabb.min.z };//右手前
	//下の面
	verticies[4] = { aabb.min.x,aabb.min.y,aabb.max.z }; //左奥
	verticies[5] = { aabb.max.x,aabb.min.y,aabb.max.z }; //右奥
	verticies[6] = { aabb.min.x,aabb.min.y,aabb.min.z };///左手前
	verticies[7] = { aabb.max.x,aabb.min.y,aabb.min.z };///右手前

	for (int i = 0; i < 8; i++) {
		//アフィン変換
		Matrix4x4 	VerticiesMatrix = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, Vector3{}, verticies[i]);
		//wvpMatrix
		Matrix4x4 	VerticieswvpMatrix = VerticiesMatrix * viewProjectionMatrix;
		//Screen変換
		verticiesScreen[i] = Matrix4x4::ScreenTransform(Vector3{}, VerticieswvpMatrix, viewportMatrix);
	}

	//点をつなぐ
	Novice::DrawLine(int(verticiesScreen[0].x), int(verticiesScreen[0].y), int(verticiesScreen[1].x), int(verticiesScreen[1].y), color);//左から右
	Novice::DrawLine(int(verticiesScreen[0].x), int(verticiesScreen[0].y), int(verticiesScreen[2].x), int(verticiesScreen[2].y), color);//奥から手前
	Novice::DrawLine(int(verticiesScreen[0].x), int(verticiesScreen[0].y), int(verticiesScreen[4].x), int(verticiesScreen[4].y), color);//上から下

	Novice::DrawLine(int(verticiesScreen[1].x), int(verticiesScreen[1].y), int(verticiesScreen[3].x), int(verticiesScreen[3].y), color);
	Novice::DrawLine(int(verticiesScreen[1].x), int(verticiesScreen[1].y), int(verticiesScreen[5].x), int(verticiesScreen[5].y), color);
	Novice::DrawLine(int(verticiesScreen[2].x), int(verticiesScreen[2].y), int(verticiesScreen[3].x), int(verticiesScreen[3].y), color);
	Novice::DrawLine(int(verticiesScreen[2].x), int(verticiesScreen[2].y), int(verticiesScreen[6].x), int(verticiesScreen[6].y), color);
	Novice::DrawLine(int(verticiesScreen[3].x), int(verticiesScreen[3].y), int(verticiesScreen[7].x), int(verticiesScreen[7].y), color);
	Novice::DrawLine(int(verticiesScreen[4].x), int(verticiesScreen[4].y), int(verticiesScreen[5].x), int(verticiesScreen[5].y), color);
	Novice::DrawLine(int(verticiesScreen[4].x), int(verticiesScreen[4].y), int(verticiesScreen[6].x), int(verticiesScreen[6].y), color);
	Novice::DrawLine(int(verticiesScreen[7].x), int(verticiesScreen[7].y), int(verticiesScreen[5].x), int(verticiesScreen[5].y), color);
	Novice::DrawLine(int(verticiesScreen[7].x), int(verticiesScreen[7].y), int(verticiesScreen[6].x), int(verticiesScreen[6].y), color);



}