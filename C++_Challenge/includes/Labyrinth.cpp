/*
 * Author: Hany Hamed
 * Description: This file includes the implementation of Labyrinth.h
 * */

#include "Labyrinth.h"
#include <tuple>
#include <iostream>
#include <fstream>


using namespace std;

/**
 * Constructor for the Labyrinth Class
 * Initialize the class variables (in_file_name, out_file_name, width, height of the map)
 *
 * @param in_file_name it is the input file name
 * @param out_file_name it is the output file name
 */
Labyrinth::Labyrinth(string in_file_name, string out_file_name): 
    in_file_name(in_file_name), out_file_name(out_file_name)
{
    width = 0;
    height = 0;
}

/**
 * Read the input file and add it to the map which is 2D vector
 *
 * @param file_name the name of the input file
 */
void Labyrinth::readFile(string file_name)
{
    ifstream fin (file_name);
    if(!fin.is_open()){
        printf("File %s does not exist\n", file_name.c_str());
        return;
    }

    string row;
    while(getline(fin, row)){
        width = row.length();
        vector<char> row_vector;

        for(int i = 0; i < width; i++)
            row_vector.push_back(row[i]);
        
        map.push_back(row_vector);
        height++;
    }
}

/**
 * Write to the output file the result of the exercise
 *
 * @param file_name the name of the output file
 */
void Labyrinth::writeFile(string file_name)
{

    ofstream fout (file_name);
    if(!fout.is_open()){
        printf("File %s does not exist\n", file_name.c_str());
        return;
    }

    solution final_solution = getLongestSolution();

    for(int i = 0; i < final_solution.length; i++){
        map[final_solution.path[i].y][final_solution.path[i].x] = char(i + 1 + '0');
    }
    // Write the soltion
    fout<<final_solution.length<<"\n";
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            fout<<map[y][x];
        }
        fout<<"\n";
    }


}

/**
 * Get the longest solution from the set of all solutions
 *
 * @return the longest solution
 */
solution Labyrinth::getLongestSolution()
{
    int mx_index = 0;
    int mx_length = 0;
    for(int i = 0; i < solutions.size(); i++){
        if(mx_length < solutions[i].length){
            mx_length = solutions[i].length;
            mx_index = i;
        }
    }
    return solutions[mx_index];
    
}

/**
 * Get all the solutions that has been found
 *
 * @return all the solutions in a vector of solution form
 */
vector<solution> Labyrinth::getlAllSolutions()
{
    return solutions;
}

/**
 * Check if the given cell is being visited or not
 *  By checking its existence in the current solution or not
 *
 * @param c: cell it is the cell to be checked
 * @param sol: solution it is the current reached solution (path)
 * @return boolean true: visited, false: not visited
 */
bool Labyrinth::visitedCell(cell c, solution sol)
{
    for(int i = 0; i < sol.length; i++)
        if(c.x == sol.path[i].x && c.y == sol.path[i].y)
            return true;
    
    return false;
}

/**
 * Check if the current cell is available to be next cell or not
 *  As available means it is not boarder of the map (or out of the grid)
 *
 * @param c: cell the cell to be checked
 * @return boolean true: available, false: not available
 */
bool Labyrinth::availableCell(cell c)
{
    if(map[c.y][c.x] == '#')
        return false;
    return true;
}


/**
 * Get all the neighbours for the cell
 *  By checking all the possibile movements and checking its availability (not being boarder or out of the grid)
 *
 * @param root: cell the cell to get all its neighbour
 * @return vector of the neighbour cells
 */
vector<cell> Labyrinth::getNeighbours(cell root)
{
    vector<cell> neighbours;
    for(int i = 0; i < sizeof(neighbours_moves)/sizeof(neighbours_moves[0]); i++){  // it is flexible with the changing of available movements
        int new_x = root.x + neighbours_moves[i][0];
        int new_y = root.y + neighbours_moves[i][1];
        
        if(new_x < 0 || new_x >= width || new_y < 0 || new_y >= height)
            continue;

        cell new_cell = {new_x, new_y};

        if(!availableCell(new_cell))
            continue;

        neighbours.push_back(new_cell);
    }

    return neighbours;

}


/**
 * This is the main logic of the class to search all the possible ways from root cell
 *  By searching DFS to get the longest path in the end
 *
 * @param root: cell: the cell to be considered root for the search
 * @param sol: solution: the path that it is already taken while searching  
 */
void Labyrinth::searchDFS(cell root, solution sol)
{

    if(!availableCell(root)){
        solutions.push_back(sol);
        return;
    }
    // if it is already visited return (exist in the root path)
    bool visited = visitedCell(root, sol);
    if(visited)
        return;
    // Visit the root
    // Add the current cell to the path
    solution new_sol = sol;
    new_sol.path.push_back(root);
    new_sol.length++;
    // Get the neighbours and make them as children
    vector<cell> neighbours = getNeighbours(root);
    for(int i = 0; i < neighbours.size(); i++)
    {

        searchDFS(neighbours[i],new_sol);

    }
    solutions.push_back(new_sol);
    return;
}


/**
 * The function to call the read the file, call all DFS search from every cell as roots
 *  Then write the solution
 * The function that is needed to be called from the user code
 */
void Labyrinth::solve()
{
    readFile(in_file_name);
    vector<cell> empty_path;
    solution empty_sol = {
        0, empty_path
    };

    // Iterate all over the cells from the first row until find a row that has a space (Wrong for the case of the deadend of the element starting from the 1st row and for example there is another cell that start and end too far)
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            cell root = {x,y};
            if(!availableCell(root))
                continue;
            
            searchDFS(root, empty_sol);
        }
    }
    
    writeFile(out_file_name);
}

