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

void drawSphere(double r, int lats, int longs) {
	int i, j;
	for(i = 0; i <= lats; i++) {
		double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
		double z0  = r * sin(lat0);
		double zr0 = r *  cos(lat0);

		double lat1 = M_PI * (-0.5 + (double) i / lats);
		double z1  = r * sin(lat1);
		double zr1 = r * cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) {
			double lng = 2 * M_PI * (double) (j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f((GLfloat)(x * zr0), (GLfloat)(y * zr0), (GLfloat)z0);
			glVertex3f((GLfloat)(x * zr0), (GLfloat)(y * zr0), (GLfloat)z0);
			glNormal3f((GLfloat)(x * zr1), (GLfloat)(y * zr1), (GLfloat)z1);
			glVertex3f((GLfloat)(x * zr1), (GLfloat)(y * zr1), (GLfloat)z1);
		}
		glEnd();
	}
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
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0, camera_width, 0.0, camera_height);

  glRasterPos2i(0, camera_height-1);
  glDrawPixels(camera_width, camera_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bkgnd);

  glPopMatrix();

  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_MODELVIEW);

	gluLookAt(0.0f, 0.0f, 5.0f, //視点の位置
						0.0f, 0.0f, 0.0f, //見ている方向
						0.0f, 1.0f, 0.0f); //視点の上方向

  for (Marker marker: markers) {
    std::cout << "No." << marker.code << std::endl;
    glPushMatrix();
    float transposed[16];
    for (int x=0; x<4; ++x)
		  for (int y=0; y<4; ++y)
			  transposed[x*4+y] = marker.resultMatrix[y*4+x];
    // glLoadMatrixf(marker.resultMatrix);
    glScalef(0.5, 0.5, 0.5);
    drawSphere(0.8, 10, 10);
    glPopMatrix();
    std::cout << std::endl;
  }

  int key = cv::waitKey (10);
	if (key == 27) exit(0);
}

void initSetting() {
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

  initSetting();

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