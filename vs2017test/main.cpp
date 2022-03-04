#include "glut.h" // open gl library for windows GUI
#include <time.h>
#include <vector>
#include "Cell.h"
#include <iostream>

using namespace std;

/*

black - visited vertex
grey - non visited neighbours vertexes of black vertex
white  - non visited vertex

BFS Complexity:
b - branching factor
d - depth
Time Complexity - O(b^d)
Place Complexity - O(b^d)
 ___________________________________
| _____ |   | ___ | ___ ___ | |   | |
| |   | |_| |__ | |_| __|____ | | | |
| | | |_________|__ |______ |___|_| |
| |_|   | _______ |______ |  | ___ _|
| ___ | |____ | |______ | |_| |____ |
|___|_|____ | |   ___ | |________ | |
|   ________| | |__ | |______ | | | |
| | | ________| | __|____ | | | __| |
|_| |__ |   | __|__ | ____| | |_| __|
|   ____| | |____ | |__ |   |__ |__ |
| |_______|_______|___|___|___|_____|

*/

const int MSZ = 100; // maze size

const int WALL = 0;
const int SPACE = 1;
const int START = 2;
const int TARGET = 3;

const int BLACK = 4;
const int GRAY = 5;
const int PATH = 6;

int maze[MSZ][MSZ] = {0};
vector<Cell*> grays; // list of grey vertexes
bool startBFS = false; // for idle function
void initMaze();


void initMaze() {

	int i, j;

	for (int i = 1; i < MSZ-1; i++)
	{
		for (int j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 0) { // even rows - mainly walls

				if (rand()%100 > 35)
					maze[i][j] = WALL;
				else
					maze[i][j] = SPACE;

			}
			else { 	// odd - mainly spaces

				if (rand() % 100 > 20)
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
		}
	
	}
	maze[MSZ / 2][MSZ / 2] = START;
	Cell* ps = new Cell(MSZ / 2, MSZ / 2,nullptr); // start vertex
	grays.push_back(ps); // add start vertex to end of the vector
	maze[rand() % MSZ][rand() % MSZ] = TARGET;

}
void restorePath(Cell* ps) {

	while (ps != nullptr)
	{
		maze[ps->getRow()][ps->getCol()] = PATH;
		ps = ps->getParent();
	}


}
void showMaze() {

	int i, j;

	for (i = 0; i < MSZ; i++)
	{
		for (j = 0; j < MSZ; j++)
		{
			// set color for cell [i][j]
			switch (maze[i][j]) {

			case WALL:
				glColor3d(0.5, 0, 0); // green
				break;

			case SPACE:
				glColor3d(1, 1, 1); // black
				break;

			case START:
				glColor3d(0.5, 1, 1); // yellow
				break;

			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
				
			case BLACK:
				glColor3d(1, 1, 0); // yellow
				break;

			case GRAY:
				glColor3d(1, 0, 1); // purple
				break;

			case PATH:
				glColor3d(0, 0.7, 0); // orange
				break;
			}
			/*
			NOTE:
			  i+1,j  _______  i+1,j+1
					|       |
					|       |
					|       |
				i,j	|_______| i,j+1

			*/
			// display cell as polygon

			glBegin(GL_POLYGON);
				glVertex2d(j, i);
				glVertex2d(j, i + 1);
				glVertex2d(j + 1, i + 1);
				glVertex2d(j + 1, i);
			glEnd();


		}

	}
}
void checkNeighbor(Cell* pCurrent, int row, int col) { // gets te pointer to current cell and the coordinates of its neighbor 

	// check if the neigbor is not the target
	if (maze[row][col] == TARGET) {

		startBFS = false;
		cout << "The solution has been found\n";
		restorePath(pCurrent);
	}
	else { // paint this neighbor gray and add it to the queue

		Cell* ps = new Cell(row, col, pCurrent);
		grays.push_back(ps);
		maze[row][col] = GRAY;
	}

}
void BFSIteration() {
	
	Cell* pCurrent;
	int row, col; // current row&col

	//check the grays queue
	if (grays.empty()) {
		
		startBFS = false;
		cout << "There is no solution\n";
	}
	else {

		// pick and remove from the queue the first cell and check its neighbors
		pCurrent = *(grays.begin());
		grays.erase(grays.begin());
		//paint it black
		row = pCurrent->getRow();
		col= pCurrent->getCol();
		maze[row][col] = BLACK;
		
		// scan all the (white) neighbors or target vertex and add them to queue (if it's not a target)
		
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET) // check UP
			checkNeighbor(pCurrent,row+1,col);

		if (startBFS && (maze[row -1][col] == SPACE || maze[row - 1][col] == TARGET)) // check DOWN
			checkNeighbor(pCurrent, row - 1, col);

		if (startBFS && (maze[row][col-1] == SPACE || maze[row][col-1] == TARGET)) // check LEFT
			checkNeighbor(pCurrent, row, col-1);

		if (startBFS && (maze[row ][col+1] == SPACE || maze[row][col+1] == TARGET)) // check RIGHT
			checkNeighbor(pCurrent, row , col+1);
	
	}

}

void menu(int choice)
{
	if (choice == 1) //BFS
	{
		startBFS = true;
	}
	else {
		startBFS = false;
		grays.clear();
		initMaze();
	}
}
void init()
{	//params: red,green,blue,alpha(color clarity)
	glClearColor(0, 0, 0.4, 0);// color of window background

	// set the main axes
	glOrtho(0,MSZ, 0, MSZ,-1,1); // no depth  - maze is 2D

	srand(time(0)); // random seed

	initMaze();
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer OF previous images

	// drawings logic -  all we want to display
	
	showMaze();

	glutSwapBuffers(); // show all
}
void idle()
{
	if(startBFS)
		BFSIteration();
	glutPostRedisplay(); // indirect call to refresh function
}


void main(int argc, char* argv[])
{
	glutInit(&argc, argv); // initialize 
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // set the window to corespond with RGB & double buffer to manage 2 and more images (for animation)
	glutInitWindowSize(600, 600); // set window scale
	glutInitWindowPosition(200, 100); // set window position on the screen
	glutCreateWindow("Unimformed Search - BFS Example"); // set title to the window

	glutDisplayFunc(display); // display function call to refresh the window 
	glutIdleFunc(idle); // running all the time when nothing happening

	glutCreateMenu(menu); // menu creation 
	glutAddMenuEntry("Run BFS",1); // if this option selected -->  it will sends 1
	glutAddMenuEntry("Randomize the maze", 2); 
	glutAttachMenu(GLUT_RIGHT_BUTTON); // right buuton of mouse


	init(); // call to set the background colors
	 
	glutMainLoop(); // starts window queue of the events
}