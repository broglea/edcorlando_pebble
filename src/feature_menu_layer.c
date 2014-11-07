#include "pebble.h"
#include "pebble_fonts.h"
#include <time.h>
#include <math.h>

#define MenuIndex(section,row) ((MenuIndex){ (section), (row) })


#define NUM_MENU_SECTIONS 1
#define NUM_ARTISTS 68
#define NUM_DAYS 2
#define NUM_SATURDAY_ARTISTS 28
#define NUM_FRIDAY_ARTISTS 32
#define TIME_ZONE_OFFSET -5


static Window *window;
static Window *friday_window;
static Window *saturday_window;
static Window *text_window;

// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *menu_layer;
static Layer *window_layer;
static MenuLayer *friday_layer;
static MenuLayer *saturday_layer;
static TextLayer *artist_text_layer;
static TextLayer *time_text_layer;
static TextLayer *stage_text_layer;

// You can draw arbitrary things in a menu item such as a background

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t days_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_DAYS;
    default:
      return 0;
  }
}

static uint16_t friday_artists_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return NUM_FRIDAY_ARTISTS;
}

static uint16_t saturday_artists_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return NUM_SATURDAY_ARTISTS;
}

static const char *stages[] = {
  "Neon Garden",
  "Kinetic Field",
  "Circuit Grounds"
};

static const char *artists_friday[] =
{
    "David Steven",
    "Illustrated",
    "Must Die!",
    "Max Vangeli",
    "Protohype",
    "Arctic Moon",
    "Ummet Ozcan",
    "Flinch",
    "RAM",
    "Arty",
    "Planet of the Drums",
    "3LAU",
    "Antiserum vs. Mayhem",
    "Indecent Noise",
    "Yellow Claw",
    "Ookay B2B Kennedy Jones",
    "Emma Hewitt",
    "Cedric Gervais",
    "Dirtyphonics",
    "Myon & Shane 54",
    "Sander van Doorn",
    "Orjan Nilsen",
    "Brillz",
    "W&W",
    "Cosmic Gate",
    "Noisia",
    "Datsik",
    "Dash Berlin",
    "Andrew Rayel",
    "Aly & Fila",
    "Flosstradamus",
    "Alesso"
};

static const char *artists_saturday[] =
{
    "Fishfood",
    "Richy",
    "Alesia",
    "Hunter/Game",
    "MOTi",
    "Vice",
    "Harvard Bass",
    "Isaac",
    "Paper Diamond",
    "Route 94",
    "Rebecca & Fiona",
    "Headhunterz",
    "Henry Fong",
    "SBCR DJ Set (The Bloody Beetroots)",
    "Amine Edge & DANCE",
    "Thomas Gold",
    "Sunnery James & Ryan Marciano",
    "Tritonal",
    "Green Velvet",
    "Dillon Francis",
    "Oliver Heldens",
    "Showtek",
    "Umek",
    "Above & Beyond",
    "Steve Aoki",
    "Claude VonStroke",
    "Steve Angello",
    "Knife Party"
};

static const char *times_friday[] =
{
    "01:00 PM",
    "01:00 PM",
    "01:00 PM",
    "01:59 PM",
    "02:00 PM",
    "02:40 PM",
    "02:56 PM",
    "03:00 PM",
    "03:40 PM",
    "03:53 PM",
    "04:00 PM",
    "04:50 PM",
    "05:00 PM",
    "05:10 PM",
    "05:56 PM",
    "06:00 PM",
    "06:40 PM",
    "06:57 PM",
    "07:00 PM",
    "07:00 PM",
    "07:58 PM",
    "08:00 PM",
    "08:00 PM",
    "08:59 PM",
    "09:00 PM",
    "09:00 PM",
    "10:00 PM",
    "10:00 PM",
    "10:00 PM",
    "11:00 PM",
    "11:00 PM",
    "11:01 PM"
};

static const char *times_saturday[] =
{
    "01:00 PM",
    "01:00 PM",
    "01:30 PM",
    "01:30 PM",
    "02:30 PM",
    "02:30 PM",
    "03:00 PM",
    "03:30 PM",
    "03:53 PM",
    "04:30 PM",
    "04:30 PM",
    "04:50 PM",
    "05:30 PM",
    "05:56 PM",
    "06:00 PM",
    "06:30 PM",
    "06:57 PM",
    "07:30 PM",
    "07:30 PM",
    "07:58 PM",
    "08:30 PM",
    "08:59 PM",
    "09:00 PM",
    "09:30 PM",
    "10:00 PM",
    "10:30 PM",
    "11:00 PM",
    "11:01 PM"
};

static const time_t unix_times_friday[] = {
  1415365200,
  1415365200,
  1415365200,
  1415368740,
  1415368800,
  1415371200,
  1415372160,
  1415372400,
  1415374800,
  1415375580,
  1415376000,
  1415379000,
  1415379600,
  1415380200,
  1415382960,
  1415383200,
  1415385600,
  1415386620,
  1415386800,
  1415386800,
  1415390280,
  1415390400,
  1415390400,
  1415393940,
  1415394000,
  1415394000,
  1415397600,
  1415397600,
  1415397600,
  1415401200,
  1415401200,
  1415401260
};

static const time_t unix_times_saturday[] = {
  1415451600,
  1415453400,
  1415453400,
  1415457000,
  1415457000,
  1415458800,
  1415460600,
  1415461980,
  1415464200,
  1415464200,
  1415465400,
  1415467800,
  1415469360,
  1415469600,
  1415471400,
  1415473020,
  1415475000,
  1415475000,
  1415476680,
  1415478600,
  1415480340,
  1415480400,
  1415482200,
  1415484000,
  1415485800,
  1415487600,
  1415487660
};

static const int stages_friday[] = { 1, 0, 2, 1, 2, 0, 1, 2, 0, 1, 2, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 0, 2, 1, 0, 2, 2, 1, 0, 0, 2, 1 };
static const int stages_saturday[] = { 0, 1, 2, 0, 2, 1, 0, 2, 1, 0, 2, 1, 2, 1, 0, 2, 1, 2, 0, 1, 2, 1, 0, 2, 1, 0, 2, 1 };


// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, "Artists");
}

// This is the menu item draw callback where you specify what each item should look like
static void friday_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  menu_cell_basic_draw(ctx, cell_layer, artists_friday[cell_index->row], times_friday[cell_index->row], NULL);
}

// Here we capture when a user selects a menu item
void friday_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  window_stack_push(text_window, true);
  text_layer_set_text(artist_text_layer, artists_friday[cell_index->row]);
  text_layer_set_text(time_text_layer, times_friday[cell_index->row]);
  text_layer_set_text(stage_text_layer, stages[stages_friday[cell_index->row]]);
  layer_mark_dirty(text_layer_get_layer(artist_text_layer));
  layer_mark_dirty(text_layer_get_layer(time_text_layer));
  layer_mark_dirty(text_layer_get_layer(stage_text_layer));
}

// This is the menu item draw callback where you specify what each item should look like
static void saturday_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  menu_cell_basic_draw(ctx, cell_layer, artists_saturday[cell_index->row], times_saturday[cell_index->row], NULL);
}

// Here we capture when a user selects a menu item
void saturday_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  window_stack_push(text_window, true);
  text_layer_set_text(artist_text_layer, artists_saturday[cell_index->row]);
  text_layer_set_text(time_text_layer, times_saturday[cell_index->row]);
  text_layer_set_text(stage_text_layer, stages[stages_saturday[cell_index->row]]);
  layer_mark_dirty(text_layer_get_layer(artist_text_layer));
  layer_mark_dirty(text_layer_get_layer(time_text_layer));
  layer_mark_dirty(text_layer_get_layer(stage_text_layer));
}

// Here we draw what each header is
static void days_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, "Days");
}

// This is the menu item draw callback where you specify what each item should look like
static void days_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  switch(cell_index -> row) {
    case 0:
    menu_cell_basic_draw(ctx, cell_layer, "Friday", NULL, NULL);
    break;

    case 1:
    menu_cell_basic_draw(ctx, cell_layer, "Saturday", NULL, NULL);
    break;
  }
}

//Friday = 0, Saturday = 1: Shitty O(n) solution on a sorted list lel
int find_closest_time(int day){
  time_t timer;
  time(&timer);
  //app_log(APP_LOG_LEVEL_INFO, "feature_menu_layer.c", 340, "Time is: %ld\n", (long)timer);
  double difference, temp;
  int result = 0, i;
  if(day == 0){
    difference = abs(difftime(unix_times_friday[0], timer));
    for(i=1; i<NUM_FRIDAY_ARTISTS; i++){
      temp = difftime(unix_times_friday[i], timer);
      if(temp >= 0 && temp < difference){
        difference = temp;
        result = i;
      }
    }
  }
  
  if(day == 1){
    difference = abs(difftime(unix_times_saturday[0], timer));
    for(i=1; i<NUM_SATURDAY_ARTISTS; i++){
      temp = difftime(unix_times_saturday[i], timer);
      if(temp >= 0 && temp < difference){
        difference = temp;
        result = i;
      }
    }
  }
 return result; 
}

// Here we capture when a user selects a menu item
void days_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  
  switch (cell_index->row) {
    case 0:
      //data = "test";
      window_stack_push(friday_window, true);
      menu_layer_set_selected_index(friday_layer,MenuIndex(0,find_closest_time(0)), MenuRowAlignCenter, true);
      //layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
    case 1:
      window_stack_push(saturday_window, true);
      menu_layer_set_selected_index(saturday_layer,MenuIndex(0,find_closest_time(1)), MenuRowAlignCenter, true);
      break;
  }
}


// This initializes the menu upon window load
void days_window_load(Window *window) {
  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);
  friday_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = days_menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = days_menu_draw_header_callback,
    .draw_row = days_menu_draw_row_callback,
    .select_click = days_menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

void days_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
}
// This initializes the menu upon window load
void friday_window_load(Window *window) {
  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  friday_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(friday_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = friday_artists_menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = friday_menu_draw_row_callback,
    .select_click = friday_menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(friday_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(friday_layer));
}

void friday_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(friday_layer);
}

void saturday_window_load(Window *window) {
  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  saturday_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(saturday_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = saturday_artists_menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = saturday_menu_draw_row_callback,
    .select_click = saturday_menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(saturday_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(saturday_layer));
}

void saturday_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(saturday_layer);
}

// This initializes the menu upon window load
void text_window_load(Window *window) {
  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  int16_t bounds_size = bounds.size.h;
  
  bounds_size /= 3;
  bounds.size.h = bounds_size;
  
  // Create the menu layer
  artist_text_layer = text_layer_create(bounds);
  bounds.origin.y += bounds_size;
  time_text_layer = text_layer_create(bounds);
  bounds.origin.y += bounds_size;
  stage_text_layer = text_layer_create(bounds);
  text_layer_set_font(artist_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_font(time_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_font(stage_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  // Add it to the window for display
  layer_add_child(window_layer, text_layer_get_layer(artist_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(time_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(stage_text_layer));
}

void text_window_unload(Window *window) {
  // Destroy the text layer
  text_layer_destroy(artist_text_layer);
  text_layer_destroy(time_text_layer);
  text_layer_destroy(stage_text_layer);
}

int main(void) {
  window = window_create();
  friday_window = window_create();
  saturday_window = window_create();
  
  text_window = window_create();

  // Setup the window handlers
  window_set_window_handlers(window, (WindowHandlers) {
    .load = days_window_load,
    .unload = days_window_unload,
  });
  
  window_set_window_handlers(friday_window, (WindowHandlers) {
    .load = friday_window_load,
    .unload = friday_window_unload,
  });
  
  window_set_window_handlers(saturday_window, (WindowHandlers) {
    .load = saturday_window_load,
    .unload = saturday_window_unload,
  });
  
  window_set_window_handlers(text_window, (WindowHandlers){
    .load = text_window_load,
    .unload = text_window_unload,
  });

  window_stack_push(window, true /* Animated */);

  app_event_loop();

  window_destroy(window);
}