#ifndef Channel_h
#define Channel_h

#include <Arduino.h>

#ifndef CHANNEL_NAME_MAX_LENGTH 
#define CHANNEL_NAME_MAX_LENGTH 20
#endif

#ifndef CHANNEL_NO_TIMER 
#define CHANNEL_NO_TIMER 0
#endif

class Channel {
    private:
        //unmutable
        const char*     id;
        char*           name;
        uint8_t         pin;
        uint8_t         pinMode;
        bool            analog;

        //mutable
        unsigned long   timerControl    = 0;
        int             currState       = -1;
        int             prevState       = -1;
        bool            enabled         = true;
        unsigned long   timer           = CHANNEL_NO_TIMER;

        bool    timeIsUp();

        //TODO Agregar el mapeo del value del channel de entrada y salida (para los diferentes tipos de canales)
        // separando en sub clases el comportamiento de input y output channel para mantener la logica simpls 
        // std::function<int(int)>         outgoingValueMapper;
        // std::function<int(byte*, int)>  ingoingValueMapper;

    public:
        Channel(const char* id, const char* name, uint8_t pin, uint8_t pinMode, bool analog);

        // Updates the channel´s name
        bool    updateName (const char*);

        // Set the value in channel and writes it over the device pin
        void    write(int);

        /*
            Read the value from the pin related to the channel, and updates the channel state.
            Returns true if value is read from pin, false otherwise.
        */
        bool    read();

        /* Timer */
        // resets the timer control setting it to timer time ftom now
        void    startTimer();
        void    resetTimer();
        void    setTimer(uint32_t);
        bool    checkTimer();

        /* Getters & setters*/
        bool            isOutput();
        bool            isAnalog();
        void            setAnalog(bool);
        bool            isEnabled();
        void            setEnabled(bool);
        int             getPrevState();
        int             getCurrentState();
        void            setState(int);
        const char*     getId();
        char*           getName();
        unsigned long   getTimer();
        
        // Sets the mapper to map the channel state
        // void    setStateMapper(std::function<int(int)> mapper);
};

#endif