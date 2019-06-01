#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>

#include "Vision.h"

const int camera_width  = 1280;
const int camera_height = 720;
const int virtual_camera_angle = 30;
unsigned char bkgnd[camera_width*camera_height*3];

// 立方体の頂点位置
static const float positions[8][3] = {
	{ -1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f }
};

// 立方体の面の色
static const float colors[8][3] = {
	{ 0.0f, 0.0f, 0.0f },  // 黒
	{ 1.0f, 0.0f, 0.0f },  // 赤
	{ 0.0f, 1.0f, 0.0f },  // 緑
	{ 0.0f, 0.0f, 1.0f },  // 青
	{ 1.0f, 1.0f, 0.0f },  // イエロー
	{ 0.0f, 1.0f, 1.0f },  // シアン
	{ 1.0f, 0.0f, 1.0f },  // マゼンタ
	{ 1.0f, 1.0f, 1.0f }   // 白
};

// 立方体の面となる三角形の定義
static const unsigned int indices[12][3] = {
	{ 1, 7, 6 }, { 1, 4, 7 },
	{ 2, 5, 7 }, { 2, 7, 4 },
	{ 3, 7, 5 }, { 3, 6, 7 },
	{ 0, 2, 4 }, { 0, 4, 1 },
	{ 0, 1, 6 }, { 0, 6, 3 },
	{ 0, 5, 2 }, { 0, 3, 5 }
};

static const float normals[6][3] = {
  {1.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, -1.0f},
	{0.0f, -1.0f, 0.0f},
	{-1.0f, 0.0f, 0.0f}
};

float sphere_positions[266][3];
unsigned int sphere_indices[528][3];
float sphere_normals[528][3];

float cone_positions[25][3];
unsigned int cone_indices[24][3];
float cone_normals[24][3];

GLfloat lightPosition[4] = {100.0f, 100.0f, 0.0f, 1.0f}; //光源の位置
GLfloat lightDiffuse[3] = {0.4f, 0.4f, 0.4f}; //拡散光
GLfloat lightAmbient[3] = {0.5f, 0.5f, 0.5f}; //環境光
GLfloat lightSpecular[3] = {1.0f, 1.0f, 1.0f}; //鏡面光

float theta = 0;

void createSphereData() {
	//半径1の球
	//緯度方向に12等分、軽度方向に24等分して描画した配列を用意する

	//座標データの入力
	sphere_positions[0][0] = 0.0f;
	sphere_positions[0][1] = -1.0f;
	sphere_positions[0][2] = 0.0f;
	for (int i = -5; i <= 5; i++) {
		float lat = M_PI * i / 12; //緯度
		float y = sin(lat); //y座標
		float local_r = sqrt(1.0f - y*y); //高さyでの断面の円の半径
		for (int j = 0; j < 24; j++) {
			float lon = M_PI * j / 12; //経度
			float x = local_r * cos(lon);
			float z = local_r * sin(lon);
			sphere_positions[1 + (i+5)*24 + j][0] = x;
			sphere_positions[1 + (i+5)*24 + j][1] = y;
			sphere_positions[1 + (i+5)*24 + j][2] = z;
		}
	}
	sphere_positions[265][0] = 0.0f;
	sphere_positions[265][1] = 1.0f;
	sphere_positions[265][2] = 0.0f;

	//順序データの入力
	for (int i = 0; i < 24; i++) {
		sphere_indices[i][0] = 0;
		sphere_indices[i][1] = i+1;
		sphere_indices[i][2] = (i+1) % 24 + 1;
	}
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 24; j++) {
			sphere_indices[i * 48 + 2 * j + 24][0] = i*24 + j + 1;
			sphere_indices[i * 48 + 2 * j + 24][1] = (i+1)*24 + j + 1;
			sphere_indices[i * 48 + 2 * j + 24][2] = i*24 + (j+1)%24 + 1;

			sphere_indices[i * 48 + 2 * j + 25][0] = i*24 + (j+1)%24 + 1;
			sphere_indices[i * 48 + 2 * j + 25][1] = (i+1)*24 + j + 1;
			sphere_indices[i * 48 + 2 * j + 25][2] = (i+1)*24 + (j+1)%24 + 1;
		}
	}
	for (int i = 0; i < 24; i++) {
		sphere_indices[i + 504][0] = 265;
		sphere_indices[i + 504][1] = 241 + (i+1)%24;
		sphere_indices[i + 504][2] = 241 + i;
	}
	//法線データの入力
	for (int i = 0; i < 528; i++) {
		//三角形を作る3点 O,A,B
		float po[3]; //点O
		float pa[3]; //点A
		float pb[3]; //点B
		for (int j = 0; j < 3; j++) {
			po[j] = sphere_positions[sphere_indices[i][0]][j];
			pa[j] = sphere_positions[sphere_indices[i][1]][j];
			pb[j] = sphere_positions[sphere_indices[i][2]][j];
		}
		float oa[3]; //ベクトルOA
		float ob[3]; //ベクトルOB
		for (int j = 0; j < 3; j++) {
			oa[j] = pa[j] - po[j];
			ob[j] = pb[j] - po[j];
		}
		//法線ベクトル N = OB * OA
		float standard = 0;
		for (int j = 0 ; j < 3; j++) {
			int jp = (j+1) % 3;
			int jpp = (j+2) % 3;
			sphere_normals[i][j] = oa[jp]*ob[jpp] - oa[jpp]*ob[jp];
			standard += sphere_normals[i][j] * sphere_normals[i][j];
		}
		//法線ベクトルの正規化
		if (standard != 0) {
			standard = sqrt(standard);
			for (int j = 0 ; j < 3; j++) {
				sphere_normals[i][j] /= standard;
			}
		}
		
	}
}

void createConeData() {
	//円錐のデータセットを作成する
	//底面が半径1の円、高さが1

	//頂点データの入力
	cone_positions[0][0] = 0;
	cone_positions[0][1] = 1;
	cone_positions[0][2] = 0;
	float local_theta = 0;
	for (int i = 1; i < 25; i++) {
		local_theta = M_PI * (i-1) / 12;
		float x = cos(local_theta);
		float z = sin(local_theta);
		cone_positions[i][0] = x;
		cone_positions[i][1] = 0;
		cone_positions[i][2] = z;
	}

	//順序データの入力
	for (int i = 0; i < 24; i++) {
		cone_indices[i][0] = 0;
		cone_indices[i][1] = (i+1)%24 + 1;
		cone_indices[i][2] = i+1;
	}
}

void drawSphere(int color) {
	glBegin(GL_TRIANGLES);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colors[color]);
	glMaterialfv(GL_FRONT, GL_AMBIENT, colors[color]);
	for (int i = 0; i < 528; i++) {
		glNormal3fv(sphere_normals[i]);
		for (int j = 0; j < 3; j++) {
			glVertex3fv(sphere_positions[sphere_indices[i][j]]);
		}
	}
	glEnd();
}

void drawCone(int color) {
	glBegin(GL_TRIANGLES);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colors[color]);
	for (int i = 0; i < 24; i++) {
		for (int j = 0; j < 3; j++) {
			glVertex3fv(cone_positions[cone_indices[i][j]]);
		}
	}
	glEnd();
}

void drawCube() {
	glBegin(GL_TRIANGLES);
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colors[7]);
	for(int face = 0; face < 6; face++){
		glNormal3fv(normals[face]);
		for(int i = 0; i < 3; i++){
			glVertex3fv(positions[indices[face*2 + 0][i]]);
		}
		glNormal3fv(normals[face]);
		for(int i = 0; i < 3; i++){
			glVertex3fv(positions[indices[face*2 + 1][i]]);
		}
	}
	glPopMatrix();
	glEnd();
}

void drawSnowman() {
	float head_size = 0.7f;
	float mouse_size = 0.4f;
	//雪だるま・目
	glPushMatrix();
	glTranslatef(0.0f,head_size, 0.0f);
	glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(0.7f*sin(M_PI/8.0f), 0.0f, 0.7f*cos(M_PI/8.0f));
	glScalef(0.1f, 0.1f, 0.1f);
	drawSphere(0);
	glPopMatrix();
	glTranslatef(0.7f*sin(-M_PI/8.0f), 0.0f, 0.7f*cos(-M_PI/8.0f));
	glScalef(0.1f, 0.1f, 0.1f);
	drawSphere(0);
	glPopMatrix();

	//雪だるま・口
	float mouse_dist = sqrt(head_size*head_size - mouse_size*mouse_size);
	glPushMatrix();
	glTranslatef(0.0f,head_size, mouse_dist);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	for (int i=0; i<6; i++) {
		float rad = -5 * M_PI / 16 + i * M_PI / 8;
		glPushMatrix();
		glTranslatef(mouse_size*sin(rad), 0.0f, mouse_size*cos(rad));
		glScalef(0.05f, 0.05f, 0.05f);
		drawSphere(0);
		glPopMatrix();
	}
	glPopMatrix();

	//雪だるま・鼻
	glPushMatrix();
	glTranslatef(0.0f, head_size - 0.15f, head_size);
	glScalef(0.15f, 0.3f, 0.15f);
	drawCone(1);
	glPopMatrix();

	//雪だるま・頭部
	glPushMatrix();
	glTranslatef(0.0f, head_size, 0.0f);
	glScalef(head_size, head_size, head_size);
	drawSphere(7);
	glPopMatrix();

	//雪だるま・胴体
	glPushMatrix();
	glTranslatef(0.0, -0.3f, 0.0f);
	// glScalef(0.85f, 0.85f, 0.85f);
	glScalef(0.5f, 0.5f, 0.5f);
	drawSphere(7);
	glPopMatrix();
}

void display(GLFWwindow* window, const cv::Mat &img_camera, std::vector<Marker> &markers) {
  const int camera_image_size = sizeof(unsigned char)*img_camera.rows*img_camera.cols * 3;
  int background_buffer_size = sizeof(bkgnd);
  memcpy(bkgnd, img_camera.data, background_buffer_size);

  int width0, height0;
  glfwGetFramebufferSize(window, &width0, &height0);

	//clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

  glDisable(GL_DEPTH_TEST);

  // draw background image
  glDisable( GL_DEPTH_TEST );

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0, camera_width, 0.0, camera_height );

  glRasterPos2i( 0, camera_height-1 );
  glDrawPixels( camera_width, camera_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bkgnd );

  glPopMatrix();

  glEnable(GL_DEPTH_TEST);

  glMatrixMode( GL_MODELVIEW );

	gluLookAt(5.0f*sin(theta), 0.0f, 5.0f*cos(theta), //視点の位置
						0.0f, 0.0f, 0.0f, //見ている方向
						0.0f, 1.0f, 0.0f); //視点の上方向

	drawSnowman();

	glPushMatrix();
	glTranslatef(2.0f, 0.0f, 0.0f);
	glRotatef(theta, 1.0f, 1.0f, 1.0f);
	theta += M_PI / 800;
	// drawCube();
	glPopMatrix();

  int key = cv::waitKey (10);
	if (key == 27) exit(0);
}

void initGL() {
  glPixelStorei(GL_PACK_ALIGNMENT, 1);// for glReadPixels
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // for glTexImage2D
	glPixelZoom(1.0, -1.0);

  glEnable( GL_COLOR_MATERIAL );
  glClearColor( 0, 0, 0, 1.0 );

  glEnable( GL_DEPTH_TEST );
	glClearDepth( 1.0 );

  GLfloat light_pos[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	GLfloat light_amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  GLfloat light_dif[] = { 0.7f, 0.7f, 0.7f, 1.0f };

  glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
	glLightfv( GL_LIGHT0, GL_AMBIENT,  light_amb );
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  light_dif );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
}

void reshape( GLFWwindow* window, int width, int height ) {

	// set a whole-window viewport
	glViewport( 0, 0, (GLsizei)width, (GLsizei)height );

	// create a perspective projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 30, ((GLfloat)width/(GLfloat)height), 0.01, 100 );

	// invalidate display
	//glutPostRedisplay();
}

int main(){
  Vision vision;
  vision.init();
  GLFWwindow* window;

  if(!glfwInit()) return -1;

  window = glfwCreateWindow(camera_width, camera_height, "Hole-in-one", NULL, NULL);
  if(!window) {
    glfwTerminate();
    return -1;
  }

	glfwSetFramebufferSizeCallback(window, reshape);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  reshape(window, window_width, window_height);

  glViewport(0, 0, window_width, window_height);

  initGL();

	createSphereData();
	createConeData();

  // while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&glfwWindowShouldClose(window) == GL_FALSE) {
  while(glfwWindowShouldClose(window) == GL_FALSE) {
		//カメラの更新とマーカー情報の更新
    vision.updateCamera();
    vision.detectMarker();
		display(window, vision.img_camera, vision.marker_list);
		//描画バッファの切り替え
    glfwSwapBuffers(window);
		glfwPollEvents();
  }

  glfwTerminate();
	return 0;
}