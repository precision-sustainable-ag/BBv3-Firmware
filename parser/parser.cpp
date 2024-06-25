#define TESTING

#include <stdio.h>
#include <iostream>
#include <config.hpp>
#include <commands.hpp>
//#include <ClearCore.h>

using namespace std;

class Parser {
public:
    Parser(){
        available = true;
    }
    int32_t parse_command(char * input_command){
        switch (*(input_command ++)){
            case M:{
                switch (*(input_command++)){
                    case A:{
                        cout << "Move Absolute Command\n";
                        Parser::absolute_motion = true;
                        if (Parser::parse_movement_command(input_command) < 0) return -1;
                    } break;
                    case R:{
                        cout << "Move Relative Command\n";
                        Parser::absolute_motion = false;
                        if (Parser::parse_movement_command(input_command) < 0) return -1;
                    } break;
                    default:{
                        return -1;
                    }
                }
            } break;
            case C:{
                cout << "Ready for Configuration\n";
            } break;
            case K:{
                cout << "MOTORS STOPPED\n";
                return 0;
            } break;
            case QUESTION:{
                switch (*(input_command ++)){
                    case P:{
                        cout << "Query Position\n";
                    } break;
                    case C:{
                        cout << "Query Config\n";
                    } break;
                    case A:{
                        cout << "Query Axes\n";
                    } break;
                }
            } break;
            default:{
                return -1;
            }
        }
        return 0;
    }

    bool is_axis(char value){
        return ((value >= A) && (value <= Z));
    }

    bool is_axis_end(char value){
        return ((value == LF) || (value == CR) || (value == SPACE) || (value == NULL_CHAR));
    }

    bool is_end(char value){
        return ((value == LF) || (value == CR) || (value == NULL_CHAR));
    }

    int32_t parse_movement_command(char * command){
        char * current_char = command;
        while (!Parser::is_end(*current_char)){
            if ((*(current_char++)) != SPACE) return -1;
            if (!Parser::is_axis(*(current_char++))) return -1;
            while (!Parser::is_axis_end(*current_char)){
                printf("%c",*current_char);
                current_char++;
            }
            cout << "\n";
        }
        return 0;
    }

private:
    bool available = false;
    bool absolute_motion = true;
    char axes[MAX_AXES + 1] = AXES;
    char command[128];
};

int main(){
    Parser parser;
    char command_stream[128];
    while(1){
        fgets(&command_stream[0],128,stdin);
        if (parser.parse_command(&command_stream[0])<0){
            cout << "CMD FORMAT ERROR\n";
        }
    }
}