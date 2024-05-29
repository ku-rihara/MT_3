#include <Novice.h>
#define _USE_MATH_DEFINES
#include<cmath>
#include"Vector3.h"
#include"Matrix4x4.h"
#include<assert.h>
#include<imgui.h>


const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_タイトル";

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

struct Sphere {
	Vector3 center;
	float radius;
};

struct Triangle {
	Vector3 vertices[3];
};
struct Plane {
	Vector3 normal;
	float distance;
};

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewprtMatrix, uint32_t color);

bool IsColligion(const Sphere& sphere, const Plane& plane);

bool IsColligion(const Segment& segment, const Plane& plane);

bool IsColligion(const Triangle& triangle, const Segment& segment);

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

Vector3 Perpendicular(const Vector3& vector);

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	/*Segment segment{ {-2.0f,-1.0f,0.0f},{3.0f,2.0f,2.0f} };
	Vector3 point{ -1.5f,0.6f,0.6f };*/
	Vector3 cameraTranslate{ 0.0f,0.0f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };
	Triangle triangle;
	triangle.vertices[0] = { -1.0f,0.0f,0.0f };
	triangle.vertices[1] = { 0.0f,1.0f,0.0f };
	triangle.vertices[2] = { 1.0f,0.0f,0.0f };
	Segment segment{ {-2.0f,0.3f,0.0f},{1.0f,1.0f,1.0f} };

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
		 camelaMatrix = camelaMatrix*Matrix4x4::MakeRotateXMatrix(cameraRotate.x);
		 camelaMatrix = camelaMatrix*Matrix4x4::MakeRotateYMatrix(cameraRotate.y);
		 camelaMatrix = camelaMatrix * Matrix4x4::MakeRotateZMatrix(cameraRotate.z);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::End();

		ImGui::Begin("Triangle");
		ImGui::DragFloat3("Triangle[0]", &triangle.vertices[0].x, 0.01f);
		ImGui::DragFloat3("Triangle[1]", &triangle.vertices[1].x, 0.01f);
		ImGui::DragFloat3("Triangle[2]", &triangle.vertices[2].x, 0.01f);
		ImGui::End();

		ImGui::Begin("Segment");
		ImGui::DragFloat3("Segment.Origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("Segment.Diff", &segment.diff.x, 0.01f);
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
		Vector3 start = Matrix4x4::ScreenTransform(segment.origin, ViewProjectionMatrix, viewportMatrix);
		Vector3 end = Matrix4x4::ScreenTransform(segment.origin + segment.diff, ViewProjectionMatrix, viewportMatrix);

		if (IsColligion(triangle, segment)) {
			Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), RED);
		}
		else {

			Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), WHITE);
		}
		DrawTriangle(triangle, ViewProjectionMatrix, viewportMatrix, WHITE);

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
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return{ -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = Multiply(plane.normal, plane.distance);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normnalize(Perpendicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z };
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z };

	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = perpendiculars[index] * 2.0f;
		Vector3 point = center + extend;
		points[index] = Matrix4x4::ScreenTransform(point, viewProjectionMatrix, viewportMatrix);
	}

	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[0].x), int(points[0].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[1].x), int(points[1].y), color);

}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {

	Vector3 screen[3];
	for (int i = 0; i < 3; i++) {
		Matrix4x4 Matrix = Matrix4x4::MakeAffineMatrix(Vector3{ 1,1,1 }, {}, triangle.vertices[i]);
		Matrix4x4 wvpMatrix = Matrix * viewProjectionMatrix;
		screen[i] = Matrix4x4::ScreenTransform(Vector3{}, wvpMatrix, viewportMatrix);
	}
	Novice::DrawTriangle(int(screen[0].x), int(screen[0].y), int(screen[1].x), int(screen[1].y), int(screen[2].x), int(screen[2].y), color, kFillModeWireFrame);


}

bool IsColligion(const Sphere& sphere, const Plane& plane) {
	float distance = std::abs(Dot(plane.normal, sphere.center) - plane.distance);
	return distance <= sphere.radius;
	//
}
bool IsColligion(const Segment& segment, const Plane& plane) {
	//まず垂直判定を行うために、法線と線の内積を求める
	float dot = Dot(segment.diff, plane.normal);
	//垂直=平行であるので、衝突しているはずがない
	if (dot == 0.0f) {
		return false;
	}
	//tを求める
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	//tの値と線の種類によって衝突しているかを判定する
	if (t >= 0 && t <= 1) {
		return true;
	}
	else {
		return false;
	}
}

bool IsColligion(const Triangle& triangle, const Segment& segment) {

	//三角形から平面を作る
	Vector3 v1 = triangle.vertices[1]- triangle.vertices[0];
	Vector3 v2 = triangle.vertices[2]- triangle.vertices[1];
	Vector3 cross = Cross(v1, v2);
	 cross = Normnalize(cross);
	Vector3 Ncross = Normnalize(cross);
	float distance = Dot(triangle.vertices[0], Ncross);
	float dot = Dot(segment.diff, Ncross);
	if (dot == 0.0f) {
		return false;
	}
	float t = (distance - Dot(segment.origin, Ncross)) / dot;
	Vector3 p; 
	p.x= segment.origin.x + t * segment.diff.x;
	p.y = segment.origin.y + t * segment.diff.y;
	p.z = segment.origin.z + t * segment.diff.z;

	//各辺を結んだベクトルと、頂点と衝突点pを結んだベクトルのクロス積を取る
	Vector3 cross01 = Cross(triangle.vertices[1] -triangle.vertices[0], p-triangle.vertices[1]);
	Vector3 cross12 = Cross(triangle.vertices[2] - triangle.vertices[1],p-  triangle.vertices[2]);
	Vector3 cross20 = Cross(triangle.vertices[0] - triangle.vertices[2], p- triangle.vertices[0]);
	
	if (t >= 0&& t <= 1) {
		//全ての小三角形のクロス
		if (Dot(cross01, Ncross) >= 0.0f &&
			Dot(cross12, Ncross) >= 0.0f &&
			Dot(cross20, Ncross) >= 0.0f) {
			return true;
		}
		else {
			return false;
		}
	}
	
		return false;
	
}