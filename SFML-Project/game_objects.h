#pragma once
#include <iostream>
#include <string>
using namespace std;

//SFML files
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
//SFML namespace
using namespace sf;

class game_object {
protected:
	Vector2f inital_position; //The inital position of the object. Used to reset the objects position
	string type; //The type of object ("Enemy", "Player", etc.)
	RectangleShape shape; //The object's shape
	Texture texture; //The object's texture
	Sprite sprite; //The object's sprite
public:
	//Constructor
	game_object(float x_position, float y_position, float width, float height, string type, Color color) {
		set_inital_position(x_position, y_position);
		set_position(x_position, y_position);
		set_size(width,height);
		set_type(type);
		set_color(color);
		if (type == "Platform") {
			texture.loadFromFile("platform.PNG");
			texture.setRepeated(true);
			sprite.setTexture(texture);
			sprite.setScale(3.125, 3.125);
			sprite.setTextureRect(IntRect(0,0,width/ 3.125,height/ 3.125));
			sprite.setPosition(x_position, y_position);
		}
	}
	//Default constructor
	game_object() = default;
	//Destructor
	~game_object(){};

	//Called every frame (delta is the time between frame in seconds)
	virtual void update(float delta) { update_sprite();  }

	//Called every time a collision is detected by the level manager
	virtual int on_collision(string type_of_other_object, Vector2f other_position, Vector2f other_size) {return 0;}

	//Resets the position of the object
	void reset_position() {
		shape.setPosition(inital_position);
	}

	//Applys gravity to an object. Should only be called if an object should have gravity applied to it
	virtual void apply_gravity(float delta) {
		float fall_speed = 50;
		shape.move(0, 9.8 * fall_speed * delta);
	}

	//Updates the sprite
	virtual void update_sprite() {
		//Update the sprites position to be the same as the shape's position
		set_sprite_position(Vector2f(get_x_position(), get_y_position()));
	}

	//Getters
	Vector2f get_inital_position() { return inital_position; };
	float get_x_position() { return shape.getPosition().x; };
	float get_y_position() { return shape.getPosition().y; };
	float get_width() { return shape.getSize().x; };
	float get_height() { return shape.getSize().y; };
	string get_type() {return type;}
	RectangleShape get_shape() { return shape; };
	Color get_color() { return shape.getFillColor(); };
	Sprite get_sprite() { return sprite; }
	//Setters
	void set_inital_position(float x_position, float y_position) { inital_position.x = x_position; inital_position.y = y_position; };
	void set_position(float x_position, float y_position) { shape.setPosition(Vector2f(x_position,y_position)); };
	void set_size(float width, float height) { shape.setSize(Vector2f(width, height)); };
	void set_type(string type) { this->type = type; };
	void set_color(Color color) { shape.setFillColor(color); };
	void set_sprite_position(Vector2f position) { sprite.setPosition(position); };
};

class health_pickup : public game_object {
protected:

public:

	health_pickup(float x_position, float y_position, float width, float height, string type, Color color) : game_object(x_position, y_position, width, height, type, color) {
		texture.loadFromFile("health_pickup.PNG");
		sprite.setTexture(texture);
		sprite.setScale(width / texture.getSize().x, height / texture.getSize().y);
		sprite.setPosition(x_position, y_position);
	}

	~health_pickup() {};
};

class speed_pickup : public game_object {
protected:
	int duration;

	void set_duration(int duration) {
		this->duration = duration;
	}

public:

	int get_duration() {
		return duration;
	}

	speed_pickup(float x_position, float y_position, float width, float height, string type, Color color, int duration) : game_object(x_position, y_position, width, height, type, color) {
		texture.loadFromFile("speed_pickup.PNG");
		sprite.setTexture(texture);
		sprite.setScale(width / texture.getSize().x, height / texture.getSize().y);
		sprite.setPosition(x_position, y_position);

		set_duration(duration);
	}

	~speed_pickup() {};
};

class jump_pad : public game_object {
protected:
	int bounce;


public:
	void set_bounce(int bounce) {
		this->bounce = bounce;
	}
	int get_bounce() {
		return bounce;
	}





	jump_pad(float x_position, float y_position, float width, float height, string type, Color color, int bounce) : game_object(x_position, y_position, width, height, type, color) {
		set_bounce(bounce);

		texture.loadFromFile("jump_pad.PNG");
		texture.setRepeated(true);
		sprite.setTexture(texture);
		sprite.setScale(3.125, 3.125);
		sprite.setTextureRect(IntRect(0, 0, width / 3.125, height / 3.125));
		sprite.setPosition(x_position, y_position);
	}
	~jump_pad() {};
};


class player : public game_object {
protected:
	int floor_count = 0; //Keeps track of the number of floors the player is currently in contact with
	int left_wall_count = 0;
	int right_wall_count = 0;
	int ceiling_count = 0;
	enum move_speeds {
		slowed = 150,
		normal = 300,
		boosted = 600
	};
	float move_speed = static_cast<float>(move_speeds::normal); //Movement speed
	
	int power_up_duration;
	

	float jump_force = -1950; //Jump force
	const float default_jump_force = -1950;
	float y_velocity = 0; //Y velocity

	bool force_bounce = false;
	bool on_down_pressed = false;

	int health = 3;
	void set_health(int health) {
		if (health <= 3) {
			this->health = health;
		}
		
	}
	

public:

	map<int, Texture> health_textures;

	//Constructor
	player(float x_position, float y_position, float width, float height, string type, Color color) : game_object(x_position,y_position,width,height,type,color)  {
		texture.loadFromFile("full_health_player.PNG");
		sprite.setTexture(texture);
		sprite.setScale(width / texture.getSize().x, height / texture.getSize().y);
		sprite.setPosition(x_position, y_position);
	}
	//Destructor
	~player() {};

	//Override update function
	void update(float delta) override {

		//Apply y velocity (jump)
		shape.move(0,y_velocity * delta);

		//Reduce y velocity (make the jump go down)
		if (y_velocity < 0)
			y_velocity += 98;
		else
			y_velocity = 0;

		//Apply gravity only if the player isn't touching the ground
		if (get_floor_count() < 1)
			apply_gravity(delta);

		update_sprite();
	}

	//Update player's movement
	void update_movement(float delta, bool left, bool right, bool up, bool down) {
		//Left pressed and not colliding with a wall
		if (left && get_right_wall_count() < 1) {
			//Move player
			shape.move(-1 * get_move_speed() * delta, 0);
		}
		//Right pressed and not colliding with a wall
		if (right && get_left_wall_count() < 1) {
			//Move player
			shape.move(1 * get_move_speed() * delta, 0);
		}
		//Jump pressed and on a floor
		if (get_floor_count() >= 1 && up) {
			//Set y velocity to the jump_force
			y_velocity = jump_force;
		}
		
		set_on_down_pressed(down);
		
	}

	//Override on collision function
	int on_collision(string type_of_other_object, Vector2f other_position, Vector2f other_size) override{
		//Check if other object is a platform
		if (type_of_other_object == "Platform" || type_of_other_object == "Jump Pad") {
			//Colliding with a wall on the left side of the platform
			if (get_x_position() < other_position.x && get_y_position() > other_position.y - (get_height() - 10)) {
				set_left_wall_count(get_left_wall_count() + 1);
			}
			//Colliding with a wall on the right side of the platform
			else if (get_x_position() + get_width() > other_position.x + other_size.x && get_y_position() > other_position.y - (get_height() - 10)) {
				set_right_wall_count(get_right_wall_count() + 1);
			}
			//Colliding with the floor of a platform
			else if (get_y_position() + get_height() < other_position.y + 10) {
				set_floor_count(get_floor_count() + 1);
				set_force_bounce(false);
				//reset jump force if the player is not on a jump pad
				if (type_of_other_object == "Jump Pad") {
					set_force_bounce(true);
					return 2;
				}
				else {
					
					set_jump_force(get_default_jump_force());
				}
				
			}
			//Colliding with the ceiling of a platform
			else if (get_y_position() > other_position.y) {
				y_velocity = 0;
			}
			return 1;
		}
		//i changed this function to an int because when it returns, if its 1 it will reset_level, but i cant call that from here
		else if (type_of_other_object == "Enemy") {
			//Colliding with a wall on the left side of the platform
			if (get_x_position() < other_position.x && get_y_position() > other_position.y - (get_height() - 10)) {
				
				return 0;
			}
			//Colliding with a wall on the right side of the platform
			else if (get_x_position() + get_width() > other_position.x + other_size.x && get_y_position() > other_position.y - (get_height() - 10)) {
				
				return 0;
			}
			else if (get_y_position() > other_position.y) {
				
				return 0;
			}
			else if (get_y_position() + get_height() < other_position.y + 10) {
				
				set_jump_force(get_default_jump_force());
				set_floor_count(get_floor_count() + 1);
				set_force_bounce(true);
			}
			return 1;
		}
		
	}

	//Sets the collision counts to 0. Called at the beginning of the player's detect_collisions loop in the level manager
	void reset_collision_counts() {
		set_floor_count(0);
		set_left_wall_count(0);
		set_right_wall_count(0);
		set_ceiling_count(0);
	}

	
	void set_jump_force(float jump_force) {
		this->jump_force = jump_force;
	}
	

	void preload_player_sprites() {
		Texture texture;
		texture.loadFromFile("full_health_player.PNG");
		health_textures[3] = texture;

		texture.loadFromFile("mid_health_player.PNG");
		health_textures[2] = texture;

		texture.loadFromFile("low_health_player.PNG");
		health_textures[1] = texture;
	}

	void update_player_sprite() {
		sprite.setTexture(health_textures[get_health()]);
	}
	void update_powerups( ) {
		if (get_powerup_duration() > 0) {
			set_power_up_duration(get_powerup_duration() - 1);
		}
		

		if ( get_powerup_duration() <= 0) {
			normal_move_speed();
			//other future powerups;
		}
	}
	void loose_heart() {
		set_health(get_health() - 1);
		update_player_sprite();
	}
	void add_health(int health) {
		set_health(get_health() + health);
		update_player_sprite();
	}
	
	//Getters

	bool get_on_down_pressed() {
		return on_down_pressed;
	}
	bool get_force_bounce() {
		return force_bounce;
	}
	int get_health() {
		return health;
	}

	float get_default_jump_force() {
		return default_jump_force;
	}
	float get_jump_force() {
		return jump_force;
	}
	int get_floor_count() {
		return floor_count;
	}
	
	int get_left_wall_count() {
		return left_wall_count;
	}
	int get_right_wall_count() {
		return right_wall_count;
	}

	int get_ceiling_count() {
		return ceiling_count;
	}

	float get_move_speed() {
		return move_speed;
	}
	int get_powerup_duration() {
		return power_up_duration;
	}
	
	//Setters
	void set_on_down_pressed(bool on_down_pressed) {
		this->on_down_pressed = on_down_pressed;
	}
	void set_force_bounce(bool force_bounce) {
		this->force_bounce = force_bounce;
	}
	
	void set_power_up_duration(int power_up_duration) {
		if (power_up_duration > 0) {
			this->power_up_duration = power_up_duration;
		}
	}
	void set_floor_count(int new_floor_count) {
		floor_count = new_floor_count;
	}
	void set_left_wall_count(int new_num) {
		left_wall_count = new_num;
	}
	void set_right_wall_count(int new_num) {
		right_wall_count = new_num;
	}
	void set_ceiling_count(int new_num) {
		ceiling_count = new_num;
	}
	void boost_move_speed() {
		move_speed = static_cast<float>(move_speeds::boosted);
	}
	void normal_move_speed() {
		move_speed = static_cast<float>(move_speeds::normal);
	}
	void slow_move_speed() {
		move_speed = static_cast<float>(move_speeds::slowed);
	}
	
};




class enemy : virtual public game_object {
protected:

	int left_wall_count = 0;
	int right_wall_count = 0;

	float move_speed = 50; //Movement speed
	float initial_move_speed = 50;
	bool invincible;
	int travel_distance;

	void set_initial_move_speed(float initial_move_speed) {
		this->initial_move_speed = initial_move_speed;
	}
	void set_move_speed(float move_speed) {
		this->move_speed = move_speed;
		
	}
	void set_invincible(bool invincible) {
		this->invincible = invincible;
	}

	void set_travel_distance(int flying_distance) {
		this->travel_distance = flying_distance;
	}
	
public:


	void update_movement(float delta)  {
		if (get_move_speed() < 0) {
			if (get_inital_position().x - get_x_position() >= get_travel_distance()) {
				change_direction();
			}
		}
		else if (get_move_speed() > 0) {
			if (get_x_position() - get_inital_position().x >= get_travel_distance()) {
				change_direction();
			}
		}
		

		shape.move(get_move_speed() * delta, 0);
	}
	

	virtual int on_collision(string type_of_other_object, Vector2f other_position, Vector2f other_size) { return 0; }

	bool get_invincible() {
		return invincible;
	}
	int get_travel_distance() {
		return travel_distance;
	}
	float get_initial_move_speed() {
		return initial_move_speed;
	}
	void reset_move_speed() {
		set_move_speed(get_initial_move_speed());
	}
	int get_left_wall_count() {
		return left_wall_count;
	}
	int get_right_wall_count() {
		return right_wall_count;
	}
	void set_left_wall_count(int new_num) {
		left_wall_count = new_num;
	}
	void set_right_wall_count(int new_num) {
		right_wall_count = new_num;
	}
	float get_move_speed() {
		return move_speed;
	}
	void change_direction() {
		set_move_speed(get_move_speed() * -1);

	}
	void update(float delta) override {
		update_sprite();
	}

	//Enemy constructor
	enemy(float x_position, float y_position, float width, float height, string type, Color color, float move_speed, int travel_distance, bool invincible) : game_object(x_position, y_position, width, height, type, color) {
		set_move_speed(move_speed);
		set_travel_distance(travel_distance);
		set_initial_move_speed(move_speed);
		set_invincible(invincible);
	}
	//Default constructor
	enemy() = default;
	//Destructor
	~enemy() {};
};



class ground_enemy: public  enemy {
protected:
	int floor_count = 0; //Keeps track of the number of floors the enemy is currently in contact with
	
	int ceiling_count = 0;

	float y_velocity = 0; //Y velocity	

public:

	int on_collision(string type_of_other_object, Vector2f other_position, Vector2f other_size) override {

		//Check if other object is an platform
		if (type_of_other_object == "Platform" || type_of_other_object == "Pickup" ) {
			//Colliding with a wall on the left side of the platform
			if (get_x_position() < other_position.x && get_y_position() > other_position.y - (get_height() - 10)) {
				set_left_wall_count(get_left_wall_count() + 1);
				change_direction();
			}
			//Colliding with a wall on the right side of the platform
			else if (get_x_position() + get_width() > other_position.x + other_size.x && get_y_position() > other_position.y - (get_height() - 10)) {
				set_right_wall_count(get_right_wall_count() + 1);
				change_direction();
			}
			//Colliding with the floor of a platform
			else if (get_y_position() + get_height() < other_position.y + 10) {
				set_floor_count(get_floor_count() + 1);
			}
			else if (get_y_position() > other_position.y) {
				y_velocity = 0;
			}
			return 0;
		}
		//i changed this function to an int because when it returns, if its 1 it will reset_level, but i cant call that from here
		else if (type_of_other_object == "Player") {
			//Colliding with a wall on the left side of the platform
			if (get_y_position() > other_position.y) {
				if (get_invincible()) {
					return 1;
				}
				else {
					shape.move(2000, 1000);
					return 0;
				}
				
			}
			else {
				 reset_position();
				return 1;
				
			}
			
			
			return 0;
		}

	}
	void reset_collision_counts() {
		set_floor_count(0);
		set_left_wall_count(0);
		set_right_wall_count(0);
	}

	int get_floor_count() {
		return floor_count;
	}
	void set_floor_count(int new_floor_count) {
		floor_count = new_floor_count;
	}

	void set_ceiling_count(int new_num) {
		ceiling_count = new_num;
	}
	


	void update(float delta) override {

		//Apply y velocity (jump)
		shape.move(0, y_velocity * delta);

		//Reduce y velocity (make the jump go down)
		if (y_velocity < 0)
			y_velocity += 98;
		else
			y_velocity = 0;

		//Apply gravity only if the player isn't touching the ground
		if (get_floor_count() < 1)
			apply_gravity(delta);

		update_sprite();
	}

	//Ground enemy constructor

	ground_enemy(float x_position, float y_position, float width, float height, string type, Color color, int move_speed, int travel_distance, bool invincible) : enemy(x_position, y_position, width, height, type, color, move_speed, travel_distance, invincible), game_object(x_position, y_position, width, height, type, color) {
		//Load texture image & apply to sprite
		if (invincible) {
			texture.loadFromFile("invincible_ground_enemy.PNG");
		}
		else {
			texture.loadFromFile("ground_enemy.PNG");
		}
		
		sprite.setTexture(texture);
		sprite.setScale(width / texture.getSize().x, height / texture.getSize().y);
		sprite.setPosition(x_position, y_position);
	};
	//Destructor
	~ground_enemy() {};
};
	

class flying_enemy: public enemy{
public:

	//Flying enemy constructor
	flying_enemy(float x_position, float y_position, float width, float height, string type, Color color, int move_speed, int travel_distance, bool invincible) : enemy(x_position, y_position, width, height, type, color, move_speed, travel_distance, invincible), game_object(x_position, y_position, width, height, type, color) {
		//Load texture image & apply to sprite
		if (invincible) {
			texture.loadFromFile("invincible_flying_enemy.PNG");
		}
		else {
			texture.loadFromFile("flying_enemy.PNG");
		}
		
		sprite.setTexture(texture);
		sprite.setScale(width / texture.getSize().x, height / texture.getSize().y);
		sprite.setPosition(x_position, y_position);
	};

	//Destructor
	~flying_enemy() {};

	void update(float delta) override {
		//Update the sprite
		update_sprite();
	}

	int on_collision(string type_of_other_object, Vector2f other_position, Vector2f other_size) override {
		//i changed this function to an int because when it returns, if its 1 it will reset_level, but i cant call that from here
		//Check if other object is an platform
		if (type_of_other_object == "Platform" || type_of_other_object == "Health Pickup") {
			//Colliding with a wall on the left side of the platform
			if (get_x_position() < other_position.x && get_y_position() > other_position.y - (get_height() - 10)) {
				set_left_wall_count(get_left_wall_count() + 1);
				change_direction();
			}
			//Colliding with a wall on the right side of the platform
			else if (get_x_position() + get_width() > other_position.x + other_size.x && get_y_position() > other_position.y - (get_height() - 10)) {
				set_right_wall_count(get_right_wall_count() + 1);
				change_direction();
			}
			
			
			return 0;
		}

		if (type_of_other_object == "Player") {
			//Colliding with a wall on the left side of the platform
			if (get_invincible()) {
				return 1;
			}
			else {
				if (get_y_position() > other_position.y) {
					shape.move(2000, 1000);
					return 0;
				}
				else {
					// reset_position();
					return 1;
				}
			}
			
			
		}
		return 0;
	}
};


class end_goal : public game_object {
protected:
	int level_to_load = 1;
public:
	end_goal(float x_position, float y_position, float width, float height, string type, Color color, int level) : game_object(x_position,y_position,width,height,type,color) {
		set_level_to_load(level);
		texture.loadFromFile("end_goal.PNG");
		sprite.setTexture(texture);
		sprite.setScale(width / texture.getSize().x, height / texture.getSize().y);
		sprite.setPosition(x_position, y_position);
	}

	//Getter(s)
	int get_level_to_load() {return level_to_load;}
	//Setter(s)
	void set_level_to_load(int new_level) { level_to_load = new_level; }
};