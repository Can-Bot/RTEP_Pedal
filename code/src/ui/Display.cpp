#include "Display.h"

Display::Display() : cursor(0), semitones(nullptr), semitone_count(0) {  
}

Display::~Display() {    
}

bool Display::init() {
    if(DEV_ModuleInit() != 0) {
        return false;
    }
    SSD1305_begin();
    SSD1305_clear();
    return true;
}

bool Display::isBlackKey(int note) {
    for (int i = 0; i < 14; i++) {
        if (black_keys[i] == note) {
            return true;
        }
    }
    return false;
}

int Display::getWhiteKeyXPosition(int cursor) {
    int cursor_index = -1;
    int zero_index = -1;
    for (int i = 0; i < 21; i++) {
        if (white_keys[i] == cursor) {
            cursor_index = i;
        }
        if (white_keys[i] == 0) {
            zero_index = i;
        }
    }
    if (cursor_index == -1) {
        
        return KEY0_X_POSITION;
    }
    int key_offset = cursor_index - zero_index;
    return KEY0_X_POSITION + (key_offset * 6)+3;
}


int Display::getBlackKeyXPosition(int cursor) {
    for (int i = 0; i < 14; i++) {
        if (black_keys[i] == cursor) {
            return black_key_positions[i];
        }
    }
    return KEY0_X_POSITION;
}


int Display::getKeyXPosition(int cursor) {
    if (isBlackKey(cursor)) {
        return getBlackKeyXPosition(cursor);
    } else {
        return getWhiteKeyXPosition(cursor);
    }
}

void Display::drawHeader(const char* effectName, float effectValue, bool isEnabled) {
    if (isEnabled) {
        for (int i = 2; i < 62; i++) {
            for (int j = 2; j < 2 + 8; j++) {
                SSD1305_pixel(i, j, 1);
            }
        }
        SSD1305_string_4x7(3, 3, effectName, 0);
    } else {
        
        SSD1305_string_4x7(3, 3, effectName, 1);
    }
    char effectValueStr[10];
    snprintf(effectValueStr, sizeof(effectValueStr), "%.2f", effectValue);
    SSD1305_string_4x7(67, 3, effectValueStr, 1);
}

void Display::drawSelection() {
    if (!semitones || semitone_count <= 0) {
        return;
    }
    for (int i = 0; i < semitone_count; i++) {
        int x_pos = getKeyXPosition(semitones[i]);
        if (isBlackKey(semitones[i])) {
            int y_pos = BLACK_KEY_Y_POSITION;
            for (int j = x_pos + 1; j < x_pos + BLACK_KEY_WIDTH - 1; j++) {
                for (int k = y_pos + 1; k < y_pos + BLACK_KEY_HEIGHT - 1; k++) {
                    SSD1305_pixel(j, k, 1); 
                }
            }
        } else {
            
            int y_pos = WHITE_KEY_Y_POSITION;
            for (int j = x_pos + 1; j < x_pos + WHITE_KEY_WIDTH - 2; j++) {
                for (int k = y_pos + 10; k < y_pos + WHITE_KEY_HEIGHT - 2; k++) {
                    SSD1305_pixel(j, k, 0); 
                }
            }
        }
    }
}

void Display::drawCursor() {
    
    bool is_black = isBlackKey(cursor);
    char cursor_x = getKeyXPosition(cursor);
    char cursor_y, cursor_width, cursor_height;
    if (is_black) {
        cursor_y = BLACK_KEY_Y_POSITION;
        cursor_width = BLACK_KEY_WIDTH;
        cursor_height = BLACK_KEY_HEIGHT;
    } else {  
        cursor_y = WHITE_KEY_Y_POSITION;
        cursor_width = WHITE_KEY_WIDTH;
        cursor_height = WHITE_KEY_HEIGHT;
    }
    if (is_black && semitones) {
        for (int k = 0; k < semitone_count; k++) { 
            if (semitones[k] == cursor) {
                for (int i = cursor_x; i < cursor_x + cursor_width; i++) {
                    for (int j = cursor_y - 1; j < cursor_y + 1; j++) {    
                        if ((i + j) % 2 == 0) {
                            SSD1305_pixel(i, j, 1);  
                        } else {
                            SSD1305_pixel(i, j, 0);  
                        }
                    }
                }
                return;  
            }
        }
    } 
    for (int i = cursor_x; i < cursor_x + cursor_width; i++) {
        for (int j = cursor_y; j < cursor_y + cursor_height; j++) {        
            if (is_black) {
                if (i == cursor_x || i == cursor_x + cursor_width - 1 || j == cursor_y + cursor_height - 1) {   
                    continue;
                }
                if ((i + j) % 2 == 0) {
                    SSD1305_pixel(i, j, 1);  
                } else {
                    SSD1305_pixel(i, j, 0);  
                }
            } else {
                if ((i + j) % 2 == 0) {
                    SSD1305_pixel(i, j, 0);  
                }
            }
        }
    }
}

void Display::update(const char* effectName, float effectValue, bool isEnabled) {
    SSD1305_clear();
    SSD1305_bitmap(0, 0, piano_screen, 128, 32, 1);
    drawCursor();
    drawSelection();
    drawHeader(effectName, effectValue, isEnabled);
    SSD1305_display();
}


void Display::setCursor(int note) {
    cursor = note;
}

void Display::setSelectedNotes(int* notes, int count) {
    semitones = notes;
    semitone_count = count;
}
