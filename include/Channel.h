#ifndef Channel_h
#define Channel_h

#include <Arduino.h>

#ifndef CHANNEL_NAME_MAX_LENGTH 
#define CHANNEL_NAME_MAX_LENGTH 20
#endif

static const uint32_t   NO_TIMER = -1;

class Channel {
    private:
        //unmutable
        const char*             id;
        char*                   name;
        uint8_t                 pin;
        uint8_t                 pinMode;
        bool                    analog;
        bool                    inverted;

        //mutable
        int                     currState = -1;
        int                     prevState = -1;
        bool                    enabled = true;
        unsigned long           timer = NO_TIMER;
        unsigned long           timerControl;
        std::function<int(int)> valueMapper;

    public:
        Channel(const char* id, const char* name, uint8_t pin, uint8_t pinMode, bool analog, bool inverted);

        // Updates the channel´s name
        void    updateName (const char *v);

        // Set the value in channel and writes it over the device pin
        void    write(int value);

        /*
            Read the value from the pin related to the channel, and updates the channel state.
            Returns true if value is read from pin, false otherwise.
        */
        bool    read();

        /** Timer control */
        // Updates the timer control setting it to timer time ftom now
        void    updateTimerControl();
        bool    timeIsUp();

        /** Getters & setters*/
        bool    isOutput();
        bool    isAnalog();
        void    setAnalog(bool analog);
        bool    isEnabled();
        void    setEnabled(bool enabled);
        void    setTimer(uint32_t time);

        int     getPrevState();
        /* 
            Returns the channel current state mapped, according to the mapper function defined.
            If no mapper defined, plain value is returned instead
        */
        int     getMappedState();
        int     getRawState();
        void    setState(int state);
        char*   getName();

        // Sets the mapper to map the channel state
        void    setStateMapper(std::function<int(int)> mapper);
};

#endif