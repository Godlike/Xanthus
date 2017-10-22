#ifndef XANTHUS_INTENT_INTENT_HPP
#define XANTHUS_INTENT_INTENT_HPP

#include "WorldTime.hpp"

#include <cstdint>
#include <memory>

namespace xanthus
{
namespace intent
{

/** Intent interface */
class Intent
{
public:
    enum class State : uint8_t
    {
        Unknown     = 0x00
        , Running
        , Finished
        , Invalid
    };

    virtual ~Intent() = 0;

    //! Checks if intent is valid to be executed
    virtual bool IsValid() const = 0;

    /** @brief Executes valid intent
     *
     *  @param  now world time point
     *
     *  @return @c true if intent was fulfilled and can be erased, @c false otherwise
     */
    virtual bool Execute(WorldTime::TimeUnit now) = 0;

    //! Discards invalid intent
    virtual void Discard() = 0;

protected:
    Intent() = default;

    Intent(Intent const& other) = default;
    Intent& operator=(Intent const& other) = default;
};

typedef std::shared_ptr<Intent> IntentPtr;

}
}

#endif // XANTHUS_INTENT_INTENT_HPP
