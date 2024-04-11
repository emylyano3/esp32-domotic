#include <string.h>
#include "Logger.h"

class Utils {   
    public:
        static bool endsWith(char* a, const char* b) {
            return String(a).endsWith(b);
        }

        static std::string concat(char* a, const char* b) {
            return std::string(a) + b;
        }
         
        static std::string concat(const char* a, const char* b) {
            return std::string(a) + b;
        }

        static void copy(char* a, const char* b, uint16_t size) {
            String(b).toCharArray(a, size);
        }

        static int toInt(char* s) {
            return String(s).toInt();
        }
 
        static int mqttPayloadToInt(uint8_t* payload, unsigned int length) {
            int value = 0;
            for (int i = 0; i < length; i++) {
                value = value * 10 + (payload[i] - '0');
            }
            return value;
        }

        /* Lee el digito del payload y lo transforma a HIGH / LOW */
        static uint8_t interpretMqttBinaryPayload(uint8_t* payload, unsigned int length) {
            #ifdef LOGGING
            log("Payload received", (*payload - '0'));
            #endif
            return (*payload - '0') > 0 ? HIGH : LOW;
        }

        static const char* getLogicState(uint8_t state) {
            //TODO Mapear este estado de acuerdo a la funcion que este definida
            // deberia manejar la logica invertida (si aplica)
            return state == HIGH ? "1" : "0";
        }
};