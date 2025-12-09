#ifndef MESSAGE_VARIANT_H
#define MESSAGE_VARIANT_H

#include "formatted_message.h"
#include <variant>

using MessageVariant = std::variant<QString, FormattedMessage>;


#endif // MESSAGE_VARIANT_H
