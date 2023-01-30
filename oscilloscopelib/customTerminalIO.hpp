#ifndef customTerminalIO_HPP
#define customTerminalIO_HPP

//got the windows code from : https://stackoverflow.com/questions/23369503/get-size-of-terminal-window-rows-columns
#if defined(_WIN32)
    #error THE WINDOWS VERSION OF THIS LIBRARY IS STILL IN DEVELOPMENT

    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    
    #include <Windows.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>
#endif

#define ENDLINE        "\r\n"

#define CTRL_KEY(in) ((in) & 0x1f)  //This code bitwise-ANDs the first 3 bits of "in" to 0 (being 0x1f = 00011111) to check if a key has been pressed alongside as CTRL
                                    //Got this line from https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html

//got all these codes from : https://en.wikipedia.org/wiki/ANSI_escape_code
//Text types:
#define RESET               0
#define BOLD                1
#define FAINT               2
#define ITALIC              3
#define UNDERLINED          4
#define SLOW_BLINK          5
#define FAST_BLINK          6
#define INVERT              7
#define CONCEAL             8
#define STRIKE              9
#define DEFAULT_FONT        10
#define ALTERNATIVE_FONT    11
#define DOUBLE_UNDERLINE    21
#define NORMAL_INTENSITY    22
#define NO_UNDERLINE        24
#define NO_BLINK            25
#define NO_INVERT           27
#define NO_CONCEAL          28
#define NO_STRIKE           29
#define DEFAULT_FCOLOR      39
#define DEFAULT_BCOLOR      49
#define OVERLINED           53
#define FRAMED              51
#define ENCIRCLED           52
#define NO_FRAMED           54
#define NO_ENCIRCLED        54
#define NO_OVERLINED        55

//Default colors: (RGB is supported too)
#define BLACK               30
#define RED                 31
#define GREEN               32
#define YELLOW              33
#define BLUE                34
#define MAGENTA             35
#define CYAN                36
#define WHITE               37
#define GRAY                90
#define BRIGHT_RED          91
#define BRIGHT_GREEN        92
#define BRIGHT_YELLOW       93
#define BRIGHT_BLUE         94
#define BRIGHT_MAGENTA      95
#define BRIGHT_CYAN         96
#define BRIGHT_WHITE        97

//Defined direct input keys:
#define KEY_UP             -1
#define KEY_DOWN           -2
#define KEY_LEFT           -3
#define KEY_RIGHT          -4
#define KEY_STARTL         -5
#define KEY_ENDL           -6
#define KEY_PGUP           -7
#define KEY_PGDOWN         -8
#define KEY_INSERT         -9
#define KEY_DELETE         -10
#define KEY_CENTER         -11
#define KEY_ESCAPE         -12
#define KEY_F1             -13
#define KEY_F2             -14
#define KEY_F3             -15
#define KEY_F4             -16
#define KEY_F5             -17
#define KEY_F6             -18
#define KEY_F7             -19
#define KEY_F8             -20
#define KEY_F9             -21
#define KEY_F10            -22
#define KEY_F11            -23
#define KEY_F12            -24
#define KEY_PAUSE          -25

//Other keys for direct-input:
#define BACKSPACE           127
#define ENTER               13
#define TAB                 9
#define SPACE               32

//Main terminal namespace which contains all the functions of the header
namespace terminal{
    //This function works on linux and on windows
    //got the windows code from : https://stackoverflow.com/questions/23369503/get-size-of-terminal-window-rows-columns

    //This function gets and returns the number of rows of the terminal
    int rows(){
        #if defined(_WIN32)
            CONSOLE_SCREEN_BUFFER_INFO CSBI;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CSBI);
            return (int)(csbi.srWindow.Right-csbi.srWindow.Left+1);
        #elif defined(__linux__)
            //Explanation for this code from https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html

            struct winsize win; //Calling the winsize struct from <sys/ioctl.h> as win
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &win); //using the ioctl command to get the attributes of STDOUT_FILENO and write them into the win (winsize) struct
            return win.ws_row; //return the ws_row (n of rows of terminal) attribute from the win struct
        #endif
    } //int terminal::rows()

    //This function gets and returns the number of columns of the terminal
    int cols(){
        //This function works on linux and on windows
        //got the windows code from : https://stackoverflow.com/questions/23369503/get-size-of-terminal-window-rows-columns

        #if defined(_WIN32)
            CONSOLE_SCREEN_BUFFER_INFO CSBI;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CSBI);
            return (int)(csbi.srWindow.Bottom-csbi.srWindow.Top+1);
        #elif defined(__linux__)
            //Explanation for this code from https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html

            struct winsize win; //Calling the winsize struct from <sys/ioctl.h> as win
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &win); //using the ioctl command to get the attributes of STDOUT_FILENO and write them into the win (winsize) struct
            return win.ws_col; //return the ws_col (n of columns of terminal) attribute from the win struct
        #endif
    }

    //This namespace contains all the functions to read input from the terminal
    //Here the functions are just declared, go to the end of the file to see the actual code.
    namespace in{
        //This function gets a single character from the user and the way it does that can be customized
        extern char get_ch(bool waitForInput = true/*Set to false for the function to not wait for user input*/, bool echo = false/*set to true to show the user the characters printed*/, bool rawMode = true/*Set to false for the rawMode not to be enable (not recommended)*/, bool eSC = true/*set to false to disable special characters (KEY_UP, KEY_DOWN, KEY_F1, KEY_ESCAPE,...)*/, bool eSI = true/*Set to false to disable signal interrupt during input (Ctrl+C / Ctrl+Z)*/);

        //These three functions are used to get a string input from the user and store it in a char array
        //Usage : use the get_str command to get the string from the user, then declare a new char array with the just-filled str_length variable and use store_str(arrayName) to store the input into the array.
        unsigned long str_length = 0;
        extern void get_str(bool echo = true/*read get_ch()*/, bool eSI = true/*read get_ch()*/, bool enterBreaks = true/*set to false to disable the ENTER key terminating input*/, char endChar = '\0'/*set to anything else than 0 to enable a custom char to end the string*/, unsigned int maxLength = 0/*set to anything else than 0 to enable a maximum input length*/);
        extern void store_str(char output[]);
    } //namespace terminal::in;

    //got all the ANSI escape codes from : https://en.wikipedia.org/wiki/ANSI_escape_code

    //This namespace contains all the functions to write output to the terminal
    //Here the functions are just declared, go to the end of the file to see the actual code.
    namespace out{
        //Declared a variadic template to create variadic functions where it can be inputted every type of variable and it will be accepted
        template <typename... args>
        extern void sprint(args... text);   //Used to print a sequence of characters to the terminal without the automatic ENDLINE at the end
                                            //This function will NOT convert integers to char arrays printing them out to the terminal

        template <typename... args>
        extern void sprintln(args... text); //Used to print a sequence of characters to the terminal with the automatic ENDLINE at the end
                                            //This function will NOT convert integers to char arrays printing them out to the terminal

        template <typename... args>
        extern void print(args... text);    //Used to print a sequence of characters to the terminal without the automatic ENDLINE at the end

        template <typename... args>
        extern void println(args... text);  //Used to print a sequence of characters to the terminal with the automatic ENDLINE at the end

        unsigned int print_precision = 6; //When printing a decimal number this integer determines the precision to use when printing it

        extern void printch(char input); //Used to print only a single character to the terminal and accepts only a single char input
        
        //Changes the color of the text written after this command is enabled, accepts either RGB input or normal predefined colors (written as preprocessor code at the start of this header)
        void set_color(int foreground, int background = BLACK, short rf = -1, short gf = -1, short bf = -1, short rb = -1, short gb = -1, short bb = -1){if(rf < 0 || gf < 0 || bf < 0 || rb < 0 || gb < 0 || bb < 0)terminal::out::print("\e[", foreground, ";", background + 10, "m"); else terminal::out::print("\e[38;2;", rf, ";", gf, ";", bf, "\e[48;2;", rb, ";", gb, ";", bb, "m");}
        void set_attribute(int attribute){terminal::out::print("\e[", attribute, "m");} //Enables a certain terminal attribute in the defined ones at the preprocessor code at the start of this header
    } //Namespace terminal::out;

    //This namespace contains all the functions used to manage the cursor on the terminal
    //Here the functions are just declared, go to the end of the file to see the actual code.
    namespace cur{
        void move_up(int n = 1){terminal::out::print("\e[", n, "A");} //Moves the cursor up n times
        void move_down(int n = 1){terminal::out::print("\e[", n, "B");} //Moves the cursor down n times
        void move_forward(int n = 1){terminal::out::print("\e[", n, "C");} //Moves the cursor forward n times
        void move_back(int n = 1){terminal::out::print("\e[", n, "D");} //Moves the cursor back n times

        void set_position(unsigned int x, unsigned int y){terminal::out::print("\e[", x, ";", y, "H");} //Sets a specific cursor position defined by an x and y axis

        void show(){terminal::out::sprint("\e[?25h");} //Shows the cursor (if hidden)
        void hide(){terminal::out::sprint("\e[?25l");} //Hides the cursor (if shown)
    } //Namespace cur;

    //This namespace contains all the functions used to manage the terminal's entire screen
    //Here the functions are just declared, go to the end of the file to see the actual code.
    namespace scr{
        void clear(unsigned short n = 2){terminal::out::print("\e[", n, "J");} //Clears the screen
        void init_alt(){terminal::out::sprint("\e[?1049h");} //Starts an alternate screen for the terminal
        void end_alt(){terminal::out::sprint("\e[?1049l");} //Exits from the alternate terminal screen
        void reset(){terminal::out::sprint("\ec");} //Resets and clears the terminal

        void scroll_up(int n = 1){terminal::out::print("\e[", n, "S");} //Scrolls up on the terminal
        void scroll_down(int n = 1){terminal::out::print("\e[", n, "T");} //Scrolls down on the terminal
    } //Namespace scr;

    //This namespace contains all internal functions which are only used in this header
    namespace internal{
        char *input;

        //Namespace used to manage terminal's rawmode
        namespace rawMode{
            static struct termios raw, noRaw; //Calls the termios struct (Containted in <termios.h> library) as raw and noRaw (to be used later)

            void disable(){
                #if defined(_WIN32)
                    //I'm still trying to write a windows version of this program but i still cannot find enough documentation to do this stuff on <windows.h>
                #elif defined(__linux__)
                    //wrote this code using : https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html

                    tcsetattr(STDIN_FILENO, TCSANOW, &noRaw);   //Applying to the terminal the original attributes to exit raw mode
                                                                //TCSANOW is used to apply the attributes immediately after sending the command

                    return;
                #endif
            }//terminal::internal::rawMode::disable();

            void enable(bool echoOn = true, bool SIGSTPINT = true){
                #if defined(_WIN32)

                #elif defined(__linux__)
                    //wrote this code using : https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html

                    tcgetattr(STDIN_FILENO, &raw);      //Reads the STDIN_FILENO (terminal) attributes into the raw (termios) struct
                    tcgetattr(STDIN_FILENO, &noRaw);    //Reads the current terminal attributes into noRaw to be restored later

                    if(!echoOn)raw.c_lflag &= ~(ECHO);                          //c_lflag is for the "local flags" field, if ECHO was requested to be on we turn it on
                                                                                //ECHO is a bitflag, used to print the characters pressed by the user in the terminal, defined as 00000000000000000000000000001000, NOT (~) operator is being used to invert it (11111111111111111111111111110111) and bitwise-AND (&=) it into the flag causing the 29th bit to flip to 0, thus setting the ECHO to OFF

                    if(!SIGSTPINT)raw.c_lflag &= ~(ISIG);                       //ISIG is another bitflag used to disable the CTRL+C (terminate) and CTRL+Z (suspend) sequences
                                                                                //It's another local flag and if it was requested to be off then turn it off.
                                                                
                    raw.c_lflag &= ~(ICANON | IEXTEN);                          //ICANON is used to exit from terminal's canonical (or cooked) mode and entering raw mode to read input byte by byte instead of line by line
                                                                                //IEXTEN is yet another bitflag used to turn off the CTRL-V sequence, used to send bytes directly to the terminal.

                    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);   //c_iflag is for the input flags
                                                                                //IXON Comes from <termios.h> and activates the software flow control using the CTRL+S and CTRL+Q commands, we bitwise-NOT (~) and bitwise-AND (&=) it into the input flags to turn it off
                                                                                //ICRNL is used to disable the CTRL-M sequence which sends a carriage return ('\r') that gets translated by the terminal into a newline ('\n')
                                                                                //
                                                                                //All these other flags are part of entering raw mode even though they are either already turned off by default or don't apply for modern terminal emulators, explenation for these goes as follow:
                                                                                //disabling BRKINT prevents a break condition (like CTRL+C) to send a SIGINT signal to the terminal (i guess it stands for SIGnal INTerrupt)
                                                                                //disabling INPCK turns off parity checking even though it doesn't apply to modern terminal emulators.
                                                                                //disabling ISTRIP prevents the 8th bit of each input to be stripped (set to 0)

                    raw.c_cflag |= (CS8);                                       //CS8 is not a flag, it's part of the c_flags and it's a bitmask with multiple bits, which we set using bitwise-OR (|=), it sets all the character size to be 8 bits per byte, on many sistems it is already like this.



                    raw.c_oflag &= ~(OPOST);                                    //c_oflag is for the output flags
                                                                                //OPOST is an output flag used to disable the output processing by the terminal, likely the only output processing done by the terminal in this case is the conversion from newline ('\n') to newline and carriage return ("\r\n")

                    raw.c_cc[VMIN] = 0;                                         //VMIN is a variable contained under the c_cc array (which stands for "control characters") of the termios struct, it tells the terminal how many bytes we wait for the input to read, setting it to 0 means that the read() function returns as long as there is any input on the terminal
                    raw.c_cc[VTIME] = 1;                                        //VTIME on the other hand, sets the maximum amount of time that read() waits before returning the input, we set it to 1 so that read returns when there is input to read

                    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);   //Applying modified attributes to the terminal, TCSAFLUSH specifies when to apply the changes, in this case it waits for all pending output to be written to the terminal and discards any not read input

                    return;
                #endif
            }//terminal::internal::rawMode::enable();
        }//Namespace terminal::internal::rawMode;

        //Function that executes a power (used in terminal::internal::convert namespace to convert a normal number to a string)
        long double power(long base, long exponent){
            long double output = 1; //Starts with the output as 1 (if the exponent is 0 then the output is 1)
            if(exponent != 0) //If the exponent is not 0 execute the power, otherwise return the outpu which was set to 1
            {
                //Multiply the output by the base as many times as the absolute value of the exponent
                if(exponent > 0)for(long i = 1;i <= exponent;i++)output *= base; 
                else if(exponent < 0) for(long i = 1;i <= exponent * -1;i++)output *= base;

                //If the exponent was negative return 1/output
                return (exponent > 0 ? output : 1.000000000000000L / output);
            } else return output;
        }//terminal::internal::power();

        //Namespace that contains all the 
        namespace convert {
            //Function that returns a single digit of a long double
            int returnSingleDigit(long double input, int digit){
                if(input < 0)input *= -1; //Take the absolute value of the input

                //If the digit is positive then return the asked digit for the decimal part, otherwise return the asked digit for the whole part
                if(digit >= 0){
                    for(int i = 0;i < digit;i++)input /= 10;
                } else {
                    for(int i = 0;i > digit;i--)input *= 10;
                }

                return (int)(input - (unsigned long)(input / 10) * 10); //return the output rounded down
            }//terminal::internal::convert::returnSingleDigit();

            //Function used to get the number of digits of a long double input
            int numberOfDigits(long double input){
                unsigned int nOfDigits; //used to store the number of whole digits
                unsigned int nOfDecimals; //used to store the number of decimal digits
                unsigned int nOfTotalDigits; //The sum of the two previous variables

                long double absInput = (input < 0 ? input * -1 : input); //Store the absolute value of the input (invert the input if it's less than 0)

                for(nOfDigits = 1;true;nOfDigits++){
                    if(absInput < (absInput >= 0 ? terminal::internal::power(10, nOfDigits) : terminal::internal::power(10, nOfDigits) * -1))break;
                }
                if(input < 0)nOfDigits++;

                nOfTotalDigits = nOfDigits;
                        
                if((long double)input > (long)input && (long double)input < (long)input + 1 || (long double)input < (long)input && (long double)input > (long)input - 1){
                    nOfDecimals = terminal::out::print_precision;

                    for(int i = terminal::out::print_precision + 1;i > 0;i--){
                        if(returnSingleDigit(input, (i - 1) * -1) == 0)nOfDecimals--;
                        else break;
                    }

                    nOfTotalDigits += ++nOfDecimals;
                }

                return nOfTotalDigits;
            }//terminal::internal::convert::numberOfDigits();

            void toCharArray(long double input, char *output, unsigned int output_length){
                const char negativeSign = '-';
                const char decimalSign = '.';

                unsigned int nOfDigits;
                unsigned int nOfDecimals;
                unsigned int nOfTotalDigits;
                
                bool negative = false;
                bool decimal = false;

                if(input < 0)negative=true;
                for(nOfDigits = 1;true;nOfDigits++)if((negative ? input * -1 : input) < ((negative ? input * -1 : input) >= 0 ? terminal::internal::power(10, nOfDigits) : terminal::internal::power(10, nOfDigits) * -1))break;
                if(negative)nOfDigits++;

                nOfTotalDigits = nOfDigits;
                
                if((long double)input > (long)input && (long double)input < (long)input + 1 || (long double)input < (long)input && (long double)input > (long)input - 1){        
                    nOfDecimals = terminal::out::print_precision;
                    decimal = true;

                    for(int i = terminal::out::print_precision + 1;i > 0;i--){
                        if(returnSingleDigit(input, (i - 1) * -1) == 0)nOfDecimals--;
                        else break;
                    }

                    nOfTotalDigits += nOfDecimals + 1;
                }

                char converted[nOfTotalDigits];

                if(negative)converted[0] = negativeSign;
                for(int i = (unsigned int)negative;i < nOfDigits;i++)converted[i] = (char)returnSingleDigit(input, nOfDigits - i - 1) + 48;

                if(decimal){
                    converted[nOfDigits] = decimalSign;
                    for(int i = 1;i <= nOfDecimals;i++)converted[nOfDigits + i] = returnSingleDigit(input, i * -1) + 48;
                }

                for(int i = 0;i <= nOfTotalDigits && i < output_length;i++)*(output + i) = converted[i];

                return;
            }
        }//Namespace terminal::internal::convert;

        namespace concat {
            int sumAllLength(const char input[]){
                int inputLength;
                for(inputLength = 0;input[inputLength] != '\0';inputLength++);
                return inputLength;
            }

            int sumAllLength(long double input){
                return terminal::internal::convert::numberOfDigits(input);
            }

            void sumAll(const char input[], char *output, bool onlyChar = false){
                int start;
                for(start = 0;true;start++)if(*(output + start) == 0)break;

                for(int i = 0;input[i] != '\0';i++)*(output + start + i) = input[i];

                return;
            }

            void sumAll(long double input, char *output, bool onlyChar = false){
                if(onlyChar)return;

                int char_length = terminal::internal::convert::numberOfDigits(input);
                char char_input[char_length];

                terminal::internal::convert::toCharArray(input, char_input, char_length);

                int start;
                for(start = 0;true;start++)if(*(output + start) == 0)break;

                for(int i = 0;i < char_length;i++)*(output + start + i) = char_input[i];

                return;
            }
        }
    }
} //namespace terminal;

namespace file{
    void open(const char name[]){
        
    }
}

void terminal::in::get_str(bool echo, bool eSI, bool enterBreaks, char endChar, unsigned int maxLength){
    bool outputN = false;

    char* output1;
    char* output0;

    for(;;){
        char current_char = terminal::in::get_ch(true, echo, true, false, eSI);

        if(current_char == '\n' && enterBreaks)break;
        if(current_char == endChar && endChar != '\0')break;
        if(terminal::in::str_length > maxLength && maxLength > 0)break;

        if(current_char == 127){
            if(terminal::in::str_length < 1)continue;

            if(outputN == 0)*(output0 + --terminal::in::str_length) = '\0'; 
            else if(outputN == 1)*(output1 + --terminal::in::str_length) = '\0';

            terminal::cur::move_back();
            terminal::out::printch(' ');
            terminal::cur::move_back();
            continue;
        }

        if(outputN == 1){
            output1 = new char[++terminal::in::str_length];
            outputN = !outputN;

            for(unsigned int i = 0;i < terminal::in::str_length - 1;i++)*(output1 + i) = *(output0 + i);
            *(output1 + terminal::in::str_length - 1) = current_char;

            delete output0;
        } else if(outputN == 0){
            output0 = new char[++terminal::in::str_length];
            outputN = !outputN;

            for(unsigned int i = 0;i < terminal::in::str_length - 1;i++)*(output0 + i) = *(output1 + i);
            *(output0 + terminal::in::str_length - 1) = current_char;

            delete output1;
        }
    }

    delete terminal::internal::input;
    terminal::internal::input = new char[terminal::in::str_length];

    for(unsigned int i = 0;i < terminal::in::str_length;i++)*(terminal::internal::input + i) = (outputN ? *(output0 + i) : *(output1 + i));

    return;
}

void terminal::in::store_str(char* output){
    for(int i = 0;i < terminal::in::str_length;i++)*(output + i) = *(terminal::internal::input + i);
    *(output + terminal::in::str_length) = '\0';

    terminal::in::str_length = 0;
    delete terminal::internal::input;

    return;
}

char terminal::in::get_ch(bool waitForInput, bool echo, bool rawMode, bool eSC, bool eSI){
    #if defined(_WIN32)
    #elif defined(__linux__)
        if(rawMode)terminal::internal::rawMode::enable(echo, eSI);

        char input[4];
        for(int i = 0;i < 4;i++)input[i] = 0;

        if(waitForInput)while(!read(STDIN_FILENO, &input, 4));
        else read(STDIN_FILENO, &input, 4);

        if(rawMode)terminal::internal::rawMode::disable();

        if(eSC){
            if(input[0] == '\e' && input[1] == 79){
                //Special key decoding for the XFree4 keyboard (default linux terminal keyboard)

                switch(input[2]){
                    case 51 : if(input[3] == 126)return KEY_DELETE;
                    case 53 : if(input[3] == 126)return KEY_PGUP;
                    case 54 : if(input[3] == 126)return KEY_PGDOWN;

                    case 65 : return KEY_UP;
                    case 66 : return KEY_DOWN;
                    case 67 : return KEY_RIGHT;
                    case 68 : return KEY_LEFT;

                    case 69 : return KEY_CENTER;
                    case 70 : return KEY_ENDL;
                    case 72 : return KEY_STARTL;

                    case 80 : return KEY_F1;
                    case 81 : return KEY_F2;
                    case 82 : return KEY_F3;
                    case 83 : return KEY_F4;

                    case 49 : {
                        switch(input[3]){
                            case 53 : return KEY_F5;
                            case 55 : return KEY_F6;
                            case 56 : return KEY_F7;
                            case 57 : return KEY_F8;
                        }
                    }

                    case 50 : {
                        switch(input[3]){
                            case 48 : return KEY_F9;
                            case 49 : return KEY_F10;
                            case 51 : return KEY_F11;
                            case 52 : return KEY_F12;

                            case 126 : return KEY_INSERT;
                        }
                    }
                }
            } else if(input[0] == '\e' && input[1] == 91){
                //Special key decoding for the linux console / macOS keyboard

                switch(input[2]){
                    case 52 : if(input[3] == 126)return KEY_ENDL;
                    case 51 : if(input[3] == 126)return KEY_DELETE;
                    case 53 : if(input[3] == 126)return KEY_PGUP;
                    case 54 : if(input[3] == 126)return KEY_PGDOWN;

                    case 65 : return KEY_UP;
                    case 66 : return KEY_DOWN;
                    case 67 : return KEY_RIGHT;
                    case 68 : return KEY_LEFT;
                    case 71 : return KEY_CENTER;

                    case 80 : return KEY_PAUSE;

                    case 91 : {
                        switch(input[3]){
                            case 65 : return KEY_F1;
                            case 66 : return KEY_F2;
                            case 67 : return KEY_F3;
                            case 68 : return KEY_F4;
                            case 69 : return KEY_F5;
                        }
                    }

                    case 49 : {
                        switch(input[3]){
                            case 55 : return KEY_F6;
                            case 56 : return KEY_F7;
                            case 57 : return KEY_F8;

                            case 126 : return KEY_STARTL;
                        }
                    }

                    case 50 : {
                        switch(input[3]){
                            case 48 : return KEY_F9;
                            case 49 : return KEY_F10;
                            case 51 : return KEY_F11;
                            case 52 : return KEY_F12;

                            case 126 : return KEY_INSERT;
                        }
                    }
                }
            } else if(input[0] == '\e' && input[1] == 0)return KEY_ESCAPE; 
            else return input[0];
        } else if(!eSC && input[0] >= 32 && input[0] != 127)return input[0]; //ASCII codes 0-31 and 127 are all control characters so we ignore em in the output
    #endif

    return '\0'; //If special characters were disabled and the input was a special character return 0
}

template <typename... args>
void terminal::out::sprint(args... text){
    int totSize = (sizeof(text) + ...); 
    char totText[totSize];
    for(int i = 0;i < totSize;i++)totText[i] = 0;

    using expander = int[]; 
    (void)expander{0, ((void)terminal::internal::concat::sumAll(text, totText, true), 0)...}; 

    #if defined(_WIN32)
    #elif defined(__linux__)
        write(STDOUT_FILENO, totText, totSize);
    #endif

    return;
}

template <typename... args>
void terminal::out::sprintln(args... text){
    int totSize = (sizeof(text) + ...); 
    char totText[totSize];
    for(int i = 0;i < totSize;i++)totText[i] = 0;

    using expander = int[]; 
    (void)expander{0, ((void)terminal::internal::concat::sumAll(text, totText, true), 0)...}; 

    #if defined(_WIN32)
    #elif defined(__linux__)
        write(STDOUT_FILENO, totText, totSize);
        write(STDOUT_FILENO, ENDLINE, 2);
    #endif

    return;
}

template <typename... args>
void terminal::out::print(args... text){
    int totSize = (terminal::internal::concat::sumAllLength(text) + ...); 
    char totText[totSize];
    for(int i = 0;i < totSize;i++)totText[i] = 0;

    using expander = int[]; 
    (void)expander{0, ((void)terminal::internal::concat::sumAll(text, totText), 0)...};

    #if defined(_WIN32)
    #elif defined(__linux__)
        write(STDOUT_FILENO, totText, totSize);
    #endif

    return;
}

template <typename... args>
void terminal::out::println(args... text){
    int totSize = (terminal::internal::concat::sumAllLength(text) + ...);
    char totText[totSize];
    for(int i = 0;i < totSize;i++)totText[i] = 0;

    using expander = int[];
    (void)expander{0, ((void)terminal::internal::concat::sumAll(text, totText), 0)...}; 

    #if defined(_WIN32)
    #elif defined(__linux__)
        write(STDOUT_FILENO, totText, totSize);
        write(STDOUT_FILENO, ENDLINE, 2);
    #endif

    return;
}

void terminal::out::printch(char input){
    char output[1];
    output[0] = input;

    #if defined(_WIN32)
    #elif defined(__linux__)
        write(STDOUT_FILENO, output, 1);
    #endif

    return;
}

#endif
