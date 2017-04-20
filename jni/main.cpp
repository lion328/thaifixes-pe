#include <iostream>
#include <string>
#include <vector>

#include "mcpelauncher_api.h"

const char *THAIFIXES_FLOATING_VOWELS = "\x31\x34\x35\x36\x37\x38\x39\x3A\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x00";

class ThaiFixes {
public:
    static bool isFloatingVowel(unsigned int ch) {
        if ((ch >> 8) != 0x0E) return false;
        
        ch &= 0xFF;
        
        const char *ptr = THAIFIXES_FLOATING_VOWELS;
        
        while (*ptr) {
            if ((*ptr) == ch) return true;
            
            ptr++;
        }
        
        return false;
    }
};

class Color {};

class Font {
public:
    class TextObject {};
    class GlyphQuad {};
    
    TextObject _makeTextObject(const std::string &text, const Color &color, bool b, int barPos);
    float _getCharWidth(int ch, bool unicode) const;
    float _buildChar(std::vector<GlyphQuad, std::allocator<GlyphQuad>> &quads, int ch, const Color &color, bool italic, float x, float y, bool unicode) const;
    
    TextObject _hook__makeTextObject(const std::string &text, const Color &color, bool b, int barPos) {
        return this->_makeTextObject(text, color, b, barPos);
    }
    
    float _hook__getCharWidth(int ch, bool unicode) const {
        if (ThaiFixes::isFloatingVowel(ch)) return 0.0f;
        
        return this->_getCharWidth(ch, unicode);
    }
    
    float _hook__buildChar(std::vector<GlyphQuad, std::allocator<GlyphQuad>> &quads, int ch, const Color &color, bool italic, float x, float y, bool unicode) const {
        if (ThaiFixes::isFloatingVowel(ch)) x -= this->_getCharWidth(ch, unicode);
        
        return _buildChar(quads, ch, color, italic, x, y, unicode);
    }
};

void test_hook(void *original, void *hook) {
    void *tmp;
    mcpelauncher_hook(original, hook, &tmp);
}

extern "C" void mod_init() {
    test_hook(reinterpret_cast<void *>(&Font::_makeTextObject), reinterpret_cast<void *>(&Font::_hook__makeTextObject));
    test_hook(reinterpret_cast<void *>(&Font::_getCharWidth), reinterpret_cast<void *>(&Font::_hook__getCharWidth));
    test_hook(reinterpret_cast<void *>(&Font::_buildChar), reinterpret_cast<void *>(&Font::_hook__buildChar));
}
