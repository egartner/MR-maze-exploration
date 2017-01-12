#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <stack>
#include <cstring>
#include <algorithm>
#include <ctime>
#include "mazegen.h"

#define SIZE 50
#define N 2505

int dir[4][2] = { {1,0}, {0,1}, {0,-1}, {-1,0} };

int main(int argc, char **argv){
	bMaze *maze = new bMaze();
	maze->init(argc, argv);
	maze->gen();
	maze->printConfig();
	maze->printObstacles();
	delete maze;
}

/**************CLASS CELL***************/
Cell::~Cell(){
}

bool Cell::inRange(Maze *maze){
	return x >= 0 && x < maze->row && y >= 0 && y < maze->column;
}

int Cell::getId(Maze *maze){
	return x*maze->column+y;
}

/************CLASS MAZE**************/
Maze::Maze(){
	PM=0;
	nNode=0;
	nEdge=0;
	wl = new bool**[SIZE]; 
	for(int i = 0; i < SIZE; ++i){
		wl[i] = new bool*[SIZE];
		for(int j = 0 ; j < SIZE ; ++j){
			wl[i][j] = new bool[4];
			for (int k = 0; k < 4 ; ++k){
				wl[i][j][k] = true;
			}
		}
	}
	fa = new int [SIZE*SIZE+5];
}

Maze::~Maze(){
	delete[] fa;
	fa = NULL;
	for(int i = 0; i < SIZE; ++i){
		for(int j = 0; j < SIZE ; ++j){
			delete[] wl[i][j];
			wl[i][j] = NULL;
		}
		delete[] wl[i];
		wl[i] = NULL;
	}
	delete[] wl;
	wl = NULL;
}

void Maze::init(int argc, char **argv) {
	seed = -1;
	row = -1;
	column = -1;
	for (int i = 1; i < argc ; i+=2) {
		if (argv[i][0] == 's')
			seed = atoi(argv[i+1]);
		else if (argv[i][0] == 'r')
			row = atoi(argv[i+1]);
		else if (argv[i][0] == 'c')
			column = atoi(argv[i+1]);
		else if (argv[i][0] == 'p')
			PM = atoi(argv[i+1]);
	}
	if (seed <= 0) seed = time(0);
	if (row <= 0 ) row = 16;
	if (column <= 0 ) column = 16;
	srand(seed);
}

void Maze::printConfig(){
	std::ofstream config;
	config.open("config.xml", fstream::out);
	if (config.is_open()){
		config << "<?xml version=\"1.0\" standalone=\"no\" ?> \n";
		config << "<world gridSize=\"100,100,100\" windowSize=\"1800,900\"> \n";
		config << "<camera target=\"900,900,0\" directionSpherical=\"0,70,500\" angle=\"90\"/> \n";
		config << "<spotlight target=\"200,20,200\" directionSpherical=\"45,60,500\" angle=\"40\"/> \n";
		config << "<blockList color=\"255,255,255\" size=\"1,1,1\" blockSize=\"71,71,65\" > \n";

		config << "</blockList> \n";
		config << "<obstacleList color=\"255,0,0\" > \n";
		config.close();
	}
	else cout << "Unable to open file";
}

/****************CLASS BRAID MAZE**************/
int bMaze::find(int x){
	while (x != fa[x]) {
		fa[x] = fa[fa[x]];
		x = fa[x];
	}
	return x;
}

void bMaze::merge(int p, int q){
	int fp = find(p);
	int fq = find(q);
	if (fp != fq)
		fa[fp] = fq;
}

void bMaze::gen(){
	int sz, tot, id, d;
	Cell a, b;
	wall.clear();
	for (int i = 0 ; i < row ; ++i){
		for (int j = 0 ; j < column ; ++j){
			fa[i*column + j] = i*column + j;
			for (int k = 0 ; k < 2 ; ++k){
				Cell b = Cell(i+dir[k][0], j+dir[k][1]);
				if(b.inRange(this)){
					wall.push_back(Wall( Cell(i,j), b, k ));
				}
			}
		}
	}
	this->nNode = row*column;
	sz = wall.size();
	tot = row*column;
	while (tot != 1){
		id = rand() % sz;
		a = wall[id].a;
		b = wall[id].b;
		d = wall[id].d;
		swap(wall[id], wall[sz-1]);
		sz--;
		if (find(a.getId(this)) != find(b.getId(this))){
			merge(a.getId(this), b.getId(this));
			tot--;
			wl[a.x][a.y][d] = wl[b.x][b.y][3-d] = 0;
			this->nEdge++;
		}
		else if (PM==0){
			wl[a.x][a.y][d] = wl[b.x][b.y][3-d] = 0;
			this->nEdge++;
		}
	}	
}

void bMaze::printObstacles(){
	int i0, j0;
	std::ofstream config;
	config.open("config.xml", std::ofstream::out | std::ofstream::app);
	for (int i = 0; i < row; ++i){
		for (int j = 0; j < column; ++j){
			for (int k = 0 ; k < 2; k++){
				if (wl[i][j][k]){
					i0 = i+dir[k][0];
					j0 = j+dir[k][1];
					config << "<obstacle firstCell=\"" << i << "," << j << ",0\" secondCell=\""<< i0 << "," << j0 <<",0\"/>\n";
				}
			}
		}
	}
	config << "</obstacleList>\n";
	config << "</world>";
	config.close();
}

/**************CLASS PERFECT MAZE***************/
pMaze::pMaze() : Maze(){
	visited = new bool*[N];
	for(int i = 0; i < N; ++i){
		visited[i] = new bool[N];
		for (int j = 0; j < N; ++j)
			visited[i][j]=false;
	}
}

pMaze::~pMaze(){

}

void pMaze::printObstacles(){
	int i0, j0, o_x, o_y;
	std::ofstream config;
	config.open("config.xml", std::ofstream::out | std::ofstream::app);
	for(int i = 0; i < row; ++i){
		for(int j = 0; j < column; ++j){
			for(int k = 0; k < 2; ++k){
				if(wl[i][j][k]){
					i0 = i+dir[k][0];
					j0 = j+dir[k][1];
					config << "<firstCell=\"" << i << "," << j << ",0\" secondCell=\"" << i0 << "," << j0 << ",0\"/>\n";
				}
			}
		}
	}
	config << "</obstacleList>\n";
	config << "</world>";
	config.close();
}

void pMaze::gen(){
	int remain = row*column-1;
	Cell u = Cell(0,0);
	stack<Cell> S;
	vector<int> unvisited;
	setVis(0,0);
	while(remain > 0){
		unvisited.clear();
		for(int i = 0; i < 4; ++i){
			Cell v = Cell(u.x+dir[i][0], u.y+dir[i][1]);
			if (v.inRange(this) && !getVis(v.x, v.y))
				unvisited.push_back(i);
		}
		if(unvisited.empty()){
			u = S.top();
			S.pop();
		}
		else{
			int id = rand() % unvisited.size();
			int d = unvisited[id];
			Cell v = Cell(u.x+dir[d][0], u.y+dir[d][1]);
			wl[u.x][u.y][d] = 0;
			wl[v.x][v.y][3-d] = 0;
			S.push(v);
			setVis(v.x, v.y);
			u = v;
			remain--;
		}
	}
}
