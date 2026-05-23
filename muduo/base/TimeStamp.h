#ifndef MUDUO_BASE_TIMESTAMP_H
#define MUDUO_BASE_TIMESTAMP_H

#include "muduo/base/copyable.h"
#include "muduo/base/Types.h"

namespace muduo
{
    class TimeStamp : public copyable
    {
    public:
        TimeStamp() : microSecondsSinceEpoch_(0) {}
        explicit TimeStamp(int64_t microSecondsSinceEpochArg) : microSecondsSinceEpoch_(microSecondsSinceEpochArg) {}

        void swap(TimeStamp &that)
        {
            std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
        }

        string toString() const;
        string toFormattedString(bool showMicroseconds = true) const;

        bool valid() const { return microSecondsSinceEpoch_ > 0; }

        int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
        time_t secondsSinceEpoch() const { return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond); }

        static TimeStamp now();
        static TimeStamp invalid() { return TimeStamp(); }
        static TimeStamp fromUnixTime(time_t t) { return fromUnixTime(t, 0); }
        static TimeStamp fromUnixTime(time_t t, int microseconds)
        {
            return TimeStamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
        }
    private:
        static const int kMicroSecondsPerSecond = 1000 * 1000;
        int64_t microSecondsSinceEpoch_;
    };
}
#endif // MUDUO_BASE_TIMESTAMP_H