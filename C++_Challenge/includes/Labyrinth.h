/*
 * Author: Hany Hamed
 * Description: This file includes the defitions for Labyrinth.cpp
 * */

#ifndef LABYRINTH_G
#define LABYRINTH_G

#include <string>
#include <vector>
#include <tuple>


using namespace std;


typedef struct cell
{
    int x;
    int y;
}cell;

typedef struct solution
{
    int length;
    vector<cell> path;
}solution;


class Labyrinth
{
    public:
        Labyrinth(string,string);
        solution getLongestSolution();
        vector<solution> getlAllSolutions();
        void solve();



    private:
        void readFile(string);
        void writeFile(string);
        void searchDFS(cell, solution);
        bool visitedCell(cell,solution);
        bool availableCell(cell c);

        int neighbours_moves[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
        string in_file_name;
        string out_file_name;
        vector<vector<char>> map;
        vector<solution> solutions;
        vector<cell> getNeighbours(cell);
        int height;
        int width;


};


#endif