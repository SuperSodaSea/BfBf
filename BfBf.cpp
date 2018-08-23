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
#include <iostream>
#include <string>
#include <vector>


inline std::string concat(const std::vector<std::string>& arr) {
    
    std::string ret;
    for(auto&& s : arr) ret += s;
    return ret;
    
}

class Generator {
    
public:
    
    using Type = std::uint16_t;
    
private:
    
    Type position = 0;
    Type current = 0;
    
public:
    
    Type allocate(Type count = 1) { auto ret = position; position += count; return ret; }
    Type getCurrent() { return current; }
    
    std::string go(Type a) {
        
        std::string ret;
        if(current <= a) ret = std::string(a - current, '>');
        else ret = std::string(current - a, '<');
        current = a;
        return ret;
        
    }
    std::string zero(Type a) { return go(a) + "[-]"; }
    std::string inc(Type a) { return go(a) + '+'; }
    std::string dec(Type a) { return go(a) + '-'; }
    std::string add(Type a, std::uint8_t value) { return go(a) + std::string(value, '+'); }
    std::string sub(Type a, std::uint8_t value) { return go(a) + std::string(value, '-'); }
    std::string set(Type a, std::uint8_t value) { return zero(a) + std::string(value, '+'); }
    std::string input(Type a) { return go(a) + ","; }
    std::string output(Type a) { return go(a) + "."; }
    std::string move(Type a, Type b) {
        
        return concat({zero(b), go(a), "[", go(b), "+", go(a), "-]"});
        
    }
    std::string assign(Type a, Type b, Type t) {
        
        return concat({
            zero(b), zero(t),
            move(a, t),
            go(t), "[", go(a), "+", go(b), "+", go(t), "-]",
        });
        
    }
    
    template <typename F1, typename F2>
    std::string loop(F1&& f1, F2&& f2) {
        
        return concat({f1(), "[", f2(), f1(), "]"});
        
    }
    template <typename F1>
    std::string cond(Type a, Type t, F1&& f1) {
        
        return concat({
            assign(a, t, t + 1),
            go(t), "[", f1(), zero(t), "]",
        });
        
    }
    template <typename F1, typename F2>
    std::string cond(Type a, Type t, F1&& f1, F2&& f2) {
        
        return concat({
            set(t, 1), zero(t + 1),
            go(a), "[", f1(), zero(t), move(a, t + 1), "]",
            move(t + 1, a),
            go(t), "[", f2(), zero(t), "]",
        });
        
    }
    
    std::string zero16(Type a) {
        
        return concat({zero(a), zero(a + 1)});
        
    }
    std::string move16(Type a, Type b) {
        
        return concat({move(a, b), move(a + 1, b + 1)});
        
    }
    std::string assign16(Type a, Type b, Type t) {
        
        return concat({assign(a, b, t), assign(a + 1, b + 1, t)});
        
    }
    std::string inc16(Type a, Type t) {
        
        return concat({
            inc(a),
            cond(a, t, [] { return ""; }, [=] { return inc(a + 1); }),
        });
        
    }
    std::string dec16(Type a, Type t) {
        
        return concat({
            cond(a, t, [] { return ""; }, [=] { return dec(a + 1); }),
            dec(a),
        });
        
    }
    std::string notZero16(Type a, Type b, Type t) {
        
        return concat({
            zero(b),
            cond(a, t, [=] { return inc(b); }),
            cond(a + 1, t, [=] { return inc(b); }),
            go(b),
        });
        
    }
    // |    0    |    1    |    2    |    3    |    4    |    5    |    6    |    7    |
    // |      index1       |      index2       |  data   |             tmp             |
    std::string readArray16(Type array, Type index, Type data) {
        
        return concat({
            assign16(index, array, array + 2), assign16(array, array + 2, array + 4),
            loop([&] { return notZero16(array, array + 5, array + 6); }, [&] { return concat({
                dec16(array, array + 5),
                move(array + 3, array + 4), move(array + 2, array + 3), move(array + 1, array + 2), move(array, array + 1),
                move(array + 8, array),
                ">",
            }); }),
            assign(array + 8, array + 4, array + 5),
            loop([&] { return notZero16(array + 2, array + 5, array + 6); }, [&] { return concat({
                dec16(array + 2, array + 5),
                move(array + 2, array + 1), move(array + 3, array + 2), move(array + 4, array + 3),
                "<",
                move(array, array + 8),
            }); }),
            assign(array + 4, data, array),
        });
        
    }
    std::string writeArray16(Type array, Type index, Type data) {
        
        return concat({
            assign16(index, array, array + 2), assign16(array, array + 2, array + 4), assign(data, array + 4, array + 5),
            loop([&] { return notZero16(array, array + 5, array + 6); }, [&] { return concat({
                dec16(array, array + 5),
                move(array + 4, array + 5), move(array + 3, array + 4), move(array + 2, array + 3), move(array + 1, array + 2), move(array, array + 1),
                move(array + 8, array),
                ">",
            }); }),
            assign(array + 4, array + 8, array + 5),
            loop([&] { return notZero16(array + 2, array + 5, array + 6); }, [&] { return concat({
                dec16(array + 2, array + 5),
                move(array + 2, array + 1), move(array + 3, array + 2),
                "<",
                move(array, array + 8),
            }); }),
        });
        
    }
    
};


int main() {
    
    Generator g;
    
    auto codePointer = g.allocate(2);
    auto dataPointer = g.allocate(2);
    auto buffer = g.allocate();
    auto level = g.allocate();
    auto t = g.allocate(2);
    auto data = g.allocate();
    
    auto str = concat({
        g.zero16(codePointer), g.zero16(dataPointer),
        g.loop([&] { return g.input(buffer); }, [&] { return concat({
            g.sub(buffer, '+'), g.cond(buffer, t, [&] { return concat({
                g.sub(buffer, ',' - '+'), g.cond(buffer, t, [&] { return concat({
                    g.sub(buffer, '-' - ','), g.cond(buffer, t, [&] { return concat({
                        g.sub(buffer, '.' - '-'), g.cond(buffer, t, [&] { return concat({
                            g.sub(buffer, '<' - '.'), g.cond(buffer, t, [&] { return concat({
                                g.sub(buffer, '>' - '<'), g.cond(buffer, t, [&] { return concat({
                                    g.sub(buffer, '[' - '>'), g.cond(buffer, t, [&] { return concat({
                                        g.sub(buffer, ']' - '['), g.cond(buffer, t, [&] { return ""; },
                                            [&] { return concat({g.set(buffer, 8),  g.writeArray16(data, dataPointer, buffer), g.inc16(dataPointer, t)}); }),
                                    }); }, [&] { return concat({g.set(buffer, 7), g.writeArray16(data, dataPointer, buffer), g.inc16(dataPointer, t)}); }),
                                }); }, [&] { return concat({g.set(buffer, 6), g.writeArray16(data, dataPointer, buffer), g.inc16(dataPointer, t)}); }),
                            }); }, [&] { return concat({g.set(buffer, 5), g.writeArray16(data, dataPointer, buffer), g.inc16(dataPointer, t)}); }),
                        }); }, [&] { return concat({g.set(buffer, 4), g.writeArray16(data, dataPointer, buffer), g.inc16(dataPointer, t)}); }),
                    }); }, [&] { return concat({g.set(buffer, 3), g.writeArray16(data, dataPointer, buffer), g.inc16(dataPointer, t)}); }),
                }); }, [&] { return concat({g.set(buffer, 2), g.writeArray16(data, dataPointer, buffer), g.inc16(dataPointer, t)}); }),
            }); }, [&] { return concat({g.set(buffer, 1), g.writeArray16(data, dataPointer, buffer), g.inc16(dataPointer, t)}); }),
        }); }),
        g.zero(buffer), g.writeArray16(data, dataPointer, buffer), g.inc16(dataPointer, t),
        g.loop([&] { return concat({g.readArray16(data, codePointer, buffer), g.go(buffer)}); }, [&] { return concat({
            g.dec(buffer), g.cond(buffer, t, [&] { return concat({
                g.dec(buffer), g.cond(buffer, t, [&] { return concat({
                    g.dec(buffer), g.cond(buffer, t, [&] { return concat({
                        g.dec(buffer), g.cond(buffer, t, [&] { return concat({
                            g.dec(buffer), g.cond(buffer, t, [&] { return concat({
                                g.dec(buffer), g.cond(buffer, t, [&] { return concat({
                                    g.dec(buffer), g.cond(buffer, t, [&] { return concat({
                                        g.dec(buffer), g.cond(buffer, t, [&] { return ""; }, [&] { return concat({
                                            // ]
                                            g.readArray16(data, dataPointer, buffer),
                                            g.cond(buffer, t, [&] { return concat({
                                                g.set(level, 1),
                                                g.loop([&] { return g.go(level); }, [&] { return concat({
                                                    g.dec16(codePointer, t), g.readArray16(data, codePointer, buffer),
                                                    g.sub(buffer, 7), g.cond(buffer, t, [&] { return concat({
                                                        g.dec(buffer), g.cond(buffer, t, [&] { return ""; }, [&] { return g.inc(level); }),
                                                    }); }, [&] { return g.dec(level); }),
                                                }); }),
                                            }); }, [] { return ""; }),
                                        }); }),
                                    }); }, [&] { return concat({
                                        // [
                                        g.readArray16(data, dataPointer, buffer),
                                        g.cond(buffer, t, [] { return ""; }, [&] { return concat({
                                            g.set(level, 1),
                                            g.loop([&] { return g.go(level); }, [&] { return concat({
                                                g.inc16(codePointer, t), g.readArray16(data, codePointer, buffer),
                                                g.sub(buffer, 7), g.cond(buffer, t, [&] { return concat({
                                                    g.dec(buffer), g.cond(buffer, t, [&] { return ""; }, [&] { return g.dec(level); }),
                                                }); }, [&] { return g.inc(level); }),
                                            }); }),
                                        }); }),
                                    }); }),
                                }); }, [&] { return concat({
                                    // >
                                    g.inc16(dataPointer, t),
                                }); }),
                            }); }, [&] { return concat({
                                // <
                                g.dec16(dataPointer, t),
                            }); }),
                        }); }, [&] { return concat({
                            // .
                            g.readArray16(data, dataPointer, buffer), g.output(buffer),
                        }); }),
                    }); }, [&] { return concat({
                        // -
                        g.readArray16(data, dataPointer, buffer), g.dec(buffer), g.writeArray16(data, dataPointer, buffer),
                    }); }),
                }); }, [&] { return concat({
                    // ,
                    g.input(buffer), g.writeArray16(data, dataPointer, buffer),
                }); }),
            }); }, [&] { return concat({
                // +
                g.readArray16(data, dataPointer, buffer), g.inc(buffer), g.writeArray16(data, dataPointer, buffer),
            }); }),
            g.inc16(codePointer, t),
        }); }),
    });
    
    constexpr std::size_t LINE = 100;
    for(std::size_t i = 0; i < str.size() / LINE; ++i)
        std::cout << str.substr(i * LINE, LINE) << '\n';
    if(str.size() % LINE) std::cout << str.substr(str.size() / LINE * LINE) << '\n';
    
}
