#ifndef UIHANDLER_H
#define UIHANDLER_H

#include "Display.h"
#include "../options/Config.h"
#include "../gpio_event/gpioevent.h"
#include "DigitalSignalChain.h"
#include <string>
#include <vector>
#include <array>
#include <map>

class EncoderHandler;

/**
 * @class UIHandler
 * @brief Controls the user interface of the system. Handles encoder input, updates the
 * effect and harmonizer configuration and manages updates to the display. 
 */
class UIHandler{
public:
    /** @brief Get the singleton instance */
    static UIHandler& getInstance();

    /** 
     * @brief Initialize UI
     * @param dspChain Reference to the DSP chain for effect control
     * @return true if successful
     */
    bool init(DigitalSignalChain &dspChain);
    
    /** @brief Update display with current state */
    void update();

    /** 
     * @brief Process encoder events
     * @param encoderID Encoder identifier
     * @param action Turn or push
     */
    void handleEncoder(int encoderID, int action);
    
    /** @brief Possible encoder actions */
    enum EncoderAction {
        ACTION_LEFT = 0,
        ACTION_RIGHT = 1,
        ACTION_PUSH = 2
    };
    /** @brief Encoder Identifiers */
    enum EncoderID {
        ENC_CURSOR = 0,
        ENC_EFFECT_SELECT = 1,    
        ENC_EFFECT_EDIT = 2,     

    };

private:
    DigitalSignalChain *dspChain;
    UIHandler();
    ~UIHandler();
    
    UIHandler(const UIHandler&) = delete;
    UIHandler& operator=(const UIHandler&) = delete;

    /** @brief Piano keyboard range limits */
    static constexpr int MIN_CURSOR_VALUE = -17;
    static constexpr int MAX_CURSOR_VALUE = 17;

    /**
     * @struct EffectParam
     * @brief Defines available effect parameters
     *
     * Stores the current state, range and name for any effect parameter.
     */
    struct EffectParam {
        std::string name;        
        std::string configKey;   
        bool isEnabled;         
        
        float minValue;         
        float maxValue;         
        float stepSize;          
        float currentValue;
        
        static const int MAX_SEMITONES = 8;
        std::array<int, MAX_SEMITONES> semitones;
        int semitoneCount;
        
        /** @brief Parameter data types */
        enum ParamType { TYPE_FLOAT, TYPE_INT, TYPE_SEMITONES } type;
        
        /**
         * @brief Constructor for custom resolution parameters
         * @param n Effect name
         * @param key Key in config.cfg
         * @param t Parameter type
         * @param min Minimum value
         * @param max Maximum value
         * @param step Step size
         */
        EffectParam(const std::string& n, const std::string& key, 
                    ParamType t, float min, float max, float step) 
            : name(n), configKey(key), isEnabled(false), 
            minValue(min), maxValue(max), stepSize(step), 
            currentValue(0), semitoneCount(0), type(t) {}
        
        /**
         * @brief Constructor for 8-bit resolution parameters
         * @param n Effect name
         * @param key Config key
         * @param t Parameter type
         * @param min Minimum value
         * @param max Maximum value
         */
        EffectParam(const std::string& n, const std::string& key, 
                    ParamType t, float min, float max) 
                : name(n), configKey(key), isEnabled(false), 
                minValue(min), maxValue(max), 
                stepSize((max - min) / 255.0f), 
                currentValue(0), semitoneCount(0), type(t) {}

    };
    
    /** 
     * @brief Handles input from the cursor encoder
     * @param action The encoder action (left/right/push)
     */
    void handleCursorEncoder(EncoderAction action);

    /** 
     * @brief Handles input from the effect selection encoder
     * @param action The encoder action (left/right/push)
     */
    void handleEffectSelectEncoder(EncoderAction action);

    /** 
     * @brief Handles input from the effect parameter encoder
     * @param action The encoder action (left/right/push)
     */
    void handleEffectEditEncoder(EncoderAction action);
    
    /** 
     * @brief Checks if a semitone is currently selected
     * @param note The semitone to check
     * @return true if the semitone is selected
     */
    bool isSemitoneSelected(int note) const;

    /** 
     * @brief Toggles semitone selection on or off
     * @param note The semitone value to toggle
     */
    void toggleSemitone(int note);
    
    /** 
     * @brief Saves current effect parameters to config in memory
     */
    void updateConfig();

    /** 
     * @brief Loads effect parameters from config file
     */
    void loadFromConfig();
    
    /** 
     * @brief Converts semitone array to string representation
     * @param semitones Array of semitone values
     * @param count Current semitone count
     * @return String representation of semitones
     */
    std::string semitonesToString(const std::array<int, EffectParam::MAX_SEMITONES>& semitones, int count);

    /** 
     * @brief Parses string representation into semitone array
     * @param str String for parsing
     * @param semitones Array of semitone values
     * @param count Current semitone count
     */
    void parseSemitonesString(const std::string& str, std::array<int, EffectParam::MAX_SEMITONES>& semitones, int& count);

    /** @brief OLED display controller instance */
    Display display;
    
    /** @brief Reference to global configuration */
    Config& config;
    
    /** @brief Current position of cursor on piano keyboard */
    int cursorPosition;
    
    /** @brief List of available effects and their parameters */
    std::vector<EffectParam> effects;
    
    /** @brief Index of currently selected effect */
    int currentEffectIndex;
    
};

#endif // UIHANDLER_H