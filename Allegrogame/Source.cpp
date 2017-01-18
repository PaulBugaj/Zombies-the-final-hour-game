#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_native_dialog.h>
#include <iostream>
#include <math.h>

const int WIDTH = 1920;
const int HEIGHT = 1080;
const int NUM_BULLETS = 7;
int help;
int help2;
int swag;
//Start with main menu/ Used to switch menus
int gamestate = 2;
bool keys[] = { false, false, false, false, false};

enum KEYS { DOWN, LEFT, RIGHT, UP, SPACE};
enum IDS { PLAYER, BULLET, ENEMY };
enum STATE { IDLE, ATTACKING, RETURNING };

/*struct zombie
{

};*/

struct Bullet
{
	int ID;
	int x;
	int y;
	bool live;
	int speed;
	int direction;
};

//Collisions across entire map~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Collision(float x, float y, float ex, float ey, int width, int height)
{
	if (((x + 25<ex) || (x>ex + width)) || ((y + 25<ey) || (y>ey + height)))
	{
		return false;
	}
	return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//AI switching states from idle to attack to return to original position~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
void stateswitch(int &state, int newstate)
{
	if (state == IDLE)
	{
		std::cout << "Moving\n";
	}
	else if (state == ATTACKING)
	{
		std::cout << "Not attacking\n";
	}
	else if (state == RETURNING)
	{
		std::cout << "Not moving\n";
	}

	state = newstate;

	if (state == IDLE)
	{
		std::cout << "Not moving\n";
	}
	else if (state == ATTACKING)
	{ 
		std::cout << "Attacking character\n";
	}
	else if (state == RETURNING)
	{ 
		std::cout << "Returning to origin\n";
	}
}

//Finds distance from zombie to character
float calcdistancefromchartozombie(int x1, int y1, int x2, int y2)
{
	return sqrt(pow((float)x1 - x2, 2) + pow((float)y1 - y2, 2));
}

//Moving zombie towards character
float movingatanagletocharacter(int x1, int y1, int x2, int y2)
{
	float deltaX = (x2 - x1);
	float deltaY = (y2 - y1);
	return atan2(deltaY, deltaX);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//Function for player


//Functions for bullets
//Initialization of bullet
void InitBullet(Bullet bullet[], int size)
{
	for (int i = 0; i < size; i++)
	{
		
		bullet[i].ID = BULLET;
		bullet[i].speed = 10;
		bullet[i].live = false;
	}
}

//Drawing bullet using primitives
void DrawBullet(Bullet bullet[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (bullet[i].live)
			al_draw_filled_circle(bullet[i].x, bullet[i].y, 2, al_map_rgb(255, 255, 255));
	}
}

//Shooting bullet using directions (according to directions of character)
void FireBullet(Bullet bullet[], int size, int x, int y)
{
	for (int i = 0; i < size; i++)
	{
		if (!bullet[i].live)
		{
			if (help == 1)
			{
				bullet[i].direction = 1;
			}
			else if (help == 2)
			{
				bullet[i].direction = 2;
			}
			else if (help2 == 1)
			{
				bullet[i].direction = 3;
			}
			else if (help2 == 2)
			{
				bullet[i].direction = 4;
			}
			bullet[i].x = x;
			bullet[i].y = y;
			bullet[i].live = true;
			break;
		}
	}
}

//Update bullet so it flies through the air
void UpdateBullet(Bullet bullet[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (bullet[i].live)
		{
			if (bullet[i].direction == 2)
			{
				bullet[i].x += bullet[i].speed;
			}
			else if (bullet[i].direction == 1)
			{
				bullet[i].x -= bullet[i].speed;
			}
			else if (bullet[i].direction == 4)
			{
				bullet[i].y += bullet[i].speed;
			}
			else if (bullet[i].direction == 3)
			{
				bullet[i].y -= bullet[i].speed;
			}
			if (bullet[i].x > WIDTH)
				bullet[i].live = false;
			if (bullet[i].x < 0)
				bullet[i].live = false;
			if (bullet[i].y > HEIGHT)
				bullet[i].live = false;
			if (bullet[i].y < 0)
				bullet[i].live = false;
			
		}
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


int main(void)
{
//Initializers
	al_init();
	al_install_keyboard();
	al_install_mouse();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();

//Variables
	//Program Variables
	const float FPS = 60.0;
	bool done = false;
	bool render = false;
	bool draw = true;
	float gameTime = 0;
	int frames = 0;
	int gameFPS = 0;
	Bullet bullets[7];

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	//Enemies
	int state = -1;

	//Origin
	const int originx = WIDTH - 1000;
	const int originy = HEIGHT / 2;
	int dir1 = DOWN;
	int life = 3;
	int zomblife = 5;
	int exp = 10;
	//Zombies position
	int zombposx = originx;
	int zombposy = originy;
	int sourceA = 0, sourceZ = 250;
	int aggrorange = 220;

	//Cursor
	int pos_x;
	int pos_y;

	//Character
	float x = 20, y = 85, x1 = 10, y1 = 10, moveSpeed = 1.5;
	bool active = false;
	
	//Camera
	int xcam = 0;
	int ycam = 0;

	//Generating Map
	int mapcolumn = 20;
	int mapSize = 400;
	int tileSize = 100;
	int dir = DOWN, sourceX = 250, sourceY = 0;
	
	//Maps
	int map[] = { 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5,
				  1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
				  2, 2, 1, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5,
				  2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
			   	  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
				  2, 2, 1, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5,
				  2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2 };
	
	/*int map2[] = { 5, 6, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 6, 5, 5,
				   1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
				   1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
				   1, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 3, 5, 5,
				   1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
				   1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
				   1, 4, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 3, 5, 5,
				   1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
				   1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
				   1, 4, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 3, 5, 5,
				   1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
				   1, 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 1, 0, 1, 2,
				   1, 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 1, 0, 1, 2,
				   1, 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 1, 0, 1, 2,
				   1, 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 1, 0, 1, 2,
				   1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
				   5, 3, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 3, 5, 5,
				   1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1,
				   1, 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 1, 0, 1, 2,
				   1, 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 1, 0, 1, 2 };*/

//Initializing Messages
	if (!al_init())
		al_show_native_message_box(NULL, "ERROR", NULL, "Could not initialize Allegro", NULL, NULL);
	
	ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);

	if (!display)
		al_show_native_message_box(NULL, "ERROR", NULL, "Could not create display", NULL, NULL);

	//al_set_window_position(display, 400, 400);

	//Load player bitmap
	ALLEGRO_BITMAP *player1 = al_load_bitmap("NEWPOSTWBACK.png");
	al_convert_mask_to_alpha(player1, al_map_rgb(241, 23, 23));

	//Load zombie bitmap
	ALLEGRO_BITMAP *zombie = al_load_bitmap("zombiespriteFINISHED.png");
	al_convert_mask_to_alpha(zombie, al_map_rgb(241, 243, 40));

	ALLEGRO_BITMAP *title = al_load_bitmap("zombies.png");
	ALLEGRO_BITMAP *map1 = al_load_bitmap("FIXEDMAP.png");
	ALLEGRO_BITMAP *house = al_load_bitmap("house1.png");
	ALLEGRO_BITMAP *deadtree = al_load_bitmap("swamptree.png");
	ALLEGRO_BITMAP *townhousedown = al_load_bitmap("townhouse2.png");
	ALLEGRO_BITMAP *townhouseleft = al_load_bitmap("townhouseleft.png");
	ALLEGRO_BITMAP *townhouse = al_load_bitmap("townhouse.png");
	ALLEGRO_BITMAP *apocalypse = al_load_bitmap("apocalypse.jpg");
	ALLEGRO_BITMAP *controlsmenu = al_load_bitmap("controlsmenu.jpg");
	ALLEGRO_BITMAP *inventory = al_load_bitmap("charinterface.png");
	ALLEGRO_BITMAP *Health = al_load_bitmap("Hp.png");
	ALLEGRO_BITMAP *minimap = al_load_bitmap("minimap.png");
	ALLEGRO_BITMAP *emptyheart = al_load_bitmap("emptyheart.png");
	ALLEGRO_BITMAP *policecar = al_load_bitmap("policecartop.png");
	ALLEGRO_BITMAP *redcar = al_load_bitmap("redcar.png");
	ALLEGRO_BITMAP *clock = al_load_bitmap("clock.png");
	al_convert_mask_to_alpha(clock, al_map_rgb(107, 107, 107));

	ALLEGRO_KEYBOARD_STATE keystate;
	ALLEGRO_FONT *font18 = al_load_font("arial.ttf", 30, 0);
	ALLEGRO_FONT *font19 = al_load_font("arial.ttf", 60, 0);
	ALLEGRO_FONT *font20 = al_load_font("arial.ttf", 70, 0);
	stateswitch(state, IDLE);

	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);

	//Playerstuff(player);
	InitBullet(bullets, NUM_BULLETS);
	
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());

	al_start_timer(timer);
	gameTime = al_current_time();

	//Loading page
	al_draw_bitmap(title, 0, 0, 0);
	al_flip_display();
	al_rest(2.0);

	while (!done)
{
		//Start menu
		if (gamestate == 2)
	{
			ALLEGRO_EVENT ev2;
			al_wait_for_event(event_queue, &ev2);
		 if (ev2.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
			pos_x = ev2.mouse.x;
			pos_y = ev2.mouse.y;
		}
		 al_draw_bitmap(apocalypse, 0, 0,0);
		 al_draw_textf(font18, al_map_rgb(255, 255, 255), 500, 5, ALLEGRO_ALIGN_RIGHT,
				"mouse pos x: %.0i", pos_x);
		al_draw_textf(font18, al_map_rgb(255, 255, 255), 1000, 5, ALLEGRO_ALIGN_RIGHT,
				"mouse pos y: %.0i", pos_y);

		//Start game
		if ((pos_x < 1130) && (pos_x > 760) && (pos_y < 530) && (pos_y > 470))
		{
			pos_x = -100;
			pos_y = -100;
			gamestate = 1;
			keys[UP] = false;
			keys[DOWN] = false;
			keys[LEFT] = false;
			keys[RIGHT] = false;
			keys[SPACE] = false;
		}

		//Enter controls menu
		if ((pos_x < 1090) && (pos_x > 780) && (pos_y < 690) && (pos_y > 620))
		{
			pos_x = -100;
			pos_y = -100;
			keys[UP] = false;
			keys[DOWN] = false;
			keys[LEFT] = false;
			keys[RIGHT] = false;
			keys[SPACE] = false;
			gamestate = 3;
		}

		//Exit game
		if ((pos_x < 1000) && (pos_x > 850) && (pos_y < 840) && (pos_y > 770))
		{
			pos_x = -100;
			pos_y = -100;
			done = true;
		}

		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
	}	
		
		//Inventory screen
		if (gamestate == 5)
		{
		ALLEGRO_EVENT ev5;
		al_wait_for_event(event_queue, &ev5);
		if (ev5.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{
		pos_x = ev5.mouse.x;
		pos_y = ev5.mouse.y;
		}
		al_draw_bitmap(inventory, 0, 0, 0);
		al_draw_textf(font18, al_map_rgb(255, 255, 255), 200, 5, ALLEGRO_ALIGN_RIGHT,
		"pos x: %.0i", pos_x);

		al_draw_textf(font18, al_map_rgb(255, 255, 255), 500, 5, ALLEGRO_ALIGN_RIGHT,
		"pos y: %.0i", pos_y);

		al_draw_textf(font20, al_map_rgb(63, 72, 204), 1830, 40, ALLEGRO_ALIGN_RIGHT,
			" %i", exp);

		al_draw_textf(font19, al_map_rgb(237, 28, 36), 980, 507, ALLEGRO_ALIGN_RIGHT,
			" %.0f", moveSpeed);

		al_draw_textf(font19, al_map_rgb(237, 28, 36), 900, 385, ALLEGRO_ALIGN_RIGHT,
			" %.0i", life);

		//Return to game
		if ((pos_x < 482) && (pos_x > 19) && (pos_y < 1037) && (pos_y > 867))
		{
		gamestate = 1;
		pos_x = -100;
		pos_y = -100;
		keys[UP] = false;
		keys[DOWN] = false;
		keys[LEFT] = false;
		keys[RIGHT] = false;
		keys[SPACE] = false;
		}
		
		//+1 hp
		if ((pos_x < 1118) && (pos_x > 1059) && (pos_y < 430) && (pos_y > 367))
		{
		if(exp>=3)
			{
			exp-=3;
			life+=1;
			}
		pos_x = -100;
		pos_y = -100;
		}

		//+1 Speed
		if ((pos_x < 1118) && (pos_x > 1059) && (pos_y < 558) && (pos_y > 496))
		{
		if(exp>=4)
			{
			exp-=4;
			moveSpeed+=1;
			}
		pos_x = -100;
		pos_y = -100;
		}

		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
		}

		//Start of level 1
		if (gamestate == 1)
		{
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);
			al_get_keyboard_state(&keystate);

			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				done = true;
			}

			if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch (ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = true;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = true;
					break;
				case ALLEGRO_KEY_UP:
					keys[UP] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = true;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = true;
					FireBullet(bullets, NUM_BULLETS, x, y);
					break;
				case ALLEGRO_KEY_I:
					gamestate = 5;
					keys[UP] = false;
					keys[DOWN] = false;
					keys[LEFT] = false;
					keys[RIGHT] = false;
					keys[SPACE] = false;
					break;
				}
			}

			else if (ev.type == ALLEGRO_EVENT_KEY_UP)
			{
				switch (ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = false;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = false;
					break;
				case ALLEGRO_KEY_UP:
					keys[UP] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = false;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = false;
					break;
				}
			}


			//START OF TIMER~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			else if (ev.type == ALLEGRO_EVENT_TIMER)
			{
				render = true;

				//UPDATE FPS
				frames++;
				if (al_current_time() - gameTime >= 1)
				{
					gameTime = al_current_time();
					gameFPS = frames;
					frames = 0;
				}

				//Set Barrier around map
				//Physical barrier around map
				if (x < 20) //left
					x = 20;

				if (x > WIDTH - 20) //Right
					x = WIDTH - 20;

				if (y > HEIGHT - 20) //Bottom
					y = HEIGHT - 20;

				if (y < 40) //Top
					y = 40;

				active = true;
				if (al_key_down(&keystate, ALLEGRO_KEY_DOWN))
				{
					y += moveSpeed;
					dir = DOWN;
					help2 = 2;
					help = 0;
				}
				else if (al_key_down(&keystate, ALLEGRO_KEY_UP))
				{
					y -= moveSpeed;
					dir = UP;
					help2 = 1;
					help = 0;
				}
				else if (al_key_down(&keystate, ALLEGRO_KEY_RIGHT))
				{
					x += moveSpeed;
					dir = RIGHT;
					help = 2;
					help2 = 0;
				}
				else if (al_key_down(&keystate, ALLEGRO_KEY_LEFT))
				{
					x -= moveSpeed;
					dir = LEFT;
					help = 1;
					help2 = 0;
				}
				else
					active = false;

				UpdateBullet(bullets, NUM_BULLETS);

				if (active)
					sourceX += al_get_bitmap_width(player1) / 15;
				else
					sourceX = 200;

				if (sourceX >= al_get_bitmap_width(player1))
					sourceX = 0;
				sourceY = dir;

				draw = true;

				for (int j = 0; j < NUM_BULLETS; j++)
				{
					if (((((zombposx - 15) <= bullets[j].x) && ((zombposx + 15) >= bullets[j].x))) &&
						((((zombposy - 15) <= bullets[j].y) && ((zombposy + 15) >= bullets[j].y))))
					{
						bullets[j].live = false;
						bullets[j].x = -1000;
						bullets[j].y = -1000;
						zomblife -= 1;
					}
				}

				if (swag == 60)
				{
					swag = 0;
				}

				if (((((zombposx - 15) <= x) && ((zombposx + 15) >= x))) &&
					((((zombposy - 15) <= y) && ((zombposy + 15) >= y))))
				{
					swag++;
					if (swag == 1)
						life -= 1;
				}

				//Start of AI
				if (state == IDLE)
				{
					if (aggrorange > calcdistancefromchartozombie(x, y, zombposx, zombposy))
						stateswitch(state, ATTACKING);
				}

				else if (state == ATTACKING)
				{
					if (aggrorange < calcdistancefromchartozombie(zombposx, zombposy, originx, originy))
						stateswitch(state, RETURNING);
					else
					{
						float angle = movingatanagletocharacter(zombposx, zombposy, x, y);
						//std::cout << angle<<std::endl;

						zombposy += (2 * sin(angle));
						zombposx += (2 * cos(angle));

						if (aggrorange < calcdistancefromchartozombie(zombposx, zombposy, x, y))
							stateswitch(state, RETURNING);
					}

				}

				else if (state == RETURNING)
				{
					if (5 >= calcdistancefromchartozombie(zombposx, zombposy, originx, originy))
					{
						zombposx = originx;
						zombposy = originy;
						stateswitch(state, IDLE);
					}
					else
					{
						float angle = movingatanagletocharacter(zombposx, zombposy, originx, originy);
						zombposy += (2 * sin(angle));
						zombposx += (2 * cos(angle));


						if (aggrorange > calcdistancefromchartozombie(zombposx, zombposy, x, y))
							stateswitch(state, ATTACKING);
					}
				}

				if (state)
					sourceZ += al_get_bitmap_width(zombie) / 15;
				else
					sourceZ = 250;

				if (sourceZ >= al_get_bitmap_width(zombie))
					sourceZ = 0;
				sourceA = dir1;

				//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Collisions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if (gamestate == 1)
				{
					//Houses on left hand side facing right
					if (Collision(x, y, 101, 201, 123, 900))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Second row of houses facing left
					if (Collision(x, y, 500, 201, 114, 585))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Houses facing down in the middle of the map
					if (Collision(x, y, 680, 710, 365, 110))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Houses facing right on the right hand side of the map
					if (Collision(x, y, 1100, 201, 114, 590))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Houses facing left on the right hand side of the map
					if (Collision(x, y, 1500, 201, 114, 590))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Houses at the very bottom of the map facing right
					if (Collision(x, y, 1060, 1030, 114, 590))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}


					//Tree collisions
					//Tree on left bottom hand corner
					if (Collision(x, y, 660, 600, 46, 70))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Tree in top left hand corner
					if (Collision(x, y, 708, 308, 46, 70))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Tree in top right hand corner
					if (Collision(x, y, 985, 318, 46, 70))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Tree in middle of four trees
					if (Collision(x, y, 870, 470, 46, 70))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Tree in bottom right hand corner of group of five trees
					if (Collision(x, y, 1000, 535, 46, 70))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Minimap
					if (Collision(x, y, 1654, 0, 266, 160))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Health
					if (Collision(x, y, 900, 0, 80, 80))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Police car
					if (Collision(x, y, 355, 431, 80, 40))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}

					//Red car
					if (Collision(x, y, 465, 930, 80, 40))
					{
						if (dir == 0)
							y -= moveSpeed;
						else if (dir == 1)
							x += moveSpeed;
						else if (dir == 2)
							x -= moveSpeed;
						else if (dir == 3)
							y += moveSpeed;
					}
					//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				}
			}
			
			if (draw)
			{
				//Generate tiles for Map
				for (int i = 0; i < mapSize; i++)
				{
					al_draw_bitmap_region(map1, tileSize * map[i], 0, tileSize, tileSize,
						xcam + tileSize * (i % mapcolumn), ycam + tileSize * (i / mapcolumn), 0);
				}

				//display HUD Stuff
				al_draw_textf(font18, al_map_rgb(255, 0, 255), 5, 5, 0, "FPS: %i", gameFPS);

				al_draw_textf(font18, al_map_rgb(255, 255, 255), 1550, 5, ALLEGRO_ALIGN_RIGHT,
					"X: %.0f", x);
				al_draw_textf(font18, al_map_rgb(255, 255, 255), 1650, 5, ALLEGRO_ALIGN_RIGHT,
					"Y: %.0f", y);

				al_draw_bitmap_region(Health, 0, 0, 75, 75, 900, 0, NULL);
				if (life == 0)
				{
					al_draw_bitmap_region(emptyheart, 0, 0, 75, 75, 900, 0, NULL);
				}
					
				al_draw_textf(font18, al_map_rgb(255, 255, 255), 945, 20, ALLEGRO_ALIGN_RIGHT,
					" %.0i", life);
				
				al_draw_textf(font18, al_map_rgb(255, 255, 255), 860, 5, ALLEGRO_ALIGN_RIGHT,
					"Exp: %i", exp);

				al_draw_textf(font18, al_map_rgb(255, 255, 255), 1135, 5, ALLEGRO_ALIGN_RIGHT,
					"Speed: %.0f", moveSpeed);

				al_draw_bitmap_region(clock, 0, 0, 100, 100, 1810, 980, NULL);

				al_draw_textf(font18, al_map_rgb(0, 0, 0), 1872, 1012, ALLEGRO_ALIGN_RIGHT,
					" %.0f", gameTime); 

				//Draw minimap
				al_draw_bitmap_region(minimap, 0, 0, 266, 151, 1654, 0, NULL);

				//Poice car
				al_draw_bitmap_region(policecar, 0, 0, 85, 54, 340, 411, NULL);

				//Red car
				al_draw_bitmap_region(redcar, 0, 0, 85, 57, 450, 911, NULL);

				//Generating tree sprites on map
				al_draw_bitmap_region(deadtree, 0, 0, 46, 76, 650, 580, NULL);
				al_draw_bitmap_region(deadtree, 0, 0, 46, 76, 700, 300, NULL);
				al_draw_bitmap_region(deadtree, 0, 0, 46, 76, 970, 310, NULL);
				al_draw_bitmap_region(deadtree, 0, 0, 46, 76, 860, 450, NULL);
				al_draw_bitmap_region(deadtree, 0, 0, 46, 76, 990, 520, NULL);

				//Generate house sprites on map

				//Houses on left handed side facing right
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 201, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 320, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 439, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 548, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 667, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 786, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 905, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 1024, NULL);

				//Final destination
				al_draw_bitmap_region(house, 0, 0, 143, 100, 1695, 760, NULL);

				//Houses facing left (Second row of houses after first row)
				al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 450, 201, NULL);
				al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 450, 320, NULL);
				al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 450, 439, NULL);
				al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 450, 548, NULL);
				al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 450, 667, NULL);

				//Houses facing down in the middle of the map
				al_draw_bitmap_region(townhousedown, 0, 0, 118, 148, 670, 700, NULL);
				al_draw_bitmap_region(townhousedown, 0, 0, 118, 148, 787, 700, NULL);
				al_draw_bitmap_region(townhousedown, 0, 0, 118, 148, 905, 700, NULL);

				//Houses facing right on right side of screen
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1101, 201, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1101, 320, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1101, 439, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1101, 548, NULL);
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1101, 667, NULL);

				//al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 786, NULL);
				//House facing right at bottom of screen
				al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 1024, NULL);

				//House on right hand side facing left
				al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 1450, 201, NULL);
				al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 1450, 320, NULL);
				al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 1450, 439, NULL);
				al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 1450, 548, NULL);
				al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 1450, 667, NULL);

				//Generate player
				if (life > 0)
				{
					al_draw_bitmap_region(player1, sourceX, sourceY * al_get_bitmap_height(player1) / 4, 40, 40, x - 19, y - 19, NULL);
				}
				else if(life==0)
				{
					al_show_native_message_box(display, "Zombies: The Final Hour", "YOU HAVE DIED!", NULL, NULL, ALLEGRO_MESSAGEBOX_OK_CANCEL);
					keys[UP] = false;
					keys[DOWN] = false;
					keys[LEFT] = false;
					keys[RIGHT] = false;
					keys[SPACE] = false;
					x = -100;
					y = -100;
					exp = 0;
					life = 3;
					dir = 0;
					int gameTime = 0;
					moveSpeed = 1.5;
					gamestate = 2;
				}

				//Enemies														
				//al_draw_circle(originx, originy, aggrorange, al_map_rgba_f(.5, .5, .5, .5), 1);
				//al_draw_circle(zombposx, zombposy, aggrorange, al_map_rgba_f(.5, 0, .5, .5), 1);
				if (zomblife > 0)
				{
					al_draw_bitmap_region(zombie, sourceZ, sourceA * al_get_bitmap_height(zombie) / 4, 50, 50, zombposx - 24, zombposy - 24, NULL);
				}
				else
				{
					int q = 0;
					zombposx = -1000;
					zombposy = -1000;
					q = exp + 1;
					al_draw_textf(font18, al_map_rgb(255, 255, 255), 645, 5, ALLEGRO_ALIGN_RIGHT,
						"Exp: %.0i", q);
				}

				//Draw bullets
				DrawBullet(bullets, NUM_BULLETS);

				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
			}

			if ((x > 1730) && (x < 1760) && (y > 820) && (y < 850))
			{
				al_show_native_message_box(display, "Zombies: The Final Hour", "LEVEL CONQUERED!", NULL, NULL, ALLEGRO_MESSAGEBOX_OK_CANCEL);
				keys[UP] = false;
				keys[DOWN] = false;
				keys[LEFT] = false;
				keys[RIGHT] = false;
				keys[SPACE] = false;
				exp = 0;
				life = 3;
				moveSpeed = 1.5;
				dir = 0;
				gameTime = 0;
				x = -100;
				y = -100;
				gamestate = 2;
			}
		}

		//Start of level 2
		//if (gamestate == 4)
		/*{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		al_get_keyboard_state(&keystate);

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
		done = true;
		}

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
		switch (ev.keyboard.keycode)
		{
		case ALLEGRO_KEY_ESCAPE:
		done = true;
		break;
		case ALLEGRO_KEY_LEFT:
		keys[LEFT] = true;
		break;
		case ALLEGRO_KEY_RIGHT:
		keys[RIGHT] = true;
		break;
		case ALLEGRO_KEY_UP:
		keys[UP] = true;
		break;
		case ALLEGRO_KEY_DOWN:
		keys[DOWN] = true;
		break;
		case ALLEGRO_KEY_SPACE:
		keys[SPACE] = true;
		FireBullet(bullets, NUM_BULLETS, x, y);
		break;
		case ALLEGRO_KEY_I:
		gamestate = 5;
		keys[UP] = false;
		keys[DOWN] = false;
		keys[LEFT] = false;
		keys[RIGHT] = false;
		keys[SPACE] = false;
		break;
		}
		}

		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
		switch (ev.keyboard.keycode)
		{
		case ALLEGRO_KEY_ESCAPE:
		done = true;
		break;
		case ALLEGRO_KEY_LEFT:
		keys[LEFT] = false;
		break;
		case ALLEGRO_KEY_RIGHT:
		keys[RIGHT] = false;
		break;
		case ALLEGRO_KEY_UP:
		keys[UP] = false;
		break;
		case ALLEGRO_KEY_DOWN:
		keys[DOWN] = false;
		break;
		case ALLEGRO_KEY_SPACE:
		keys[SPACE] = false;
		break;
		}
		}


		//START OF TIMER~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		else if (ev.type == ALLEGRO_EVENT_TIMER)
		{
		render = true;

		//UPDATE FPS
		frames++;
		if (al_current_time() - gameTime >= 1)
		{
		gameTime = al_current_time();
		gameFPS = frames;
		frames = 0;
		}

		//Set Barrier around map
		//Physical barrier around map
		if (x < 20) //left
		x = 20;

		if (x > WIDTH - 20) //Right
		x = WIDTH - 20;

		if (y > HEIGHT - 20) //Bottom
		y = HEIGHT - 20;

		if (y < 40) //Top
		y = 40;

		active = true;
		if (al_key_down(&keystate, ALLEGRO_KEY_DOWN))
		{
		y += moveSpeed;
		dir = DOWN;
		help2 = 2;
		help = 0;
		}
		else if (al_key_down(&keystate, ALLEGRO_KEY_UP))
		{
		y -= moveSpeed;
		dir = UP;
		help2 = 1;
		help = 0;
		}
		else if (al_key_down(&keystate, ALLEGRO_KEY_RIGHT))
		{
		x += moveSpeed;
		dir = RIGHT;
		help = 2;
		help2 = 0;
		}
		else if (al_key_down(&keystate, ALLEGRO_KEY_LEFT))
		{
		x -= moveSpeed;
		dir = LEFT;
		help = 1;
		help2 = 0;
		}
		else
		active = false;

		UpdateBullet(bullets, NUM_BULLETS);

		if (active)
		sourceX += al_get_bitmap_width(player1) / 15;
		else
		sourceX = 200;

		if (sourceX >= al_get_bitmap_width(player1))
		sourceX = 0;
		sourceY = dir;

		draw = true;

		for (int j = 0; j < NUM_BULLETS; j++)
		{
		if (((((zombposx - 15) <= bullets[j].x) && ((zombposx + 15) >= bullets[j].x))) &&
		((((zombposy - 15) <= bullets[j].y) && ((zombposy + 15) >= bullets[j].y))))
		{
		bullets[j].live = false;
		bullets[j].x = -1000;
		bullets[j].y = -1000;
		zomblife -= 1;
		}
		}

		if (swag == 60)
		{
		swag = 0;
		}

		if (((((zombposx - 15) <= x) && ((zombposx + 15) >= x))) &&
		((((zombposy - 15) <= y) && ((zombposy + 15) >= y))))
		{
		swag++;
		if (swag == 1)
		life -= 1;
		}

		//Start of AI
		if (state == IDLE)
		{
		if (aggrorange > calcdistancefromchartozombie(x, y, zombposx, zombposy))
		stateswitch(state, ATTACKING);
		}

		else if (state == ATTACKING)
		{
		if (aggrorange < calcdistancefromchartozombie(zombposx, zombposy, originx, originy))
		stateswitch(state, RETURNING);
		else
		{
		float angle = movingatanagletocharacter(zombposx, zombposy, x, y);
		//std::cout << angle<<std::endl;

		zombposy += (2 * sin(angle));
		zombposx += (2 * cos(angle));

		if (aggrorange < calcdistancefromchartozombie(zombposx, zombposy, x, y))
		stateswitch(state, RETURNING);
		}

		}

		else if (state == RETURNING)
		{
		if (5 >= calcdistancefromchartozombie(zombposx, zombposy, originx, originy))
		{
		zombposx = originx;
		zombposy = originy;
		stateswitch(state, IDLE);
		}
		else
		{
		float angle = movingatanagletocharacter(zombposx, zombposy, originx, originy);
		zombposy += (2 * sin(angle));
		zombposx += (2 * cos(angle));


		if (aggrorange > calcdistancefromchartozombie(zombposx, zombposy, x, y))
		stateswitch(state, ATTACKING);
		}
		}

		if (state)
		sourceZ += al_get_bitmap_width(zombie) / 15;
		else
		sourceZ = 250;

		if (sourceZ >= al_get_bitmap_width(zombie))
		sourceZ = 0;
		sourceA = dir1;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Collisions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if (gamestate == 1)
		{
		//Houses on left hand side facing right
		if (Collision(x, y, 101, 201, 123, 900))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Second row of houses facing left
		if (Collision(x, y, 500, 201, 114, 585))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Houses facing down in the middle of the map
		if (Collision(x, y, 680, 710, 365, 110))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Houses facing right on the right hand side of the map
		if (Collision(x, y, 1100, 201, 114, 590))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Houses facing left on the right hand side of the map
		if (Collision(x, y, 1500, 201, 114, 590))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Houses at the very bottom of the map facing right
		if (Collision(x, y, 1060, 1030, 114, 590))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}


		//Tree collisions
		//Tree on left bottom hand corner
		if (Collision(x, y, 660, 600, 46, 70))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Tree in top left hand corner
		if (Collision(x, y, 708, 308, 46, 70))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Tree in top right hand corner
		if (Collision(x, y, 985, 318, 46, 70))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Tree in middle of four trees
		if (Collision(x, y, 870, 470, 46, 70))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Tree in bottom right hand corner of group of five trees
		if (Collision(x, y, 1000, 535, 46, 70))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Minimap
		if (Collision(x, y, 1654, 0, 266, 160))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Health
		if (Collision(x, y, 900, 0, 80, 80))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Police car
		if (Collision(x, y, 355, 431, 80, 40))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}

		//Red car
		if (Collision(x, y, 465, 930, 80, 40))
		{
		if (dir == 0)
		y -= moveSpeed;
		else if (dir == 1)
		x += moveSpeed;
		else if (dir == 2)
		x -= moveSpeed;
		else if (dir == 3)
		y += moveSpeed;
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		}
		}

		if (draw)
		{
		//Generate tiles for Map
		for (int i = 0; i < mapSize; i++)
		{
		al_draw_bitmap_region(map1, tileSize * map[i], 0, tileSize, tileSize,
		xcam + tileSize * (i % mapcolumn), ycam + tileSize * (i / mapcolumn), 0);
		}

		//display HUD Stuff
		al_draw_textf(font18, al_map_rgb(255, 0, 255), 5, 5, 0, "FPS: %i", gameFPS);

		al_draw_textf(font18, al_map_rgb(255, 255, 255), 1550, 5, ALLEGRO_ALIGN_RIGHT,
		"X: %.0f", x);
		al_draw_textf(font18, al_map_rgb(255, 255, 255), 1650, 5, ALLEGRO_ALIGN_RIGHT,
		"Y: %.0f", y);

		al_draw_bitmap_region(Health, 0, 0, 75, 75, 900, 0, NULL);
		if (life == 0)
		{
		al_draw_bitmap_region(emptyheart, 0, 0, 75, 75, 900, 0, NULL);
		}

		al_draw_textf(font18, al_map_rgb(255, 255, 255), 945, 20, ALLEGRO_ALIGN_RIGHT,
		" %.0i", life);

		al_draw_textf(font18, al_map_rgb(255, 255, 255), 860, 5, ALLEGRO_ALIGN_RIGHT,
		"Exp: %i", exp);

		al_draw_textf(font18, al_map_rgb(255, 255, 255), 1135, 5, ALLEGRO_ALIGN_RIGHT,
		"Speed: %.0f", moveSpeed);

		al_draw_bitmap_region(clock, 0, 0, 100, 100, 1810, 980, NULL);

		al_draw_textf(font18, al_map_rgb(0, 0, 0), 1872, 1012, ALLEGRO_ALIGN_RIGHT,
		" %.0f", gameTime);

		//Draw minimap
		al_draw_bitmap_region(minimap, 0, 0, 266, 151, 1654, 0, NULL);

		//Poice car
		al_draw_bitmap_region(policecar, 0, 0, 85, 54, 340, 411, NULL);

		//Red car
		al_draw_bitmap_region(redcar, 0, 0, 85, 57, 450, 911, NULL);

		//Generating tree sprites on map
		al_draw_bitmap_region(deadtree, 0, 0, 46, 76, 650, 580, NULL);
		al_draw_bitmap_region(deadtree, 0, 0, 46, 76, 700, 300, NULL);
		al_draw_bitmap_region(deadtree, 0, 0, 46, 76, 970, 310, NULL);
		al_draw_bitmap_region(deadtree, 0, 0, 46, 76, 860, 450, NULL);
		al_draw_bitmap_region(deadtree, 0, 0, 46, 76, 990, 520, NULL);

		//Generate house sprites on map

		//Houses on left handed side facing right
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 201, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 320, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 439, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 548, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 667, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 786, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 905, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 1024, NULL);

		//Final destination
		al_draw_bitmap_region(house, 0, 0, 143, 100, 1695, 760, NULL);

		//Houses facing left (Second row of houses after first row)
		al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 450, 201, NULL);
		al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 450, 320, NULL);
		al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 450, 439, NULL);
		al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 450, 548, NULL);
		al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 450, 667, NULL);

		//Houses facing down in the middle of the map
		al_draw_bitmap_region(townhousedown, 0, 0, 118, 148, 670, 700, NULL);
		al_draw_bitmap_region(townhousedown, 0, 0, 118, 148, 787, 700, NULL);
		al_draw_bitmap_region(townhousedown, 0, 0, 118, 148, 905, 700, NULL);

		//Houses facing right on right side of screen
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1101, 201, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1101, 320, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1101, 439, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1101, 548, NULL);
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1101, 667, NULL);

		//al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 786, NULL);
		//House facing right at bottom of screen
		al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 1024, NULL);

		//House on right hand side facing left
		al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 1450, 201, NULL);
		al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 1450, 320, NULL);
		al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 1450, 439, NULL);
		al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 1450, 548, NULL);
		al_draw_bitmap_region(townhouseleft, 0, 0, 148, 118, 1450, 667, NULL);

		//Generate player
		if (life > 0)
		{
		al_draw_bitmap_region(player1, sourceX, sourceY * al_get_bitmap_height(player1) / 4, 40, 40, x - 19, y - 19, NULL);
		}
		else if(life==0)
		{
		al_show_native_message_box(display, "Zombies: The Final Hour", "YOU HAVE DIED!", NULL, NULL, ALLEGRO_MESSAGEBOX_OK_CANCEL);
		keys[UP] = false;
		keys[DOWN] = false;
		keys[LEFT] = false;
		keys[RIGHT] = false;
		keys[SPACE] = false;
		x = -100;
		y = -100;
		exp = 0;
		life = 3;
		dir = 0;
		int gameTime = 0;
		moveSpeed = 1.5;
		gamestate = 2;
		}

		//Enemies
		//al_draw_circle(originx, originy, aggrorange, al_map_rgba_f(.5, .5, .5, .5), 1);
		//al_draw_circle(zombposx, zombposy, aggrorange, al_map_rgba_f(.5, 0, .5, .5), 1);
		if (zomblife > 0)
		{
		al_draw_bitmap_region(zombie, sourceZ, sourceA * al_get_bitmap_height(zombie) / 4, 50, 50, zombposx - 24, zombposy - 24, NULL);
		}
		else
		{
		int q = 0;
		zombposx = -1000;
		zombposy = -1000;
		q = exp + 1;
		al_draw_textf(font18, al_map_rgb(255, 255, 255), 645, 5, ALLEGRO_ALIGN_RIGHT,
		"Exp: %.0i", q);
		}

		//Draw bullets
		DrawBullet(bullets, NUM_BULLETS);

		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));
		}

		if ((x > 1730) && (x < 1760) && (y > 820) && (y < 850))
		{
		al_show_native_message_box(display, "Zombies: The Final Hour", "LEVEL CONQUERED!", NULL, NULL, ALLEGRO_MESSAGEBOX_OK_CANCEL);
		keys[UP] = false;
		keys[DOWN] = false;
		keys[LEFT] = false;
		keys[RIGHT] = false;
		keys[SPACE] = false;
		exp = 0;
		life = 3;
		moveSpeed = 1.5;
		dir = 0;
		gameTime = 0;
		x = -100;
		y = -100;
		gamestate = 2;
		}

		}*/

		//Controls menu
		if (gamestate == 3)
		{
			ALLEGRO_EVENT ev3;
			al_wait_for_event(event_queue, &ev3);
			if (ev3.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
				pos_x = ev3.mouse.x;
				pos_y = ev3.mouse.y;
			}

			al_draw_bitmap(controlsmenu, 0, 0, 0);

			al_draw_textf(font18, al_map_rgb(255, 255, 255), 500, 5, ALLEGRO_ALIGN_RIGHT,
				"mouse pos x: %.0i", pos_x);

			al_draw_textf(font18, al_map_rgb(255, 255, 255), 1000, 5, ALLEGRO_ALIGN_RIGHT,
				"mouse pos y: %.0i", pos_y);

			//Return to main menu
			if ((pos_x < 817) && (pos_x > 48) && (pos_y < 1026) && (pos_y > 839))
			{
				pos_x = -100;
				pos_y = -100;
				gamestate = 2;
			}

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}	
}

//Destroy all windows
	al_show_native_message_box(display, "Zombies: The Final Hour", "Do you really want to exit?", NULL, NULL, ALLEGRO_MESSAGEBOX_YES_NO);

	al_destroy_bitmap(title);
	al_destroy_bitmap(player1);
	al_destroy_bitmap(townhouse);
	al_destroy_bitmap(townhousedown);
	al_destroy_bitmap(townhouseleft);
	al_destroy_bitmap(deadtree);
	al_destroy_bitmap(clock);
	al_destroy_bitmap(apocalypse);
	al_destroy_bitmap(controlsmenu);
	al_destroy_bitmap(Health);
	al_destroy_bitmap(inventory);
	al_destroy_bitmap(house);
	al_destroy_bitmap(policecar);
	al_destroy_bitmap(redcar);
	al_destroy_bitmap(zombie);
	al_destroy_bitmap(map1);
	al_destroy_bitmap(minimap);
	al_destroy_bitmap(emptyheart);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);
	return 0;
}




