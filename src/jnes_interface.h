#ifndef JNES_INTERFACE_H
#define JNES_INTERFACE_H

#include <string>
#include <fstream>
#include <windows.h>
#include <iostream>

class Joystick_state
{
    public:
    uint8_t bin_state;
    
    Joystick_state();
    Joystick_state(uint8_t bin_state);
};

class Jnes_Interface
{
    public:
    Jnes_Interface(std::string jnes_path,std::string jnes_rom);
    
    void init();
    void update_joystick(Joystick_state state);
    
    int update_screen_data();
    
    uint8_t *screendata;
    
    //private:
    std::string jnes_path;
    std::string jnes_rom;
    Joystick_state current_joy_state;
    int reset_config();
    
    DWORD button_key[8];
    void KeyboardButtonDown(DWORD a);
    void KeyboardButtonUp(DWORD a);
};

#endif