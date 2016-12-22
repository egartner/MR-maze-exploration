#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace std;
class Cell;
class Wall;
class Maze;
class pMaze;

class Cell{
	friend class Maze;
	friend class bMaze;
	friend class pMaze;
	int x, y;
public:
	Cell(){x=0; y=0;};
	Cell(int _x, int _y){x=_x; y=_y;};
	~Cell();
	bool inRange(Maze *maze);
	int getId(Maze *maze);
	int getX(){return x;};
	int getY(){return y;};
};

class Wall{
	friend class Maze;
	friend class pMaze;
	friend class bMaze;
	Cell a, b;
	int d;
public:
	Wall(Cell _a, Cell _b, int _d){a=_a; b=_b; d=_d;};
	~Wall(){};
};


class Maze{
	friend class Cell;
protected:
	int  PM, row, column, seed, nNode, nEdge;
	bool ***wl;
	int *fa;
	vector<Wall> wall;
public:
	Maze();
	virtual ~Maze();
	void init(int argc, char **argv);
	virtual void gen()=0;
	void printEdge(int x, int y);
	virtual void printObstacles()=0;
	void printConfig();
	
};

class bMaze : public Maze{
	friend class Cell;
public:
	bMaze() : Maze() {};
	~bMaze(){};
	int find(int x);
	void merge(int p, int q);
	virtual void gen();
	virtual void printObstacles();
};

class pMaze : public Maze{
	friend class Cell;
	bool **visited;
public:
	pMaze();
	~pMaze();
	virtual void gen();
	virtual void printObstacles();
	bool getVis(int x, int y){return visited[x][y];};
	bool setVis(int x, int y){visited[x][y]=true;};
};
