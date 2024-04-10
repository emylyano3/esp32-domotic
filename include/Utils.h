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

        static uint8_t mqttPayloadToBinaryState(uint8_t* payload, unsigned int length) {
            log("Payload received", (*payload - '0'));
            return (*payload - '0') > 0 ? 1 : 0;
        }
};