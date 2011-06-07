/*
 * hexDump.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Miha Vitorovic
 */

#include <pv/hexDump.h>

#include <iostream>
#include <sstream>

using namespace epics::pvData;
using std::stringstream;
using std::endl;
using std::cout;

namespace epics {
    namespace pvAccess {

        String toHex(int8);
        char toAscii(int8);

        void hexDump(const String name, const int8 *bs, int len) {
            hexDump(name, bs, 0, len);
        }

        void hexDump(const String name, const int8 *bs, int start, int len) {
            hexDump("", name, bs, start, len);
        }

        void hexDump(const String prologue, const String name, const int8 *bs,
                int start, int len) {

            stringstream header;

            header<<prologue<<endl<<"Hexdump ["<<name<<"] size = "<<len;

            String out(header.str());

            String chars;

            for(int i = start; i<(start+len); i++) {
                if(((i-start)%16)==0) {
                    out += chars;
                    out += '\n';
                    chars.clear();
                }

                chars += toAscii(bs[i]);

                out += toHex(bs[i]);

                if(((i-start)%4)==3) {
                    chars += ' ';
                    out += ' ';
                }
            }

            if(len%16!=0) {
                int pad = 0;
                int delta_bytes = 16-(len%16);

                //rest of line (no of bytes)
                //each byte takes two chars plus one ws
                pad = delta_bytes*3;

                //additional whitespaces after four bytes
                pad += (delta_bytes/4);
                pad++;

                for(int i = 0; i<pad; i++)
                    chars.insert(0, " ");
            }

            out += chars;
            cout<<out<<endl;
        }

        /**
         * byte to hexchar mapping.
         */
        static const char lookup[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

        /**
         * Get hex representation of byte.
         * @param b
         * @return string hex representation of byte.
         */
        String toHex(int8 b) {
            String sb;

            int upper = (b>>4)&0x0F;
            sb += lookup[upper];

            int lower = b&0x0F;
            sb += lookup[lower];

            sb += ' ';

            return sb;
        }

        /**
         * Get ASCII representation of byte, dot if non-readable.
         * @param b
         * @return ASCII representation of byte, dot if non-readable.
         */
        char toAscii(int8 b) {
            if(b>(int8)31&&b<(int8)127)
                return (char)b;
            else
                return '.';
        }

    }
}
