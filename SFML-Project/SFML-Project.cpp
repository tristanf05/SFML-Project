#include <iostream>
#include <string>
using namespace std;

//Our files
#include "game_objects.h"
#include "level_manager.h"

//SFML files
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
//SFML namespace
using namespace sf;

int main()
{
    //Variables
    string player_name;
    int user_seleciton;
    int* current_level_size;
    game_object*** current_level_array;

    //Level manager
    level_manager levels = level_manager();

    //Inital message
    cout << "Welcome to [GAME TITLE GOES HERE]!" << endl;
    //New game or continue
    cout << "1. Start New Game" << endl << "2. Continue From Existing Save File" << endl;
    //Get user selection
    cin >> user_seleciton;
    switch(user_seleciton){
    case 1:
        //New game
        cout << endl << "Enter Your Name:" << endl;
        cin >> player_name;

        //TODO: Save player name to save file

        levels.set_current_level(1);

        break;
    case 2:
        //Continue

        //TODO: Add continuing from save file

        //Code for testing only, should be setting it to whatever the saved level id is
        levels.set_current_level(1);

        break;
    default:
        //Invalid input handling
        cout << endl << "Invalid input!" << endl;
        return -1;
        break;
    }

    //Some of the following code is based on the offical SFML documentation (https://www.sfml-dev.org/documentation/2.6.2/)
    //Create window with SFML
    RenderWindow window(VideoMode(1440, 810), "Game Title", Style::Titlebar | Style::Close);
    window.setFramerateLimit(60); //Set the framerate limit to 60fps
    //SFML input detection
    Event input_event;
    //Clock that records the time between each frame
    Clock delta_clock;
    //Time between each frame
    //IMPORTANT: Make sure to mutliply any movement by delta so that it is frame independant!
    Time delta;

    //Main game loop -- exits when the window is closed
    while (window.isOpen()) {


        //Detect user inputs
        while (window.pollEvent(input_event)) {

            //TODO: Detect user input

            //Close the window if the close window button is pressed or the escape button is pressed
            if (input_event.type == Event::Closed || input_event.type == Event::KeyPressed && input_event.key.code == Keyboard::Escape)
                //Close the window
                window.close();
        }



        //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        //TODO: Main game logic goes here

        //Run the update function for every object in the current level
        levels.update_all_objects(delta);

        //Need collision detection. Check here: https://stackoverflow.com/questions/49616112/c-sfml-collision-detection-between-different-classes
        //https://www.sfml-dev.org/tutorials/2.1/graphics-transform.php#bounding-boxes
        //Iterate through every player and ground enemy in the level and check if it is colliding with any of the other objects
        //Needs an on_collision(other_object) function on the game_object class


        //TODO: Make a game manager class cause this is starting to get messy!


        //Check for collisions between all objects
        

        //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-



        //Render
        //Clear the previous frame and color the background with light blue
        window.clear(Color(147, 248, 250));

        //Draw every object in the level
        for (int i = 0; i < levels.get_current_level_size(); i++) {
            window.draw(levels.get_current_level_array()[0][i]->get_shape());
        }

        //Display the new frame
        window.display();

        //Returns the time elapsed from the last restart (time between each frame)
        delta = delta_clock.restart();
    }

    //Delete the level arrays
    levels.delete_levels();

    return 1;
}