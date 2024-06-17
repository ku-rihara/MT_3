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

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

struct Ray {
	Vector3 origin;
	Vector3 diff;
};

struct Line {
	Vector3 origin;
	Vector3 diff;
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

Vector3 Perpendicular(const Vector3& vector);

bool IsCollision(const AABB& aabb1, const AABB& aabb2);

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

Vector3 Project(const Vector3& v1, const Vector3& v2);

Vector3 ClosesPoint(const Vector3& point, const Segment& segment);

bool IsCollision(const AABB& aabb, const Sphere& sphere);

bool IsCollision(const OBB& obb, const Sphere& sphere);
float Clamp(float n, float min, float max);
size_t Clamp(size_t n, size_t min, size_t max);
void DrawCatmullRom(const Vector3& p0, const Vector3& p1, const Vector3 p2, const Vector3& p3, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color);
Vector3 Lerp(const Vector3& start, const Vector3& end, float t);

void DrawBezier(const Vector3& cp0, const Vector3& cp1, const Vector3 cp2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color);

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color);


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraTranslate{ 0.0f,0.0f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	Vector3 controlPoints[4] = {
		{-0.8f,0.58f,1.0f},
		{1.76f,1.0f,-0.3f},
		{0.96f,-0.7f,2.3f},
		{-0.53f,-0.26f,-0.15f},
	};

	Sphere constrolSphere[4];


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

		ImGui::Begin("ControlPoint");
		ImGui::DragFloat3("controlPoint[0]", &controlPoints[0].x, 0.01f);
		ImGui::DragFloat3("controlPoint[1]", &controlPoints[1].x, 0.01f);
		ImGui::DragFloat3("controlPoint[2]", &controlPoints[2].x, 0.01f);
		ImGui::DragFloat3("controlPoint[3]", &controlPoints[3].x, 0.01f);
		ImGui::End();

		for (int i = 0; i < 4; i++) {
			constrolSphere[i].center = controlPoints[i];
			constrolSphere[i].radius = 0.03f;
		}

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
		DrawCatmullRom(controlPoints[0], controlPoints[0], controlPoints[1], controlPoints[2], ViewProjectionMatrix, viewportMatrix, BLUE);
		DrawCatmullRom(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3], ViewProjectionMatrix, viewportMatrix, BLUE);
		DrawCatmullRom(controlPoints[1], controlPoints[2], controlPoints[3], controlPoints[3], ViewProjectionMatrix, viewportMatrix, BLUE);
		/*DrawBezier(controlPoints[0], controlPoints[1], controlPoints[2], ViewProjectionMatrix, viewportMatrix, BLUE);*/
		for (int i = 0; i < 4; i++) {
			DrawSphere(constrolSphere[i], ViewProjectionMatrix, viewportMatrix, BLACK);
		}

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

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 normalizeB = Normnalize(v2);
	float dot = Dot(v1, normalizeB);
	return normalizeB * dot;
}

Vector3 ClosesPoint(const Vector3& point, const Segment& segment) {

	Vector3 projection = Project(Vector3(point.x - segment.origin.x, point.y - segment.origin.y, point.z - segment.origin.z), segment.diff);

	return Vector3{ segment.origin.x + projection.x,segment.origin.y + projection.y,segment.origin.z + projection.z };
}

bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	//最近接点を求める
	Vector3 closesPoint{ std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),std::clamp(sphere.center.z,aabb.min.z,aabb.max.z) };
	//最近接点と球の中心との距離を求める
	float distance = Length(closesPoint - sphere.center);
	//距離が半径よりも小さければ衝突
	if (distance <= sphere.radius) {
		return true;
	}
	else {
		return false;
	}
}
//
//bool IsCollision(const OBB& obb, const Sphere& sphere);

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {


	Vector3 verticies[8];
	Vector3 verticiesScreen[8];


	// OBB の 8 つの頂点を計算(中心+各軸のサイズ)
	verticies[0] = obb.center + (obb.orientations[0] * obb.size.x) + (obb.orientations[1] * obb.size.y) + (obb.orientations[2] * obb.size.z);
	verticies[1] = obb.center - (obb.orientations[0] * obb.size.x) + (obb.orientations[1] * obb.size.y) + (obb.orientations[2] * obb.size.z);
	verticies[2] = obb.center + (obb.orientations[0] * obb.size.x) - (obb.orientations[1] * obb.size.y) + (obb.orientations[2] * obb.size.z);
	verticies[3] = obb.center - (obb.orientations[0] * obb.size.x) - (obb.orientations[1] * obb.size.y) + (obb.orientations[2] * obb.size.z);
	verticies[4] = obb.center + (obb.orientations[0] * obb.size.x) + (obb.orientations[1] * obb.size.y) - (obb.orientations[2] * obb.size.z);
	verticies[5] = obb.center - (obb.orientations[0] * obb.size.x) + (obb.orientations[1] * obb.size.y) - (obb.orientations[2] * obb.size.z);
	verticies[6] = obb.center + (obb.orientations[0] * obb.size.x) - (obb.orientations[1] * obb.size.y) - (obb.orientations[2] * obb.size.z);
	verticies[7] = obb.center - (obb.orientations[0] * obb.size.x) - (obb.orientations[1] * obb.size.y) - (obb.orientations[2] * obb.size.z);


	for (int i = 0; i < 8; i++) {

		//アフィン変換
		Matrix4x4 	VerticiesMatrix = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, {}, verticies[i]);
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


bool IsCollision(const AABB& aabb, const Segment& segment) {

	Vector3 tMin = (aabb.min - segment.origin) / segment.diff;
	Vector3 tMax = (aabb.max - segment.origin) / segment.diff;

	Vector3 tNear{ min(tMin.x,tMax.x), min(tMin.y,tMax.y), min(tMin.z,tMax.z) };
	Vector3 tFar{ max(tMin.x,tMax.x), max(tMin.y,tMax.y), max(tMin.z,tMax.z) };

	if ((segment.diff.x == 0 && segment.diff.y == 0) ||
		(segment.diff.x == 0 && segment.diff.z == 0) ||
		(segment.diff.y == 0 && segment.diff.z == 0)) {

		if (segment.origin.x == aabb.max.x || segment.origin.x == aabb.min.x ||
			segment.origin.y == aabb.max.y || segment.origin.y == aabb.min.y ||
			segment.origin.z == aabb.max.z || segment.origin.z == aabb.min.z) {
			return false;
		}

	}

	//AABBとの衝突点（貫通点）のtが小さい方
	float tmin = max(max(tNear.x, tNear.y), tNear.z);
	//AABBとの衝突点（貫通点）のtが大きい方
	float tmax = min(min(tFar.x, tFar.y), tFar.z);

	if (tmin < 1.0f && tmax > 0.0f) {
		if (tmin <= tmax) {
			return true;
		}
		else {

			return false;
		}
	}


	return false;


}

bool IsCollision(const AABB& aabb, const Ray& ray) {

	Vector3 tMin = (aabb.min - ray.origin) / ray.diff;
	Vector3 tMax = (aabb.max - ray.origin) / ray.diff;

	Vector3 tNear{ min(tMin.x,tMax.x), min(tMin.y,tMax.y), min(tMin.z,tMax.z) };
	Vector3 tFar{ max(tMin.x,tMax.x), max(tMin.y,tMax.y), max(tMin.z,tMax.z) };

	if ((ray.diff.x == 0 && ray.diff.y == 0) ||
		(ray.diff.x == 0 && ray.diff.z == 0) ||
		(ray.diff.y == 0 && ray.diff.z == 0)) {

		if (ray.origin.x == aabb.max.x || ray.origin.x == aabb.min.x ||
			ray.origin.y == aabb.max.y || ray.origin.y == aabb.min.y ||
			ray.origin.z == aabb.max.z || ray.origin.z == aabb.min.z) {
			return false;
		}

	}

	//AABBとの衝突点（貫通点）のtが小さい方
	float tmin = max(max(tNear.x, tNear.y), tNear.z);
	//AABBとの衝突点（貫通点）のtが大きい方
	float tmax = min(min(tFar.x, tFar.y), tFar.z);

	if (tmin < 1.0f) {
		if (tmin <= tmax) {
			return true;
		}
		else {

			return false;
		}
	}


	return false;


}

bool IsCollision(const AABB& aabb, const Line& line) {

	Vector3 tMin = (aabb.min - line.origin) / line.diff;
	Vector3 tMax = (aabb.max - line.origin) / line.diff;

	Vector3 tNear{ min(tMin.x,tMax.x), min(tMin.y,tMax.y), min(tMin.z,tMax.z) };
	Vector3 tFar{ max(tMin.x,tMax.x), max(tMin.y,tMax.y), max(tMin.z,tMax.z) };

	if ((line.diff.x == 0 && line.diff.y == 0) ||
		(line.diff.x == 0 && line.diff.z == 0) ||
		(line.diff.y == 0 && line.diff.z == 0)) {

		if (line.origin.x == aabb.max.x || line.origin.x == aabb.min.x ||
			line.origin.y == aabb.max.y || line.origin.y == aabb.min.y ||
			line.origin.z == aabb.max.z || line.origin.z == aabb.min.z) {
			return false;
		}

	}

	//AABBとの衝突点（貫通点）のtが小さい方
	float tmin = max(max(tNear.x, tNear.y), tNear.z);
	//AABBとの衝突点（貫通点）のtが大きい方
	float tmax = min(min(tFar.x, tFar.y), tFar.z);

	if (tmin <= tmax) {
		return true;
	}
	else {

		return false;
	}
}


bool  IsCollision(const OBB& obb, const Sphere& sphere) {
	//回転行列を生成
	Matrix4x4 rotateMatrix;
	rotateMatrix = Matrix4x4::MakeIdentity4x4();
	rotateMatrix.m[0][0] = obb.orientations[0].x;
	rotateMatrix.m[0][1] = obb.orientations[0].y;
	rotateMatrix.m[0][2] = obb.orientations[0].z;
	//Y軸方向			  
	rotateMatrix.m[1][0] = obb.orientations[1].x;
	rotateMatrix.m[1][1] = obb.orientations[1].y;
	rotateMatrix.m[1][2] = obb.orientations[1].z;
	//Z軸方向			  
	rotateMatrix.m[2][0] = obb.orientations[2].x;
	rotateMatrix.m[2][1] = obb.orientations[2].y;
	rotateMatrix.m[2][2] = obb.orientations[2].z;
	Matrix4x4 OBBWorldSTMatrix = Matrix4x4::MakeAffineMatrix({ 1,1,1 }, { }, obb.center);
	Matrix4x4 OBBWorldSTRMatrix = rotateMatrix * OBBWorldSTMatrix;

	Matrix4x4 InverseOBBWorldMatrix = Matrix4x4::Inverse(OBBWorldSTRMatrix);


	Vector3 centerInOBBLocalSphere = Matrix4x4::Transform(sphere.center, InverseOBBWorldMatrix);

	AABB aabbOBBLocal{ .min{-obb.size.x,-obb.size.y,-obb.size.z},.max{obb.size.x,obb.size.y,obb.size.z} };
	Sphere sphereOBBLocal{ centerInOBBLocalSphere,sphere.radius };

	if (IsCollision(aabbOBBLocal, sphereOBBLocal)) {
		return true;
	}
	else {
		return	false;
	}

}

bool IsCollision(const Segment& segment, const OBB& obb) {
	//回転行列を生成
	Matrix4x4 rotateMatrix;
	rotateMatrix = Matrix4x4::MakeIdentity4x4();
	rotateMatrix.m[0][0] = obb.orientations[0].x;
	rotateMatrix.m[0][1] = obb.orientations[0].y;
	rotateMatrix.m[0][2] = obb.orientations[0].z;
	//Y軸方向			  
	rotateMatrix.m[1][0] = obb.orientations[1].x;
	rotateMatrix.m[1][1] = obb.orientations[1].y;
	rotateMatrix.m[1][2] = obb.orientations[1].z;
	//Z軸方向			  
	rotateMatrix.m[2][0] = obb.orientations[2].x;
	rotateMatrix.m[2][1] = obb.orientations[2].y;
	rotateMatrix.m[2][2] = obb.orientations[2].z;
	Matrix4x4 OBBWorldSTMatrix = Matrix4x4::MakeAffineMatrix({ 1,1,1 }, { }, obb.center);
	Matrix4x4 OBBWorldSTRMatrix = rotateMatrix * OBBWorldSTMatrix;

	Matrix4x4 InverseOBBWorldMatrix = Matrix4x4::Inverse(OBBWorldSTRMatrix);

	Vector3 localOrigin = Matrix4x4::Transform(segment.origin, InverseOBBWorldMatrix);
	Vector3 localEnd = Matrix4x4::Transform(segment.origin + segment.diff, InverseOBBWorldMatrix);

	AABB aabbOBBLocal{ .min{-obb.size.x,-obb.size.y,-obb.size.z},.max{obb.size.x,obb.size.y,obb.size.z} };

	Segment localSegment;
	localSegment.origin = localOrigin;
	localSegment.diff = localEnd - localOrigin;
	return IsCollision(aabbOBBLocal, localSegment);
}

bool IsCollision(const Ray& ray, const OBB& obb) {
	//回転行列を生成
	Matrix4x4 rotateMatrix;
	rotateMatrix = Matrix4x4::MakeIdentity4x4();
	rotateMatrix.m[0][0] = obb.orientations[0].x;
	rotateMatrix.m[0][1] = obb.orientations[0].y;
	rotateMatrix.m[0][2] = obb.orientations[0].z;
	//Y軸方向			  
	rotateMatrix.m[1][0] = obb.orientations[1].x;
	rotateMatrix.m[1][1] = obb.orientations[1].y;
	rotateMatrix.m[1][2] = obb.orientations[1].z;
	//Z軸方向			  
	rotateMatrix.m[2][0] = obb.orientations[2].x;
	rotateMatrix.m[2][1] = obb.orientations[2].y;
	rotateMatrix.m[2][2] = obb.orientations[2].z;
	Matrix4x4 OBBWorldSTMatrix = Matrix4x4::MakeAffineMatrix({ 1,1,1 }, { }, obb.center);
	Matrix4x4 OBBWorldSTRMatrix = rotateMatrix * OBBWorldSTMatrix;

	Matrix4x4 InverseOBBWorldMatrix = Matrix4x4::Inverse(OBBWorldSTRMatrix);

	Vector3 localOrigin = Matrix4x4::Transform(ray.origin, InverseOBBWorldMatrix);
	Vector3 localEnd = Matrix4x4::Transform(ray.origin + ray.diff, InverseOBBWorldMatrix);

	AABB aabbOBBLocal{ .min{-obb.size.x,-obb.size.y,-obb.size.z},.max{obb.size.x,obb.size.y,obb.size.z} };

	Ray Localray;
	Localray.origin = localOrigin;
	Localray.diff = localEnd - localOrigin;
	return IsCollision(aabbOBBLocal, Localray);
}

bool IsCollision(const Line& line, const OBB& obb) {
	//回転行列を生成
	Matrix4x4 rotateMatrix;
	rotateMatrix = Matrix4x4::MakeIdentity4x4();
	rotateMatrix.m[0][0] = obb.orientations[0].x;
	rotateMatrix.m[0][1] = obb.orientations[0].y;
	rotateMatrix.m[0][2] = obb.orientations[0].z;
	//Y軸方向			  
	rotateMatrix.m[1][0] = obb.orientations[1].x;
	rotateMatrix.m[1][1] = obb.orientations[1].y;
	rotateMatrix.m[1][2] = obb.orientations[1].z;
	//Z軸方向			  
	rotateMatrix.m[2][0] = obb.orientations[2].x;
	rotateMatrix.m[2][1] = obb.orientations[2].y;
	rotateMatrix.m[2][2] = obb.orientations[2].z;
	Matrix4x4 OBBWorldSTMatrix = Matrix4x4::MakeAffineMatrix({ 1,1,1 }, { }, obb.center);
	Matrix4x4 OBBWorldSTRMatrix = rotateMatrix * OBBWorldSTMatrix;

	Matrix4x4 InverseOBBWorldMatrix = Matrix4x4::Inverse(OBBWorldSTRMatrix);

	Vector3 localOrigin = Matrix4x4::Transform(line.origin, InverseOBBWorldMatrix);
	Vector3 localEnd = Matrix4x4::Transform(line.origin + line.diff, InverseOBBWorldMatrix);

	AABB aabbOBBLocal{ .min{-obb.size.x,-obb.size.y,-obb.size.z},.max{obb.size.x,obb.size.y,obb.size.z} };

	Line localLine;
	localLine.origin = localOrigin;
	localLine.diff = localEnd - localOrigin;
	return IsCollision(aabbOBBLocal, localLine);
}

Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {

	Vector3 result;

	result.x = t * start.x + (1.0f - t) * end.x;
	result.y = t * start.y + (1.0f - t) * end.y;
	result.z = t * start.z + (1.0f - t) * end.z;

	return result;
}

void DrawBezier(const Vector3& cp0, const Vector3& cp1, const Vector3 cp2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color) {
	const int LineMax = 32;
	Vector3 Sp0p1;
	Vector3 Sp0p2;
	Vector3 Sp;
	Vector3 Ep0p1;
	Vector3 Ep0p2;
	Vector3 Ep;
	Matrix4x4 	StartMatrix[LineMax];
	Matrix4x4 	StartwvpMatrix[LineMax];
	Vector3 StartScreen[LineMax];
	Matrix4x4 	EndMatrix[LineMax];
	Matrix4x4 	EndwvpMatrix[LineMax];
	Vector3 EndScreen[LineMax];

	for (int index = 0; index < LineMax; index++) {
		float t0 = index / float(LineMax);
		float t1 = (index + 1) / float(LineMax);

		Sp0p1 = Lerp(cp0, cp1, t0);
		Sp0p2 = Lerp(cp1, cp2, t0);
		Ep0p1 = Lerp(cp0, cp1, t1);
		Ep0p2 = Lerp(cp1, cp2, t1);

		Sp = Lerp(Sp0p1, Sp0p2, t0);
		Ep = Lerp(Ep0p1, Ep0p2, t1);

		//アフィン変換
		StartMatrix[index] = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, {}, Sp);
		EndMatrix[index] = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, {}, Ep);
		//wvpMatrix
		StartwvpMatrix[index] = StartMatrix[index] * viewProjectionMatrix;
		EndwvpMatrix[index] = EndMatrix[index] * viewProjectionMatrix;
		//Screen変換
		StartScreen[index] = Matrix4x4::ScreenTransform(Vector3{}, StartwvpMatrix[index], viewprtMatrix);
		EndScreen[index] = Matrix4x4::ScreenTransform(Vector3{}, EndwvpMatrix[index], viewprtMatrix);

		Novice::DrawLine(int(StartScreen[index].x), int(StartScreen[index].y), int(EndScreen[index].x), int(EndScreen[index].y), color);
	}

}
void DrawCatmullRom(const Vector3& p0, const Vector3& p1, const Vector3 p2, const Vector3& p3, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color) {
	const int LineMax = 32;
	Vector3 result;
	float s = 0.5f;
	Matrix4x4 	StartMatrix[LineMax];
	Matrix4x4 	StartwvpMatrix[LineMax];
	Matrix4x4 	EndMatrix[LineMax];
	Matrix4x4 	EndwvpMatrix[LineMax];
	Vector3 StartScreen[LineMax];
	Vector3 EndScreen[LineMax];
	Vector3 Start;
	Vector3 End;

	for (int index = 0; index < LineMax; index++) {
		float t0 = index / float(LineMax);
		float t1 = (index + 1) / float(LineMax);
		float t02 = std::powf(t0, 2);
		float t03 = std::powf(t0, 3);
		float t12 = std::powf(t1, 2);
		float t13 = std::powf(t1, 3);

		Start.x = s * ((-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t03 + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t02 + (-p0.x + p2.x) * t0 + 2 * p1.x);
		Start.y = s * ((-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t03 + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t02 + (-p0.y + p2.y) * t0 + 2 * p1.y);
		Start.z = s * ((-p0.z + 3 * p1.z - 3 * p2.z + p3.z) * t03 + (2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z) * t02 + (-p0.z + p2.z) * t0 + 2 * p1.z);

		End.x = s * ((-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t13 + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t12 + (-p0.x + p2.x) * t1 + 2 * p1.x);
		End.y = s * ((-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t13 + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t12 + (-p0.y + p2.y) * t1 + 2 * p1.y);
		End.z = s * ((-p0.z + 3 * p1.z - 3 * p2.z + p3.z) * t13 + (2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z) * t12 + (-p0.z + p2.z) * t1 + 2 * p1.z);
		//アフィン変換
		StartMatrix[index] = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, {}, Start);
		EndMatrix[index] = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, {}, End);
		//wvpMatrix
		StartwvpMatrix[index] = StartMatrix[index] * viewProjectionMatrix;
		EndwvpMatrix[index] = EndMatrix[index] * viewProjectionMatrix;
		//Screen変換
		StartScreen[index] = Matrix4x4::ScreenTransform(Vector3{}, StartwvpMatrix[index], viewprtMatrix);
		EndScreen[index] = Matrix4x4::ScreenTransform(Vector3{}, EndwvpMatrix[index], viewprtMatrix);
		Novice::DrawLine(int(StartScreen[index].x), int(StartScreen[index].y), int(EndScreen[index].x), int(EndScreen[index].y), color);

	}
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

//Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t) {
//	assert(points.size() >= 4 && "制御点は４点以上必要です");
//	// 区間数は制御点の数-1
//	size_t division = points.size() - 1;
//	// 1区間の長さ(全体を1.0fとした割合)
//	float areaWidth = 1.0f / division;
//	// 区間内の始点0.0f、終点を1.0fとした時の現在位置
//	float t_2 = std::fmod(t, areaWidth) * division;
//	// 下限(0.0f)と上限(1.0f)とした時の現在位置
//	t_2 = Clamp(t_2, 0.0f, 1.0f);
//	// 区間番号
//	size_t index = static_cast<size_t>(t / areaWidth);
//	// 区間番号が上限を超えないように収める
//	index = Clamp(index, 0, division - 1);
//
//	// 4点分のインデックス
//	size_t index0 = index - 1;
//	size_t index1 = index;
//	size_t index2 = index + 1;
//	size_t index3 = index + 2;
//	// 最初の区間のp0はp1を重複使用する
//	if (index == 0) {
//		index0 = index1;
//	}
//	// 最初の区間のp3はp2を重複使用する
//	if (index3 >= points.size()) {
//		index3 = index2;
//	}
//
//	// 4点の座標
//	const Vector3& p0 = points[index0];
//	const Vector3& p1 = points[index1];
//	const Vector3& p2 = points[index2];
//	const Vector3& p3 = points[index3];
//
//	// 4点を指定してCamull-Rom補間
//	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
//}