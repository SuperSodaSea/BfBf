/*
 *
 * MIT License
 *
 * Copyright (c) 2018 BfBf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <cstdint>
#include <cstdio>


enum Op {
    ADD = 1,
    INPUT = 2,
    SUB = 3,
    OUTPUT = 4,
    LEFT = 5,
    RIGHT = 6,
    BEGIN = 7,
    END = 8,
};


std::uint8_t memory[65536];

int main() {
    
    std::uint16_t codePointer = 0, dataPointer = 0;
    for(char c; (c = std::getchar()) != EOF; ) {
        
        switch(c) {
        case '+': memory[dataPointer++] = ADD; break;
        case ',': memory[dataPointer++] = INPUT; break;
        case '-': memory[dataPointer++] = SUB; break;
        case '.': memory[dataPointer++] = OUTPUT; break;
        case '<': memory[dataPointer++] = LEFT; break;
        case '>': memory[dataPointer++] = RIGHT; break;
        case '[': memory[dataPointer++] = BEGIN; break;
        case ']': memory[dataPointer++] = END; break;
        }
        
    }
    memory[dataPointer++] = 0;
    for(std::uint8_t code; (code = memory[codePointer]); ++codePointer) {
        
        switch(code) {
        // +
        case ADD: ++memory[dataPointer]; break;
        // ,
        case INPUT: { char c = std::getchar(); memory[dataPointer] = c == EOF ? 0 : c; break; }
        // -
        case SUB: --memory[dataPointer]; break;
        // .
        case OUTPUT: std::putchar(memory[dataPointer]); break;
        // <
        case LEFT: --dataPointer; break;
        // >
        case RIGHT: ++dataPointer; break;
        // [
        case BEGIN: {
            
            if(!memory[dataPointer]) {
                
                std::uint16_t level = 1;
                while(level) {
                    
                    ++codePointer;
                    code = memory[codePointer];
                    if(code == 7) ++level;
                    else if(code == 8) --level;
                    
                }
                
            }
            break;
            
        }
        // ]
        case END: {
            
            if(memory[dataPointer]) {
                
                std::uint16_t level = 1;
                while(level) {
                    
                    --codePointer;
                    code = memory[codePointer];
                    if(code == 7) --level;
                    else if(code == 8) ++level;
                    
                }
                
            }
            break;
            
        }
        }
        
    }
    return 0;
    
}
