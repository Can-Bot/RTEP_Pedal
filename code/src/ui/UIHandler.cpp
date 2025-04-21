#include "UIHandler.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

UIHandler& UIHandler::getInstance() {
    static UIHandler instance;
    return instance;
}

UIHandler::UIHandler() 
    : config(Config::getInstance()),
      cursorPosition(0),
      currentEffectIndex(1) {  
    
    effects.emplace_back("Harmonizer", "harmonizer", EffectParam::TYPE_SEMITONES, 0.0f, 0.0f);
    effects.emplace_back("Fuzz", "fuzz", EffectParam::TYPE_FLOAT, 0.0f, 1.0f);
    effects.emplace_back("Gain", "gain", EffectParam::TYPE_FLOAT, 0.0f, 200.0f);
    
}

UIHandler::~UIHandler() {
}

bool UIHandler::init(DigitalSignalChain &dspChain) {
    this->dspChain = &dspChain;
    if (!display.init()) {
        std::cerr << "Failed to initialize display" << std::endl;
        return false;
    }
    loadFromConfig();
    update();
    for (int i = 1; i < 9; i++) {
        toggleSemitone(i);
        update();
    }
    return true;
}

void UIHandler::update() {
    display.setCursor(cursorPosition);       
    if (effects[0].isEnabled && effects[0].semitoneCount > 0) {
        for (int i = 0; i < effects[0].semitoneCount; i++) {
            std::cout << effects[0].semitones[i] << " ";
        }   
        display.setSelectedNotes(effects[0].semitones.data(), effects[0].semitoneCount);
    } else {
        display.setSelectedNotes(nullptr, 0);
    }
    const auto& effect = effects[currentEffectIndex];
    display.update(effect.name.c_str(), effect.currentValue, effect.isEnabled);
}

void UIHandler::handleEncoder(int encoderID, int action) {
    switch (encoderID) {
        case ENC_CURSOR:
            handleCursorEncoder(static_cast<EncoderAction>(action));
            break;
            
        case ENC_EFFECT_SELECT:
            handleEffectSelectEncoder(static_cast<EncoderAction>(action));
            break;
            
        case ENC_EFFECT_EDIT:
            handleEffectEditEncoder(static_cast<EncoderAction>(action));
            break;
            
        default:
            return;
    }
    
    updateConfig();
    update();
}

void UIHandler::handleCursorEncoder(EncoderAction action) {
    if (action == ACTION_LEFT) {
        cursorPosition--;
        if (cursorPosition < MIN_CURSOR_VALUE) {
            cursorPosition = MAX_CURSOR_VALUE;
        }
    } 
    else if (action == ACTION_RIGHT) {
        cursorPosition++;
        if (cursorPosition > MAX_CURSOR_VALUE) {
            cursorPosition = MIN_CURSOR_VALUE; 
        }
    } 
    else if (action == ACTION_PUSH) {

        if (effects[0].isEnabled) {
            toggleSemitone(cursorPosition);
        }
    }
}

void UIHandler::handleEffectSelectEncoder(EncoderAction action) {   
    if (action == ACTION_LEFT || action == ACTION_RIGHT) {
        if (currentEffectIndex == 1) {
            currentEffectIndex = 2; 
        } else {
            currentEffectIndex = 1;
        }
    } 
    else if (action == ACTION_PUSH) {
        effects[currentEffectIndex].isEnabled = !effects[currentEffectIndex].isEnabled;
    }
}

void UIHandler::handleEffectEditEncoder(EncoderAction action) {
    if (currentEffectIndex < 1 || currentEffectIndex >= static_cast<int>(effects.size())) {
        return;
    }
    auto& effect = effects[currentEffectIndex];
    if (!effect.isEnabled) {
        return;
    }
    if (action == ACTION_LEFT || action == ACTION_RIGHT) {
        float delta = (action == ACTION_LEFT) ? -effect.stepSize : effect.stepSize;
        effect.currentValue += delta;
        if (effect.currentValue < effect.minValue) effect.currentValue = effect.minValue;
        if (effect.currentValue > effect.maxValue) effect.currentValue = effect.maxValue;
        if (effect.type == EffectParam::TYPE_INT) {
            effect.currentValue = std::round(effect.currentValue);
        }
    }
    else if (action == ACTION_PUSH) {
        // Could implement a "reset to default" feature here
        // For now, does nothing
    }
}

bool UIHandler::isSemitoneSelected(int note) const {
    const auto& semitones = effects[0].semitones;
    int count = effects[0].semitoneCount;
    for (int i = 0; i < count; i++) {
        if (semitones[i] == note) {
            return true;
        }
    }
    return false;
}

void UIHandler::toggleSemitone(int note) {
    auto& effect = effects[0];
    for (int i = 0; i < effect.semitoneCount; i++) {
        if (effect.semitones[i] == note) {
            for (int j = i; j < effect.semitoneCount - 1; j++) {
                effect.semitones[j] = effect.semitones[j + 1];
            }
            effect.semitoneCount--;
            return;
        }
    }
    if (effect.semitoneCount < EffectParam::MAX_SEMITONES) {
        effect.semitones[effect.semitoneCount++] = note;
    }
}

void UIHandler::updateConfig() {
    for (const auto& effect : effects) {
        if (effect.type == EffectParam::TYPE_SEMITONES) {
            std::string semitonesStr = semitonesToString(effect.semitones, effect.semitoneCount);
            config.set(effect.configKey, effect.isEnabled, semitonesStr);
            this->dspChain->configureEffects(config);
        } else {
            config.set(effect.configKey, effect.isEnabled, effect.currentValue);
        }
    }
}

void UIHandler::loadFromConfig() { 
    for (auto& effect : effects) {
        bool isEnabled = false; 
        if (config.contains(effect.configKey)) {
            isEnabled = true;      
            if (effect.type == EffectParam::TYPE_SEMITONES) {
                std::string rawValue = config.get<std::string>(effect.configKey, "");
                parseSemitonesString(rawValue, effect.semitones, effect.semitoneCount);
                for (int i = 0; i < effect.semitoneCount; i++) {
                    std::cout << effect.semitones[i] << " ";
                }
                std::cout << std::endl;
            }
            else if (effect.type == EffectParam::TYPE_FLOAT || effect.type == EffectParam::TYPE_INT) {
                // For numeric values, use a consistent approach
                float defaultValue = (effect.minValue + effect.maxValue) / 2.0f;
                float value = config.get<float>(effect.configKey, defaultValue);            
                effect.currentValue = value;
            }
        } 
        else {
            isEnabled = false;
            if (effect.type == EffectParam::TYPE_SEMITONES) {
                effect.semitoneCount = 0;
            } else {
                float defaultValue = (effect.minValue + effect.maxValue) / 2.0f;
                effect.currentValue = defaultValue;
            }
        }
        
        effect.isEnabled = isEnabled;
    }
    
    for (auto& effect : effects) {
        if (effect.type == EffectParam::TYPE_SEMITONES) {
            effect.stepSize = 1.0f;
        }
        else if (effect.type == EffectParam::TYPE_FLOAT) {

            if (effect.name == "Fuzz") {
                effect.stepSize = 0.01f;
            }
            else if (effect.name == "Gain") {
                effect.stepSize = 5.0f; 
            }
            else {
                effect.stepSize = (effect.maxValue - effect.minValue) / 40.0f;
            }
        }
        else if (effect.type == EffectParam::TYPE_INT) {
            effect.stepSize = 1.0f;
        }
    }
}

std::string UIHandler::semitonesToString(const std::array<int, EffectParam::MAX_SEMITONES>& semitones, int count) {
    std::stringstream ss;
    for (int i = 0; i < count; i++) {
        ss << semitones[i];
        if (i < count - 1) {
            ss << " ";
        }
    }
    return ss.str();
}

void UIHandler::parseSemitonesString(const std::string& str, std::array<int, EffectParam::MAX_SEMITONES>& semitones, int& count) {
    std::stringstream ss(str);
    count = 0;
    int value;
    while (ss >> value && count < EffectParam::MAX_SEMITONES) {
        semitones[count++] = value;
    }
}