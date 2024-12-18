class Timer{
private:
    int _start_val;
    int value;
    bool ticking;
public:
    void tick(){
        if(ticking)
            value = millis() - _start_val;
    }
    void start(){
        if(ticking == false)
        {
            _start_val = millis();
            ticking = true;
        }
    }
    void stop(){
        ticking = false;
    }
    
    void reset(){
        ticking = false;
        value = 0;
    }
    void set_value(int val)
    {
        value = val;
    }
    int get_value(){
        return value;
    }
    bool isticking(){
        return ticking;
    }
};