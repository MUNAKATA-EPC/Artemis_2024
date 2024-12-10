#ifndef _BUTTON_HPP
#define _BUTTON_HPP

class Button{
public:
    typedef enum{
        PULLUP,
        PULLDOWN
    }Button_Value_Type;

    void init(int pin, Button_Value_Type type);
    void loop();
    bool is_pushing();
    bool is_pushed();
    bool is_released();
private:
    int _pin;
    int _push_time;
    Button_Value_Type _value_type;
};

#endif