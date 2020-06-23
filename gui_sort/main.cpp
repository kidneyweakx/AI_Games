/*
 * @description: OpenGL Visualize Sorting Algorithm
 * @author : kidneyweakx
 */
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h> // OpenGL header
#include <string> // std::string 
#include <random>
#include <time.h>
#include <thread>
#include <cstdlib>
#include "usleep.h"

 // defining global variables
const int N = 1000;
int vec[N], compares, exchanges;
std::string sortName;
float width = 1500, height = 900;

// OpenGL functions
void init();
void display();
void visual(int* arr);
void vprintf(std::string s);
void drawBitmapString(float x, float y, std::string s);
/* define sorting thread */
#include"sort.h"
void mergearr(int* arr1, int* arr2, int n1, int n2, int* arr);
void quickthread(int* arr, int num) {
	quickSort quick(arr, num);
}
void quickthread2(int* arr, int num) {
	quickSort quick(arr, num, true);
}
void mergethread(int* arr1, int* arr2, int num1, int num2, int* arr) {
	mergearr(arr1, arr2, num1, num2, arr);
}

/* main function */
int main(int argc, char** argv) {

	// Generating random array within the range of N
	srand((unsigned)time(NULL));
	for (int i = 0; i < N; i++) {
		vec[i] = (rand() % 600);
	}

	// glut settings
	glutInit(&argc, argv);// initializing glut
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 100);
	glutCreateWindow("Sorting Visualization");// name of window

	glutDisplayFunc(display); // call back function
	init(); // initializing GL
	gluOrtho2D(0, width, 0, height); // define position
	glutMainLoop(); // infinite processing loop

	system("pause");
	return 0;
}

/* custom OpenGL Initialisation */
void init() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
/* The display function - choose which sort what you want*/
void display() {
	double START, END, s1, s2, s3;
	visual(vec);
	int arr[N];
	FILE* stream;
	stream = fopen("execoutput.txt", "w+");
	/* slow sort
	sortName = "selection sort";
	compares = 0, exchanges = 0;
	for (int i = 0; i < N; i++)
		arr[i] = vec[i];
	selectionSort sel(arr, N);
	Sleep(1);
	
	compares = 0, exchanges = 0;
	sortName = "insert sort";
	for (int i = 0; i < N; i++)
		arr[i] = vec[i];
	insertionSort ins(arr, N);
	Sleep(1);
	*/
	for (int i = 0; i < N; i++)
		arr[i] = vec[i];
	START = clock();
	compares = 0, exchanges = 0;
	sortName = "quick sort";
	quickSort quick(arr, N);
	END = clock();

	s1 = (END - START) / 1000.0;
	printf("quicksorting exclusive time %fsec\n", s1);
	vprintf("quick sorting exclusive time " + std::to_string(s1));
	fprintf(stream, "quicksorting exclusive time %fsec\n", s1);
	Sleep(1);


	for (int i = 0; i < N; i++)
		arr[i] = vec[i];
	START = clock();
	compares = 0, exchanges = 0;
	sortName = "merge sort";

	mergeSort merge(arr, N);
	END = clock();
	s2 = (END - START) / 1000.0;
	printf("merge sorting exclusive time %fsec\n", s2);
	vprintf("merge sorting exclusive time " + std::to_string(s2));

	fprintf(stream, "mergesorting exclusive time %fsec\n", s2);
	Sleep(1);



	int* aux1 = NULL;
	int* aux2 = NULL;
	std::thread t[3];
	for (int i = 0; i < N / 2; i++) {
		aux1 = (int*)realloc(aux1, sizeof(int) * (i + 1));
		aux1[i] = vec[i];
	}
	for (int i = 0; i < N / 2; i++) {
		aux2 = (int*)realloc(aux2, sizeof(int) * (i + 1));
		aux2[i] = vec[i + (N / 2)];
	}
	START = clock();
	vprintf("thread sorting start processing...");
	compares = 0, exchanges = 0;
	sortName = "mt sort";

	t[0] = std::thread(quickthread, aux1, N / 2);
	t[1] = std::thread(quickthread2, aux2, N / 2);
	t[0].join();
	t[1].join();
	//mergethread(aux1, aux2, N / 2, N / 2, arr);
	t[2] = std::thread(mergethread, aux1, aux2, N / 2, N / 2, arr);
	t[2].join();
	END = clock();
	s3 = (END - START) / 1000.0;
	printf("thread sorting exclusive time %fsec\n", s3);
	vprintf("thread sorting exclusive time " + std::to_string(s3));
	fprintf(stream, "thread sorting exclusive time %fsec\n", s3);
	fclose(stream);
	Sleep(1);
	glutDestroyWindow(glutGetWindow());
}


void vprintf(std::string s) {
	glClear(GL_COLOR_BUFFER_BIT);
	drawBitmapString(width / 5 * 1, height / 2, s);
	glFlush();
	usleep(1000000);
}
// visualize arr bar
void visual(int* arr) {
	glClear(GL_COLOR_BUFFER_BIT); // sets the previous background with the current background
	drawBitmapString(width / 2, height / 20 * 18, sortName);
	drawBitmapString(width / 2, height / 20 * 17, "Compare times:" + std::to_string(compares));
	drawBitmapString(width / 2, height / 20 * 16, "Swap times:" + std::to_string(exchanges));
	drawBitmapString(width / 2, height / 20 * 15, "the lower the better");
	glColor3f(1.0f, 1.0f, 0.0f); //red
	// drawing bars on the screen by diagonal coordinates
	for (int i = 0; i < N; i++) {
		glRecti(4 * i, 100, 4 * i + 3, 100 + arr[i]);
	}
	glFlush();
	usleep(150);
}

void drawBitmapString(float x, float y, std::string s) {
	glColor3f(1.0f, 1.0f, 1.0f); // sets color of the text
	glRasterPos2f(x, y);// screen coordinate at which text will appear
	int length = s.size();
	// writing charater by charater on the screen
	for (int i = 0; i < length; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
	}
}

void mergearr(int* arr1, int* arr2, int n1, int n2, int* arr) {// a1 and a2 as total of numbers contain in its array
	visual(arr);
	int currenta = 0, currentb = 0, currentc = 0;
	while (currenta != n1 || currentb != n2) {// while index doesn't equal to its max   
		if (currenta == n1 && currentb != n2) {// when index of arr1 reach its max
			for (int i = currentb; i < n2; i++) {// sort for value in arr2
				arr[currentc] = arr2[currentb];
				currentb = currentb + 1;
				currentc = currentc + 1;
			}
			visual(arr);
		}
		else if (currenta != n1 && currentb == n2) {// when index of arr2 reach its max
			for (int i = currenta; i < n1; i++)// sort for value in arr1
			{
				arr[currentc] = arr1[currenta];
				currenta = currenta + 1;
				currentc = currentc + 1;
			}
			visual(arr);
		}
		else {
			if (arr1[currenta] < arr2[currentb]) {// if value in arr1 smaller than arr2
				arr[currentc] = arr1[currenta];// put smallest value into a new array from arr1 and 2
				currenta = currenta + 1;// move to next index
				currentc = currentc + 1;
			}
			else {
				arr[currentc] = arr2[currentb];
				currentb = currentb + 1;
				currentc = currentc + 1;
			}
			visual(arr);
		}
		visual(arr);
	}
}
