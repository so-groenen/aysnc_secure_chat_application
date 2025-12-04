#ifndef MESSAGE_VARIANT_H
#define MESSAGE_VARIANT_H

#include "message.h"
#include <variant>

using MessageVariant = std::variant<QString, Message>;


#endif // MESSAGE_VARIANT_H
