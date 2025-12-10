#ifndef CHAT_PARTICIPANT_HPP
#define CHAT_PARTICIPANT_HPP
#include <unordered_set>
#include "interface_controllable_broadcaster.hpp"

class IChatParticipant : public IControllableBroadcaster
{
public:
    virtual ~IChatParticipant() {}
    virtual void start() = 0;
};
typedef std::unique_ptr<IChatParticipant> IChatParticipant_ptr;



namespace chat_participant
{
    namespace details 
    {
        struct ChatPartHash_
        {
            using is_transparent = void;

            auto operator()(IChatParticipant* p) const { return std::hash<IChatParticipant*>{}(p); }
            auto operator()(const IChatParticipant_ptr& p) const { return std::hash<IChatParticipant*>{}(p.get()); }
        };

        struct ChartPartEqual_
        {
            using is_transparent = void;
            template <typename LHS, typename RHS>
            auto operator()(const LHS& lhs, const RHS& rhs) const
            {
                return AsPtr(lhs) == AsPtr(rhs);
            }
        private:
            static const IChatParticipant* AsPtr(const IChatParticipant* p) { return p; }
            static const IChatParticipant* AsPtr(const IChatParticipant_ptr& p) { return p.get(); }
        };
    }

    using unique_ptr_set = std::unordered_set<IChatParticipant_ptr, details::ChatPartHash_, details::ChartPartEqual_>;
}

#endif