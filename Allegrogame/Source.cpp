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

bool keys[] = { false, false, false, false, false};
enum KEYS { DOWN, LEFT, RIGHT, UP, SPACE};

//Start of Artificial intelligence
enum STATE{IDLE,ATTACKING,RETURNING};

bool Collision(float x, float y, float ex, float ey, int width, int height)
{
	if (((x + 25<ex) || (x>ex + width)) || ((y + 25<ey) || (y>ey + height)))
	{
		return false;
	}
	return true;
}

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

float calcdistancefromchartozombie(int x1, int y1, int x2, int y2)
{
	return sqrt(pow((float)x1 - x2, 2) + pow((float)y1 - y2, 2));
}

float movingatanagletocharacter(int x1, int y1, int x2, int y2)
{
	float deltaX = (x2 - x1);
	float deltaY = (y2 - y1);
	return atan2(deltaY, deltaX);
}

int main(void)
{
//Initializers
	al_init();
	al_install_keyboard();
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
	
	//Enemies
	int state = -1;

	//Origin
	const int originx = WIDTH - 1000;
	const int originy = HEIGHT / 2;
	int dir1 = DOWN;
	bool lives = 5;
	//Zombies position
	int zombposx = originx;
	int zombposy = originy;
	int sourceA = 0, sourceZ = 250;
	int aggrorange = 220;

	//Character
	float x = 400, y = 400, x1 = 10, y1 = 10, moveSpeed = 2.5;
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
	ALLEGRO_BITMAP *player = al_load_bitmap("NEWPOSTWBACK.png");
	al_convert_mask_to_alpha(player, al_map_rgb(241, 23, 23));

	//Load zombie bitmap
	ALLEGRO_BITMAP *zombie = al_load_bitmap("zombiespriteFINISHED.png");
	al_convert_mask_to_alpha(zombie, al_map_rgb(241, 243, 40));

	ALLEGRO_BITMAP *title = al_load_bitmap("zombies.png");
	ALLEGRO_BITMAP *map1 = al_load_bitmap("FIXEDMAP.png");
	ALLEGRO_BITMAP *house = al_load_bitmap("house1.png");
	ALLEGRO_BITMAP *townhousedown = al_load_bitmap("townhouse2.png");
	ALLEGRO_BITMAP *townhouse = al_load_bitmap("townhouse.png");

	ALLEGRO_KEYBOARD_STATE keystate;
	ALLEGRO_FONT *font18 = al_load_font("arial.ttf", 18, 0);
	stateswitch(state, IDLE);

	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
	
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_start_timer(timer);
	gameTime = al_current_time();

	//Loading page
	al_draw_bitmap(title, 0, 0, 0);
	al_flip_display();
	al_rest(2.0);

	while (!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		al_get_keyboard_state(&keystate);

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
				break;
			}
		}

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
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

			if (x > WIDTH-20) //Right
				x = WIDTH-20;

			if (y > HEIGHT-20) //Down
				y = HEIGHT-20;

			if (y < 20) //Up
				y = 20;

			active = true;
			if (al_key_down(&keystate, ALLEGRO_KEY_DOWN))
			{
				y += moveSpeed;
				dir = DOWN;
			}
			else if (al_key_down(&keystate, ALLEGRO_KEY_UP))
			{
				y -= moveSpeed;
				dir = UP;
			}
			else if (al_key_down(&keystate, ALLEGRO_KEY_RIGHT))
			{
				x += moveSpeed;
				dir = RIGHT;
			}
			else if (al_key_down(&keystate, ALLEGRO_KEY_LEFT))
			{
				x -= moveSpeed;
				dir = LEFT;
			}
			else
				active = false;

			if (active)
				sourceX += al_get_bitmap_width(player) / 15;
			else
				sourceX = 200;

			if (sourceX >= al_get_bitmap_width(player))
				sourceX = 0;
			sourceY = dir;
		
			draw = true;

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
				
			//Collisions
				
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

				if (Collision(x, y, 1051, 201, 123, 703))
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
				if (Collision(x, y, 1051, 1023, 123, 200))
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
		}

		if (draw)
		{
			//Map
			for (int i = 0; i < mapSize; i++)
			{
				al_draw_bitmap_region(map1, tileSize * map[i], 0, tileSize, tileSize,
				xcam + tileSize * (i % mapcolumn), ycam + tileSize * (i / mapcolumn), 0);
			}
			//display FPS on screen
			al_draw_textf(font18, al_map_rgb(255, 0, 255), 5, 5, 0, "FPS: %i", gameFPS);

			al_draw_textf(font18, al_map_rgb(255, 255, 255), 200, 5, ALLEGRO_ALIGN_RIGHT,
				"X pos: %.0f", x); 
			al_draw_textf(font18, al_map_rgb(255, 255, 255), 300, 5, ALLEGRO_ALIGN_RIGHT,
				"Y pos: %.0f", y); 

			//Generate house collision
			al_draw_bitmap_region(house, 0, 0, 143, 100, 1695, 760, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 201, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 320, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 439, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 548, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 667, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 786, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 905, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 101, 1024, NULL);

			al_draw_bitmap_region(townhousedown, 0, 0, 118, 148, 1050, 750, NULL);

			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 201, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 320, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 439, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 548, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 667, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 786, NULL);
			al_draw_bitmap_region(townhouse, 0, 0, 148, 118, 1051, 1024, NULL);
			//Generate player
			al_draw_bitmap_region(player, sourceX, sourceY * al_get_bitmap_height(player) / 4, 40, 40, x-19, y-19, NULL);

			//Enemies														
			//al_draw_circle(originx, originy, aggrorange, al_map_rgba_f(.5, .5, .5, .5), 1);
			//al_draw_circle(zombposx, zombposy, aggrorange, al_map_rgba_f(.5, 0, .5, .5), 1);
				
			al_draw_bitmap_region(zombie, sourceZ, sourceA * al_get_bitmap_height(zombie) / 4, 50, 50, zombposx-24, zombposy-24, NULL);
			
			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}
		if (x==1750 && y==850)
		{
			al_show_native_message_box(display, "Zombies: The Final Hour", "LEVEL CONQUERED!", NULL, NULL, ALLEGRO_MESSAGEBOX_OK_CANCEL);
			done = true;
		}
	}

	//Destroy all windows
	al_show_native_message_box(display, "Zombies: The Final Hour", "Do you really want to exit?", NULL, NULL, ALLEGRO_MESSAGEBOX_YES_NO);
	al_destroy_bitmap(title);
	al_destroy_bitmap(player);
	al_destroy_bitmap(townhouse);
	al_destroy_bitmap(townhousedown);
	al_destroy_bitmap(house);
	al_destroy_bitmap(zombie);
	al_destroy_bitmap(map1);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);
	return 0;
}




