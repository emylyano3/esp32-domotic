#include <string.h>

class Utils {
    public:
        static bool endsWith(char* a, const char* b) {
            return String(a).endsWith(b);
        }

        static std::string concat(char* a, const char* b) {
            return std::string(a) + b;
        }

        static void copy(char* a, const char* b, uint16_t size) {
            String(b).toCharArray(a, size);
        }
};