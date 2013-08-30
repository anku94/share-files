

#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstdlib>
#include<GL/glut.h>
#include<GL/glu.h>
#include<GL/gl.h>
using namespace std;

double eye_x, eye_y, eye_z;
double dir_x, dir_y, dir_z;
double hor_angle, ver_angle;
double car_angle;
double radius;
double height;
double rotation_angle ;
double delta = 0.01;
double count;

void changeSize(int w, int h) {
   glClearColor(0,0,0,1);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glViewport(0, 0, w, h);
   gluPerspective(45,w/(float)h,1,1000);
   glMatrixMode(GL_MODELVIEW);
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_DEPTH_TEST);
   gluLookAt(eye_x, eye_y, eye_z, eye_x + dir_x, eye_y + dir_y, eye_z + dir_z, 0, 1, 0);
}

void draw_cuboid( double size_x = 10, double size_y = 10, double size_z = 10, double centre_x = 0, double centre_y = 0, double centre_z = 0) {
   glPushMatrix();
   glTranslatef(centre_x, centre_y, centre_z);
   glScaled( size_x , size_y , size_z  );
   glBegin( GL_TRIANGLE_STRIP);
   // cube using minimum vertices (14)
   glVertex3i( -1,  1, -1 );
   glVertex3i(  1,  1, -1 );
   glVertex3i( -1, -1, -1 );
   glVertex3i(  1, -1, -1 );
   glVertex3i(  1, -1,  1 );
   glVertex3i(  1,  1, -1 );
   glVertex3i(  1,  1,  1 );
   glVertex3i( -1,  1, -1 );
   glVertex3i( -1,  1,  1 );
   glVertex3i( -1, -1, -1 );
   glVertex3i( -1, -1,  1 );
   glVertex3i(  1, -1,  1 );
   glVertex3i(  1, -1,  1 );
   glVertex3i(  1, -1,  1 );
   glEnd();
   glPopMatrix();
} 
void draw_cylinder( double rotation_angle = 0,
      double radius = 5, double height = 30, 
      double centre_x = 0, double centre_y = 0, double centre_z = 0) {
   glPushMatrix();
   glRotated( rotation_angle, 0, 0, 1 );
   glTranslatef(centre_x, centre_y, centre_z);
   glScaled( radius, height , radius );
   glBegin( GL_TRIANGLE_STRIP);
   for ( double i = -1; i < 1; i += 0.1 ) {
      glVertex3d( i,  1, sqrt( 1 - i * i) );
      glVertex3d( i, -1, sqrt( 1 - i * i) );
   }
   for ( double i = 1; i > -1; i -= 0.1 ) {
      glVertex3d( i,  1, -sqrt( 1 - i * i) );
      glVertex3d( i, -1, -sqrt( 1 - i * i) );
   }
   glEnd();

   glPopMatrix();
}

class Dog {
   double centre_x, centre_y, centre_z;
   double facing_angle;
   double torso_length, torso_width, torso_height;
   double head_length, head_width, head_height;
   double ear_length, ear_width, ear_height;
   double leg_bent_angle[2][2];
   double leg_delta[2][2];
   double thigh_length, thigh_radius;
   double leg_length, leg_radius;
   double tail_length, tail_radius, tail_beta_angle, tail_phi_angle;
   double head_angle;
   public :
   Dog() {
      centre_z = centre_y = centre_x = 0;
      facing_angle = 0;
      leg_radius = 10;
      leg_length = 10;
      thigh_radius = 3;
      thigh_length = 8;
      head_length = head_height = head_width = 10;
      ear_length = ear_height = ear_width = 4;
      torso_length = 25;
      torso_height = 10;
      torso_width = 15;
      head_angle = 0;
      leg_bent_angle[0][0] = 10;leg_delta[0][0] = 0.01;
      leg_bent_angle[0][1] = 0; leg_delta[0][1] = 0.01;
      leg_bent_angle[1][0] = 0; leg_delta[1][0] = 0.01;
      leg_bent_angle[1][1] = 10;leg_delta[1][1] = 0.01;
      tail_length = 15;
      tail_radius = 3;
      tail_phi_angle = 0;
      tail_beta_angle = 90;

   }
   void draw() {
      glTranslatef(0.01,0,0);
      glPushMatrix();
      glScaled(2,2,2);
      /* Draw the torso first */
      glColor3d(0.4, 0.4, 0.8);
      draw_cuboid( torso_length, torso_height, torso_width );
      glPushMatrix();

      /* Move towards the face */
      glTranslated( torso_length, torso_height, 0);

      /* Rotate the face */
      glRotated( head_angle , 0,0,1);

      /* Move towards the centre of the head and draw the head */
      glTranslated( head_length/3, head_height/3, 0);

      //glColor3f( 0.4, 0.4, 0.4);
      // Neck
      draw_cuboid( head_length/2, head_height/2, head_width/2 );
      glTranslated( head_length/2, head_height/2, 0);

      // Head
      glColor3d(0.4, 0.4, 0.6);
      glutSolidSphere(head_length,30,30);
      glTranslated( head_length, 0, 0);
      draw_cylinder( 90, 5,10);
      glTranslated( -head_length, 0, 0);

      // Ear
      glTranslated( -2 * ear_length, 2*ear_height, 2 * ear_width);
      for(int i = -1; i <= 1; i+= 2) {
	 draw_cuboid( ear_length, ear_height, -ear_width);
	 glTranslated(0, 0, -4*ear_width);
      }
      glTranslated(  ear_length, -ear_height, - ear_width);
      glColor3d(0,0,0);
      glutSolidSphere(40,0,0);
      glPopMatrix();

      /* Move to the thigh */
      glColor3f( 0.2, 0.3, 0.7);
      int ii,jj;
      for(int i = -1; i <= 1; i += 2) {
	 for(int j = -1; j <= 1; j += 2) {
	    ii = i + 1 >>1;
	    jj = j + 1 >>1;
	    glPushMatrix();
	    glTranslated( i* torso_length/2, -torso_height/2, j*torso_width);

	    glTranslated( 0, -thigh_length, 0);

	    draw_cylinder( leg_bent_angle [ii][jj], thigh_radius, thigh_length);

	    glTranslated(0, -1.5*thigh_length, 0);
	    draw_cylinder( -leg_bent_angle[ii][jj], thigh_radius, thigh_length);
	    leg_bent_angle[ii][jj] +=  leg_delta[ii][jj];
	    if ( leg_bent_angle[ii][jj] >= 15) leg_delta[ii][jj] *= -1;
	    if ( leg_bent_angle[ii][jj] <= 0) leg_delta[ii][jj] *= -1;
//	    cout << leg_bent_angle << endl;
	    glPopMatrix();
	 }
      }
      glTranslated( -torso_length, torso_height, 0);
      glRotated(tail_beta_angle, 0,0,1);
      glRotated(tail_phi_angle, 1,0,0);
      glTranslated( 0,tail_length,0);
      draw_cylinder(0, tail_radius, tail_length);
      glPopMatrix();
      head_angle += delta;
      tail_beta_angle += delta;
      tail_phi_angle += delta;
      count ++ ;
      if( count == 2000) {
	 count = -2000;
	 delta = -delta;
      }

   }


}dog;

void Draw() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glColor3f(1,0,1);
   /*
   glBegin( GL_QUADS);
   glTexCoord2i(0,0);
   glVertex3f(-500,  -.01,-500);
   glTexCoord2i(1,0);
   glVertex3f( 500,  -.01,-500);
   glTexCoord2i(1,1);
   glVertex3f( 500,  -.01, 500);
   glTexCoord2i(0,1);
   glVertex3f(-500,  -.01, 500);
   glEnd();
   */
   dog.draw();



   glutSwapBuffers();
}

void start_camera() {
   dir_x = dir_y = 0;
   dir_z = -1;
   eye_z = 600;
   eye_y = 100;
   eye_x = 100;
   hor_angle = 0;
   ver_angle = 0;
   car_angle = 0;
}
void move_viewpoint(unsigned char key, int x, int y) {
   int sign = 0;
   switch(key) {
      case 'r': start_camera(); break;
      case 'a': hor_angle -= 0.01;  break;
      case 'd': hor_angle += 0.01;  break;
      case 'w': sign = 1;  break;
      case 's': sign = -1;  break;
      case 'z': ver_angle += 0.01; break;
      case 'c': ver_angle -= 0.01; break;
//    case 'a': eye_z -= 0.01;  break;
//    case 'a': eye_z -= 0.01;  break;
   }
   dir_x = sin(hor_angle);
   dir_y = cos(hor_angle) * sin(ver_angle);
   dir_z = -cos(hor_angle) * cos(ver_angle);
   eye_x += sign*dir_x * 10;
   eye_y += sign*dir_y * 10;
   eye_z += sign*dir_z * 10;
   glLoadIdentity();
   gluLookAt( eye_x, eye_y, eye_z, eye_x + dir_x, eye_y + dir_y , eye_z + dir_z , 0, 1, 0); 

}

int main(int argc, char **argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowPosition(10,10);
   glutInitWindowSize(800,600);
   glutCreateWindow("AIBO");
   glutSpecialFunc(NULL);
   glutKeyboardFunc(move_viewpoint);
   glutDisplayFunc(Draw);
   glutIdleFunc(Draw);
   start_camera();
   glutReshapeFunc(changeSize);
   glutMainLoop();
}

