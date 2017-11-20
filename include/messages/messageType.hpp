//
// Created by jestjest on 11/19/17.
//

#ifndef INC_244B_FINAL_MESSAGETYPE_HPP
#define INC_244B_FINAL_MESSAGETYPE_HPP

namespace kdml {
    namespace net {
        enum class MessageType {
            PING,
            REPLY,
            FIND_NODE,
            FIND_VALUE,
            STORE,
            NONE
        };
    }
}

#endif //INC_244B_FINAL_MESSAGETYPE_HPP
