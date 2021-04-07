#include "jnes_interface.h"

Joystick_state::Joystick_state()
{
    bin_state = 0;
}

Joystick_state::Joystick_state(uint8_t bin_state)
{
    this->bin_state = bin_state;
}
    
Jnes_Interface::Jnes_Interface(std::string jnes_path,std::string jnes_rom)
    : jnes_path(jnes_path), jnes_rom(jnes_rom)
{
    current_joy_state = Joystick_state();
    screendata = 0;
    
    //ideal button delay to avoid more than one controller update per frame. (1000/60)*2
    //used when updateing joystick state
    button_delay = 34;
}

void Jnes_Interface::init()
{
    button_key[7] = 'W';
    button_key[6] = 'S';
    button_key[5] = 'A';
    button_key[4] = 'D';
    button_key[3] = 'Z';
    button_key[2] = 'C';
    button_key[1] = 'Q';
    button_key[0] = 'E';
    
    if(reset_config() != 0)
    {
        return;
    }
    
    std::string command = "START " + jnes_path + "Jnes.exe \"" +  jnes_rom + "\"";
    system(command.c_str());

    return;
}

int Jnes_Interface::reset_config()
{
    std::string config_file_name = jnes_path + "Jnes.ini";
    std::ofstream config_file;
    config_file.open(config_file_name);
    
    if(config_file.is_open() == false)
    {
        std::cout << "Could not open Jnes config file" << std::endl;
        return -1;
    }
    
    config_file <<"[Position]"                                                                                   << std::endl;
    config_file <<"Main Top=110"                                                                                 << std::endl;
    config_file <<"Main Left=222"                                                                                << std::endl;
    config_file <<"Main Bottom=416"                                                                              << std::endl;
    config_file <<"Main Right=494"                                                                               << std::endl;
    config_file <<"Browser Top=110"                                                                              << std::endl;
    config_file <<"Browser Left=222"                                                                             << std::endl;
    config_file <<"Browser Bottom=416"                                                                           << std::endl;
    config_file <<"Browser Right=494"                                                                            << std::endl;
    config_file <<"[Jnes]"                                                                                       << std::endl;
    config_file <<"UpdateTime=A070F55F0000000064"                                                                << std::endl;
    config_file <<"Input1=1E00200011001F002C002E0012001000020003000100000000000000000000000000000000000000F0"    << std::endl;
    config_file <<"Input2=0000000000000000000000000000000000000000000000000000000000000000000000000000000000"    << std::endl;
    config_file <<"options2=9004000296"                                                                          << std::endl;
    config_file <<"Language="                                                                                    << std::endl;
    config_file <<"[FullScreen]"                                                                                 << std::endl;
    config_file <<"width=1920"                                                                                   << std::endl;
    config_file <<"height=1080"                                                                                  << std::endl;
    config_file <<"[Directory]"                                                                                  << std::endl;
    config_file <<"Saves=.\\saves"                                                                               << std::endl;
    config_file <<"Rom="                                                                                         << std::endl;
    config_file <<"Last=..\\emulator"                                                                            << std::endl;
    config_file <<"Screenshots=.\\screenshots"                                                                   << std::endl;
    config_file <<"Browser=..\\emulator\\Jnes"                                                                   << std::endl;
    config_file <<"[Recent Files]"                                                                               << std::endl;
    config_file <<"Entry0=..\\emulator\\Tetris (U) [!].zip"                                                      << std::endl;
    config_file <<"[Recent Dirs]"                                                                                << std::endl;
    config_file <<"Entry0=..\\emulator"                                                                          << std::endl;

    config_file.close();
    
    return 0;
}

void Jnes_Interface::update_joystick(Joystick_state state)
{
    for(int i = 8;i>=0;i--)
    {
        bool current_button_state = current_joy_state.bin_state & (1<<i);
        bool next_button_state = state.bin_state & (1<<i);
        
        if(current_button_state != next_button_state)
        {
            if(next_button_state)
            {
                KeyboardButtonDown(button_key[i]);
            }
            else
            {
                KeyboardButtonUp(button_key[i]);
            }
        }
    }
    
    current_joy_state.bin_state = state.bin_state;
    Sleep(button_delay);
}

void Jnes_Interface::KeyboardButtonDown(DWORD a)
{
    INPUT Input = { 0 };
    // Set up a generic keyboard event.
    Input.type       = INPUT_KEYBOARD;
    Input.ki.wScan   = MapVirtualKey(a, MAPVK_VK_TO_VSC);
    Input.ki.dwFlags = 0;
    Input.ki.wVk   = 0; 
    SendInput( 1, &Input, sizeof( INPUT ) );
}

void Jnes_Interface::KeyboardButtonUp(DWORD a)
{
    INPUT Input = { 0 };
    Input.type       = INPUT_KEYBOARD;
    Input.ki.wScan   = MapVirtualKey(a, MAPVK_VK_TO_VSC);
    Input.ki.dwFlags = KEYEVENTF_KEYUP;
    Input.ki.wVk   = 0;
    SendInput( 1, &Input, sizeof( INPUT ) );
}

int Jnes_Interface::update_screen_data()
{
    HWND activewindow = GetForegroundWindow();
    
    //verify window name
    char window_title[256];
    GetWindowText(activewindow,window_title,sizeof(window_title));
    if(std::string(window_title).find("Jnes") == std::string::npos)
    {
        std::cerr << "Active window does not seem to be correct" << std::endl;
        return -1; //error
    }
    
    //get and verify window dimensions
    RECT rect;
    GetWindowRect(activewindow,&rect);
    
    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;
    
    if(window_width != 262)
    {
        std::cerr << "Active window dimensions are not correct" << std::endl;
        return -1; //error
    }
    
    //get image data https://www.codeproject.com/Articles/5051/Various-methods-for-capturing-the-screen
    HDC window_dc = GetDC(activewindow);
    HDC capture_dc = CreateCompatibleDC(window_dc);
    HBITMAP capture_bitmap = CreateCompatibleBitmap(window_dc,window_width,window_height);
    SelectObject(capture_dc,capture_bitmap);
    BitBlt(capture_dc,0,0,window_width,window_height,window_dc,0,0,SRCCOPY); 
    
    if(screendata)
    {
        free(screendata);
    }
    screendata = (uint8_t*)malloc(4 * window_width * window_height);
    
    BITMAPINFOHEADER bmi = {0};
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biWidth = window_width;
    bmi.biHeight = -window_height;
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = 0;
    
    GetDIBits(capture_dc, capture_bitmap, 0, window_height, screendata, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
    
    ReleaseDC(NULL, window_dc);
    DeleteDC(capture_dc);
    DeleteObject(capture_bitmap);
    
    return 0;
}