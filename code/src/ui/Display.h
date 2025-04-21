#ifndef DISPLAY_H
#define DISPLAY_H

#include "SSD1305.h"
#include "DEV_Config.h"

/** @brief Piano keyboard layout constants */
#define KEY0_X_POSITION 59      ///< X position of the central C key
#define WHITE_KEY_Y_POSITION 12 ///< Y position of white keys
#define WHITE_KEY_WIDTH 6       ///< Width of white keys in pixels
#define WHITE_KEY_HEIGHT 19     ///< Height of white keys in pixels
#define BLACK_KEY_Y_POSITION 12 ///< Y position of black keys
#define BLACK_KEY_WIDTH 4       ///< Width of black keys in pixels
#define BLACK_KEY_HEIGHT 12     ///< Height of black keys in pixels

/** @brief Forward declare piano keyboard bitmap */
extern const unsigned char piano_screen[];

/**
 * @class Display
 * @brief Controls the OLED display output
 *
 * Manages the display of::
 * - Piano keyboard visualization
 * - Effect parameters and status
 * - Cursor position
 * - Selected notes
 */
class Display {
private:
    /** @brief Piano keyboard mapping for lookup */
    static constexpr int white_keys[21] = {-17, -15, -13, -12, -10, -8, -7, -5, -3, -1, 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17};
    static constexpr int black_keys[14] = {-16, -14, -11, -9, -6, -4, -2, 1, 3, 6, 8, 10, 13, 15};
    static constexpr int black_key_positions[14] = {5, 12, 23, 30, 41, 47, 54, 65, 72, 83, 89, 96, 107, 114};
    
    /** @brief Current display state */
    int cursor;
    int* semitones;
    int semitone_count;

    /**
     * @brief Check if a is a black key
     * @param note The note to check
     * @return true if black key
     */
    bool isBlackKey(int note);

    /**
     * @brief Get X coordinate for white key
     * @param cursor Note value
     * @return X coordinate in pixels
     */
    int getWhiteKeyXPosition(int cursor);

    /**
     * @brief Get X coordinate for black key
     * @param cursor Note value
     * @return X coordinate in pixels
     */
    int getBlackKeyXPosition(int cursor);

    /**
     * @brief Get X coordinate for any key
     * @param cursor Note value
     * @return X coordinate in pixels
     */
    int getKeyXPosition(int cursor);
    
    /**
     * @brief Draw display header
     * @param effectName Name of current effect
     * @param effectValue Current effect value
     * @param isEnabled Effect enabled state
     */
    void drawHeader(const char* effectName, float effectValue, bool isEnabled);
    
    /** @brief Draw selected harmony notes */
    void drawSelection();

    /** @brief Draw cursor at current position */
    void drawCursor();

public:
    /** @brief Constructor */
    Display();

    /** @brief Destructor */
    ~Display();
    
    /**
     * @brief Initialize display hardware
     * @return true if initialization successful
     */
    bool init();
    
    /**
     * @brief Set cursor position
     * @param note New cursor position to set
     */

    void setCursor(int note);
    
    /**
     * @brief Set selected harmony notes
     * @param notes Array of selected notes
     * @param count Number of selected notes
     */
    void setSelectedNotes(int* notes, int count);
    
    /**
     * @brief Update display with current state
     * @param effectName Name of current effect
     * @param effectValue Current parameter value
     * @param isEnabled Effect enabled state
     */
    void update(const char* effectName, float effectValue, bool isEnabled);
};

#endif // DISPLAY_H