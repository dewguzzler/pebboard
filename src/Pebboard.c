#include <pebble.h>

#define TOP 0
#define MID 1
#define BOT 2
#define KEY_BUTTON    0
#define KEY_VIBRATE   1

#define BUTTON_UP     0
#define BUTTON_SELECT 1
#define BUTTON_DOWN   2

#define NOTEPAD_TEXT 1
#define NOTEPAD_CHAR_COUNT 2
#define PHONE_NUMBER 3
#define PHONE_POS 4

static Window* window;
//new name? Keyble, Peyboard?
static TextLayer* text_layer;
static TextLayer* wordsYouWrite;
static TextLayer* phoneNumber;
static TextLayer* pbreak;
static TextLayer* numnum;
static char scroll_text = ' ';
static ScrollLayer* scrollTweets;

static TextLayer* buttons1[13];
static TextLayer* buttons2[13];
//static TextLayer* buttons3[3];
static TextLayer** bbuttons[2];

static bool menu = false;

// Here are the three cases, or sets
static char caps[] =    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char letters[] = "abcdefghijklmnopqrstuvwxyz";
//static char numsym[] = "1234567890!?-'$()&*+#:@/,.";
static char numsym[] = "1234567890-;.!@#$%^&*()_'?";
static char pNumPer[] = "1234567890\0";
static char numnums[] = "#";

// the below three strings just have to be unique, abc - xyz will be overwritten with the long strings above
static char* btext1[] = {"a\0","b\0","c\0","d\0","e\0","f\0","g\0","h\0","i\0","j\0","k\0","l\0","m\0"};
static char* btext2[] = {"n\0","o\0","p\0","q\0","r\0","s\0","t\0","u\0","v\0","w\0","x\0","y\0","z\0"};
//static char* btext3[] = {"@   SPACE    #"};
static char** btexts[] = {btext1, btext2};

// These are the actual sets that are displayed on each button, also need to be unique
static char set1[4] = "  a\0";
static char set2[4] = "  b\0";
static char set3[4] = "  c\0";
static char* setlist[] = {set1, set2, set3};

static char* cases[] = {"CAP", "low", "#@1"};
static GBitmap *s_example_bitmap;
static int phone_cur = 0;
static int cur_set = 1;
static int cur_row = 0;
static int cur_column = 0;
static int whichField = 1;
static bool blackout = false;

static void drawSides();
static void drawMenu();
static void set_menu();
static void drawNotepadText();
static GFont custom_font;
static GFont c_font_14;
static GFont c_font_18;


static char* rotate_text[] = {numsym, caps, letters};
static void next();

static char* master = numsym;

static char text_buffer[150];
static char p_buffer[10];
static char *sendingtext;
static int pos = 0;
static int ppos = 0;
static int top, end, size;

static char *sendsomething = "hey fucker\0";

// This function changes the next case/symbol set.
static void change_set(int s, bool lock)
{
    int count = 0;
    master = rotate_text[s];
    
        for (int j=0; j<2; j++)
        {
          for (int k=0; k<13; k++)
            {
                btexts[j][k][0] = master[count];
                count++;
            }
        }
    

    menu = false;
    if (lock) cur_set = s;
    
    drawSides();
    	   text_layer_set_background_color(bbuttons[cur_row][cur_column], GColorBlack);
	   text_layer_set_text_color(bbuttons[cur_row][cur_column], GColorClear);
}

static void send(int key, int message) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  static char together[sizeof(p_buffer) + sizeof(text_buffer) + 2];
  snprintf(together, sizeof(together), "# %s %s", p_buffer, text_buffer);
  //dict_write_int(iter, key, &message, sizeof(int), true);
  dict_write_cstring(iter, 0, together);
  app_message_outbox_send();
}

static void sendMe(){
  DictionaryIterator *iter2;
  app_message_outbox_begin(&iter2);
  dict_write_cstring(iter2, 0, text_buffer);
  app_message_outbox_send();
}

// static void inbox_received_handler(DictionaryIterator *iterator, void *context) {
//   // Get the first pair
//   Tuple *t = dict_read_first(iterator);

//   // Process all pairs present
//   while(t != NULL) {
//     // Process this pair's key
//     switch(t->key) {
//       case KEY_VIBRATE:
//         // Trigger vibration
//         //text_layer_set_text(s_text_layer, "Vibrate!");
//         vibes_short_pulse();
//         break;
//       default:
//         APP_LOG(APP_LOG_LEVEL_INFO, "Unknown key: %d", (int)t->key);
//         break;
//     }

//     // Get next pair, if any
//     t = dict_read_next(iterator);
//   }
// }

// static void inbox_dropped_handler(AppMessageResult reason, void *context) {
//   APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
// }

static void outbox_failed_handler(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_handler(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
 }

static void next()
{
    top = 0;
    end = 25;
    size = 26;
}

static void up_long_release_handler(ClickRecognizerRef recognizer, void *window) {}
static void select_long_release_handler(ClickRecognizerRef recognizer, void *window) {}
static void down_long_release_handler(ClickRecognizerRef recognizer, void *window) {}

static void clickButton(int b)
{
int charcount = 0;
for(int i = 0; i < 140; i++) {
    if (text_buffer[i] != '\0') {
	charcount++;
	}
   }
drawNotepadText();
//APP_LOG(APP_LOG_LEVEL_DEBUG, "charcount:%u", charcount);
    if(pos < 140) {
     if(b == top) {
	if(cur_row==0) {
	   cur_row = 1;
	   text_layer_set_text_color(bbuttons[1][cur_column], GColorClear);
	   text_layer_set_text_color(bbuttons[0][cur_column], GColorBlack);
	   text_layer_set_background_color(bbuttons[1][cur_column], GColorBlack);
	   text_layer_set_background_color(bbuttons[0][cur_column], GColorClear);
	   }
	else {
	   cur_row=0;
	   text_layer_set_text_color(bbuttons[0][cur_column], GColorClear);
	   text_layer_set_text_color(bbuttons[1][cur_column], GColorBlack);
	   text_layer_set_background_color(bbuttons[0][cur_column], GColorBlack);
	   text_layer_set_background_color(bbuttons[1][cur_column], GColorClear);
	   }
     switch(whichField) {
      case 1:
        p_buffer[ppos] = btexts[cur_row][cur_column][0];
        break;
      case 2:
        text_buffer[pos] = btexts[cur_row][cur_column][0];
        break;
     }
	}
     else if(b == BOT) { 
	    if (cur_column > 11) { 
		cur_column = 0;
	   text_layer_set_background_color(bbuttons[cur_row][12], GColorClear);
	   text_layer_set_text_color(bbuttons[cur_row][12], GColorBlack);
		} else { 
		cur_column = cur_column + 1;
	   text_layer_set_background_color(bbuttons[cur_row][cur_column-1], GColorClear);
	   text_layer_set_text_color(bbuttons[cur_row][cur_column-1], GColorBlack);
		}
           switch(whichField) {
      case 1:
        p_buffer[ppos] = btexts[cur_row][cur_column][0];
        break;
      case 2:
        text_buffer[pos] = btexts[cur_row][cur_column][0];
        break;
     }
	   text_layer_set_background_color(bbuttons[cur_row][cur_column], GColorBlack);
	   text_layer_set_text_color(bbuttons[cur_row][cur_column], GColorClear);
           //drawNotepadText();
	}
    else if (b == MID) {
          switch(whichField) {
            case 1:
              p_buffer[ppos++] = btexts[cur_row][cur_column][0];
              break;
            case 2:
              text_buffer[pos++] = btexts[cur_row][cur_column][0];
            break;
          }
           //drawNotepadText();
	   
           }
        //drawSides();
    }
 drawNotepadText();
    
}

// Modify these common button handlers
static void up_single_click_handler(ClickRecognizerRef recognizer, void* context) {
    
    clickButton(TOP);
    
}

static void select_single_click_handler(ClickRecognizerRef recognizer, void* context) {
    
    clickButton(MID);
}

static void down_single_click_handler(ClickRecognizerRef recognizer, void* context) {
    
    clickButton(BOT);
}

static bool common_long(int b)
{
    if (menu)
    {
        change_set(b, true);
        return true;
    }
    return false;
}

static void up_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
  const uint16_t count = click_number_of_clicks_counted(recognizer);
  if(count == 2) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Multi-Click: count:%u", count);
	text_buffer[pos++] = '#';
           drawNotepadText();
  }
}

static void up_long_click_handler(ClickRecognizerRef recognizer, void* context) {
    
    if (common_long(TOP)) return;
    
     else if(cur_set<2) {
        change_set(cur_set+1, true);
	}
     else {
	change_set(0,true);
       }

    
}

static void select_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
  const uint16_t count = click_number_of_clicks_counted(recognizer);
  if(count == 2) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Multi-Click: count:%u", count);
	text_buffer[pos++] = ' ';
           drawNotepadText();
  }
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void* context) {
    
    if (common_long(MID)) return;
    
//    blackout = !blackout;
//    
//    if (blackout)
//    text_layer_set_background_color(text_layer, GColorBlack);
//    else
//    text_layer_set_background_color(text_layer, GColorClear);
    
    // clear the string
    text_buffer[pos++] = '\0';
    
    send(5,6);
    sendMe();
    snprintf(p_buffer, sizeof(p_buffer), "          ");
    for (int i=0; i<140; i++)
        text_buffer[i] = '\0';
    pos = 0;
    ppos = 0;
    whichField = 1;
    change_set(0, true);
	text_layer_set_text(wordsYouWrite, text_buffer);
	GSize max_size = text_layer_get_content_size(wordsYouWrite);
	scroll_layer_set_content_size(scrollTweets, max_size);
	int x = max_size.h < 100?100-max_size.h:max_size.h - 100;
	GPoint diffs = GPoint(0,0- max_size.h);
        GSize scrolsize = scroll_layer_get_content_size(scrollTweets);
    drawNotepadText();
    //drawSides();

//  APP_LOG(APP_LOG_LEVEL_DEBUG, text_buffer);

//  APP_LOG(APP_LOG_LEVEL_DEBUG, "pos %u", pos);
    
}

static void back_single_click_handler(ClickRecognizerRef recognizer, void *context) {

}

static void down_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
  const uint16_t count = click_number_of_clicks_counted(recognizer);
  if(count == 2) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Multi-Click: count:%u", count);
	text_buffer[pos++] = '@';
           drawNotepadText();
  }
}


static void down_long_click_handler(ClickRecognizerRef recognizer, void* context) {
    
    if (common_long(BOT)) return;
    switch(whichField) {
      case 1:
        if(ppos != 0){
          if(p_buffer[ppos] != ' '){
            p_buffer[ppos] = ' ';
          } else {
            p_buffer[--ppos] = ' ';
          }
          
          drawNotepadText();
        }
        break;
      case 2:
        //delete or cancel when back is held
      if(pos != 0) {
        if (text_buffer[pos] != ' ') {
          text_buffer[pos] = ' ';
	      } else { 
          text_buffer[--pos] = ' ';
        }
          drawNotepadText();
      }
        break;
    }
 //    for (int i=0; i<140; i++)
 //        text_buffer[i] = '\0';
 //    pos = 0;
	// text_layer_set_text(wordsYouWrite, text_buffer);
	// GSize max_size = text_layer_get_content_size(wordsYouWrite);
	// scroll_layer_set_content_size(scrollTweets, max_size);
	// int x = max_size.h < 100?100-max_size.h:max_size.h - 100;
	// GPoint diffs = GPoint(0,0- max_size.h);
 //        GSize scrolsize = scroll_layer_get_content_size(scrollTweets);
 //    drawNotepadText();
	
}

static void set_menu()
{
    if (!blackout)
    {
        menu = true;
        drawMenu();
    }
}

// This usually won't need to be modified

static void click_config_provider(void* context) {
    
    window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
    window_long_click_subscribe(BUTTON_ID_UP, 500, up_long_click_handler, up_long_release_handler);
    window_multi_click_subscribe(BUTTON_ID_UP, 2, 10, 100, true, up_multi_click_handler);
    
    window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
    window_long_click_subscribe(BUTTON_ID_SELECT, 1000, select_long_click_handler, select_long_release_handler);
    window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 100, true, select_multi_click_handler);
    
    //window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN,200, down_single_click_handler);
    //window_long_click_subscribe(BUTTON_ID_DOWN, 1000, down_long_click_handler, down_long_release_handler);
    //window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 10, 100, true, down_multi_click_handler);

    window_single_click_subscribe(BUTTON_ID_BACK, down_long_click_handler);

}

static void drawMenu()
{
    for (int i=0; i<3; i++)
    {
        text_layer_set_text(bbuttons[i][i!=2], " ");
        text_layer_set_text(bbuttons[i][2], " ");
        
        text_layer_set_text(bbuttons[i][i==2], cases[i]);
        text_layer_set_font(bbuttons[i][0], c_font_14);
    }
}


// This method draws the characters on the right side near the buttons
static void drawSides()
{
    if (size==27) // first click (full size)
    {
        // update all 9 labels to their proper values
        for (int h=0; h<1; h++)
        {
            for (int i=0; i<2; i++)
            {
                text_layer_set_text(bbuttons[h][i], btexts[h][i]);
                text_layer_set_background_color(bbuttons[h][i], GColorClear);
                text_layer_set_font(bbuttons[h][i], fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
            }
            
        }
    }
    else if (size==9)   // second click
    {
        
        for (int i=0; i<3; i++)
        {
            text_layer_set_text(bbuttons[i][i!=2], " ");
            text_layer_set_text(bbuttons[i][2], " ");
            
            text_layer_set_text(bbuttons[i][i==2], btexts[top/9][i]);
            text_layer_set_font(bbuttons[i][i==2], fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
        }
        
    } else if (size == 3)
    {
        for (int i=0; i<3; i++)
        {
            setlist[i][2] = master[top+i];
            text_layer_set_text(bbuttons[i][i==2], setlist[i]);
            
        }
    } else
    {
      // APP_LOG(APP_LOG_LEVEL_DEBUG, "size:%u", size);
        for (int h=0; h<2; h++)
        {
            for (int i=0; i<13; i++)
            {            
		          text_layer_set_text(bbuttons[h][i], btexts[h][i]);
		          text_layer_set_text_alignment(bbuttons[h][i], GTextAlignmentCenter);
              text_layer_set_background_color(bbuttons[h][i], GColorClear);
              text_layer_set_font(bbuttons[h][i], c_font_14);
            }
            
        }
}
    
}

static void initSidesAndText()
{
    Layer *window_layer = window_get_root_layer(window);
    scrollTweets = scroll_layer_create((GRect) { .origin = { 0, 25 }, .size = { 144, 100 } });
    wordsYouWrite = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { 144, 2000 } });
    text_layer_set_overflow_mode(wordsYouWrite, GTextOverflowModeWordWrap);
    text_layer_set_background_color(wordsYouWrite, GColorClear);
    text_layer_set_font(wordsYouWrite, custom_font);
    text_layer_set_text(wordsYouWrite, text_buffer);
    layer_add_child(window_layer, scroll_layer_get_layer(scrollTweets));
    scroll_layer_add_child(scrollTweets, text_layer_get_layer(wordsYouWrite));
    
    for (int i = 0; i<13; i++)
    {
        buttons1[i] = text_layer_create((GRect) { .origin = { i*(164/14), 130 }, .size = { 164/14, 18 } });
        buttons2[i] = text_layer_create((GRect) { .origin = { i*(164/14), 147 }, .size = { 164/14, 18 } });
        //buttons3[i] = text_layer_create((GRect) { .origin = { 5, 12*i + 113 }, .size = { 20, 100 } });
    }
    
    for (int i=0; i<2; i++)
        for (int j=0; j<13; j++) {
            layer_add_child(window_layer, text_layer_get_layer(bbuttons[i][j]));
	    //APP_LOG(APP_LOG_LEVEL_DEBUG, bbuttons[i][j]);
	  }
    
}

static void drawNotepadText()
{
	text_layer_set_text(wordsYouWrite, text_buffer);
  text_layer_set_text(phoneNumber, p_buffer);
	GSize max_size = text_layer_get_content_size(wordsYouWrite);
	scroll_layer_set_content_size(scrollTweets, max_size);
	int x = max_size.h < 100?100-max_size.h:max_size.h - 100;
	GPoint diffs = GPoint(0,0- max_size.h);
        GSize scrolsize = scroll_layer_get_content_size(scrollTweets);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "contentsize = %u", max_size.h);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "scroll size = %u", scrolsize.h);
	scroll_layer_set_content_offset(scrollTweets, diffs ,false);
  if(whichField == 1 && ppos > 9){
    whichField = 2;
    pos = 0;
  }
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "contentofset = %u", sizeof text_buffer);
}

static void deinit(void) {
    text_buffer[pos] = '\0';
    //persist_write_string(NOTEPAD_TEXT, text_buffer);
    //persist_write_int(NOTEPAD_CHAR_COUNT, pos);
    //persist_write_string(PHONE_NUMBER, p_buffer);
    window_destroy(window);
}

static void window_unload(Window *window) {
    text_layer_destroy(text_layer);
}

static void window_load(Window* window)
{
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    //text_layer = text_layer_create((GRect) { .origin = { 0, 33 }, .size = { bounds.size.w, 20 } });
    phoneNumber = text_layer_create(GRect (30,0,114,25));
    numnum = text_layer_create(GRect (5,0, 25,25));
    text_layer_set_background_color(numnum, GColorWhite);
    //text_layer_set_text(text_layer, "wtf");
    snprintf(numnums, sizeof(numnums), "%s", "#");
    text_layer_set_text(numnum, numnums);
    pbreak = text_layer_create(GRect (0,25,144,2));
    text_layer_set_background_color(pbreak,GColorWhite);
    text_layer_set_background_color(pbreak,GColorBlack);
    text_layer_set_text(phoneNumber,p_buffer);
    //  text_layer_set_text(&textLayer, text_buffer);
//    text_layer_set_background_color(&textLayer, GColorClear);
    custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_Roboto_Regular_20));
    c_font_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_Roboto_Regular_14));
    c_font_18 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_Roboto_Regular_18));
    //text_layer_set_font(text_layer, custom_font);
    text_layer_set_font(phoneNumber, c_font_18);
    text_layer_set_font(numnum, c_font_18);
    layer_add_child(window_layer, text_layer_get_layer(numnum));
    layer_add_child(window_layer, text_layer_get_layer(phoneNumber));
    layer_add_child(window_layer, text_layer_get_layer(pbreak));
    //layer_add_child(window_layer, text_layer_get_layer(text_layer));
    initSidesAndText();
    drawSides();
    change_set(0, true);
    
    // Attach our desired button functionality
//    window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);

}

static void init(void) {
    window = window_create();
    pos = persist_exists(NOTEPAD_CHAR_COUNT) ? persist_read_int(NOTEPAD_CHAR_COUNT) : 0;
    if(pos < 0) {
      pos = 0;
    }
    // app_message_register_inbox_received(inbox_received_handler);
    // app_message_register_inbox_dropped(inbox_dropped_handler);
    app_message_register_outbox_failed(outbox_failed_handler);
    app_message_register_outbox_sent(outbox_sent_handler);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    if (persist_exists(NOTEPAD_TEXT))
        persist_read_string(NOTEPAD_TEXT, text_buffer, pos+1);
    //if(persist_exists(PHONE_NUMBER))
      //persist_read_string(PHONE_NUMBER, p_buffer, sizeof(p_buffer));
    snprintf(p_buffer, sizeof(p_buffer), "%s", "");
    //s_example_bitmap = gbitmap_create_with_resource(RESOURCE_ID_status_bar_icon);
    window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    //window_set_status_bar_icon(window, s_example_bitmap);

    //const bool animated = false;
    window_set_fullscreen(window, true);
    window_stack_push(window, false);
}


int main(void) {
    bbuttons[0] = buttons1;
    bbuttons[1] = buttons2;
    //bbuttons[2] = buttons3;
    init();
//    PebbleAppHandlers handlers = {
//        .init_handler = &handle_init
//    };
  //  change_set(1, true);
 //   next();
//    drawSides();
    drawNotepadText();
    	   text_layer_set_background_color(bbuttons[cur_row][cur_column], GColorBlack);
	   text_layer_set_text_color(bbuttons[cur_row][cur_column], GColorClear);
    app_event_loop();
    deinit();
}
