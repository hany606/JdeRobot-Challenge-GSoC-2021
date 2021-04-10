/*
 * Author: Hany Hamed
 * Description: The main code to be run and modify the parameters of the Labyrinth
 * */


#include "Labyrinth.h"
#include <iostream>
using namespace std;

int main()
{
    string root_dir = "../data/";

    Labyrinth labyrinth = Labyrinth(root_dir+"input.txt", root_dir+"output.txt");
    labyrinth.solve();
    
    return 0;
}